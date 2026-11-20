/* MSHV-IC8TEM  Power & SWR window
 * See hvpowerswrw.h.
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include "hvpowerswrw.h"
#include "../HvRigControl/HvRigCat/rigdef.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QPainter>
#include <QCloseEvent>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QFontDatabase>
#include <QApplication>
#include <QClipboard>
#include <QScreen>
#include <QShowEvent>
#include "../HvRigControl/qexsp_1_2rc/qextserialenumerator.h"

// ---------------------------------------------------------------- meter
HvPwrMeter::HvPwrMeter(int kind,QWidget *parent)
        : QWidget(parent)
{
    s_kind = kind;
    s_val = 0.0;
    s_peak = -1.0;
    s_limit = -1.0;
    setMinimumHeight(18);
    setMinimumWidth(110);
}
void HvPwrMeter::SetValue(double v,double peak)
{
    s_val = v;
    s_peak = peak;
    update();
}
void HvPwrMeter::SetLimit(double l)
{
    s_limit = l;
    update();
}
static double SwrPos(double swr)   // 1.0..3.0+ -> 0..1
{
    double p = (swr - 1.0) / 2.0;
    if (p < 0.0) p = 0.0;
    if (p > 1.0) p = 1.0;
    return p;
}
void HvPwrMeter::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    const int w = width(), h = height();
    if (s_kind == 1)
    {
        // zones: 1.0-1.8 green, 1.8-2.4 yellow, 2.4-3.0 red
        p.fillRect(0,0,(int)(w*0.40),h,QColor(187,247,208));
        p.fillRect((int)(w*0.40),0,(int)(w*0.30)+1,h,QColor(253,230,138));
        p.fillRect((int)(w*0.70),0,w-(int)(w*0.70),h,QColor(254,202,202));
        double sp = SwrPos(s_val);
        QColor c = s_val < 1.8 ? QColor(21,128,61) : (s_val < 2.4 ? QColor(180,83,9) : QColor(185,28,28));
        if (s_val > 1.0) p.fillRect(0,3,(int)(w*sp),h-6,c);
        if (s_limit > 1.0)
        {
            int x = (int)(w*SwrPos(s_limit));
            p.fillRect(x-1,0,2,h,QColor(153,27,27));
        }
    }
    else
    {
        p.fillRect(0,0,w,h,QColor(229,231,235));
        double v = s_val;
        if (v < 0.0) v = 0.0;
        if (v > 1.0) v = 1.0;
        QColor c = (s_kind == 0) ? QColor(29,78,216) : (s_val > 0.5 ? QColor(185,28,28) : QColor(75,85,99));
        p.fillRect(0,0,(int)(w*v),h,c);
        if (s_peak > 0.0)
        {
            int x = (int)(w*qMin(1.0,s_peak));
            p.fillRect(x-1,0,3,h,QColor(17,24,39));
        }
    }
    p.setPen(QColor(107,114,128));
    p.drawRect(0,0,w-1,h-1);
}

// ---------------------------------------------------------------- window
static const char *bands_names[] = {"160 m","80 m","60 m","40 m","30 m","20 m","17 m","15 m","12 m","10 m",
                                    "6 m","4 m","2 m","1.25 m","70 cm","23 cm"};
static const double bands_lo[] = {1.8,3.5,5.2,7.0,10.1,14.0,18.0,21.0,24.8,28.0,50.0,69.9,144.0,219.0,420.0,1240.0};
static const double bands_hi[] = {2.0,4.0,5.5,7.3,10.2,14.4,18.2,21.5,25.0,29.7,54.0,71.0,148.0,226.0,450.0,1300.0};
#define PWR_NBANDS 16

QString HvPowerSwrW::BandOf(double mhz)
{
    for (int i = 0; i < PWR_NBANDS; ++i)
        if (mhz >= bands_lo[i] && mhz <= bands_hi[i]) return QString(bands_names[i]);
    return QString();
}
double HvPowerSwrW::PoFraction(int raw)   // Icom Po meter: 0=0%, 143=50%, 213=100%
{
    if (raw <= 0) return 0.0;
    if (raw <= 143) return 0.5 * raw / 143.0;
    return 0.5 + 0.5 * (raw - 143) / 70.0;
}
double HvPowerSwrW::SwrValue(int raw)     // Icom SWR meter: 0=1.0, 48=1.5, 80=2.0, 120=3.0
{
    if (raw <= 0) return 1.0;
    if (raw <= 48) return 1.0 + 0.5 * raw / 48.0;
    if (raw <= 80) return 1.5 + 0.5 * (raw - 48) / 32.0;
    return 2.0 + (raw - 80) / 40.0;
}

HvPowerSwrW::HvPowerSwrW(QString app_path,QWidget *parent)
        : QWidget(parent)
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    setWindowTitle(tr("Power and SWR"));
    s_set_path = app_path+"/settings/ms_pwrswr";
    f_block = true;
    f_capable = false;
    f_cat_capable = false;
    f_link_ok = false;
    f_tx = false;
    f_blocked = false;
    f_tune = false;
    s_saved_pct = 50;
    tx_since = 0;
    swr_high_since = -1;
    po_peak = 0.0;
    po_peak_at = 0;
    last_po = 0.0;
    last_swr = 1.0;
    last_alc = 0.0;
    f_was_visible = false;
    f_closing = false;
    t_fit = 0;
    f_fit_exact = false;
    clock.start();
    QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    // ---------------- tab 1
    QWidget *w1 = new QWidget();
    l_head = new QLabel();
    l_tx = new QLabel(" TX ");
    l_tx->setStyleSheet("QLabel{background-color:rgb(156,163,175);color:white;font-weight:bold;padding:2px 8px;}");
    QHBoxLayout *H_head = new QHBoxLayout();
    H_head->addWidget(l_head,1);
    H_head->addWidget(l_tx);

    l_notcap = new QLabel(tr("Not available: choose an Icom radio (CI-V on USB or Icom LAN) in Rig Control and start the CAT.")+"\n"+
                          tr("Or, in Settings, Connection: Hamlib rigctld (any radio) or Icom CI-V on its own serial port, also without the MSHV CAT."));
    l_notcap->setWordWrap(true);
    l_notcap->setStyleSheet("QLabel{background-color:rgb(254,243,199);color:rgb(120,53,15);padding:4px;}");

    QGroupBox *gb_pwr = new QGroupBox(tr("Output Power"));
    l_pct = new QLabel("50 %");
    QFont fb = mono;
    fb.setPointSize(fb.pointSize()+6);
    fb.setBold(true);
    l_pct->setFont(fb);
    l_pct->setMinimumWidth(QFontMetrics(fb).boundingRect("100 %").width()+6);//no jumps while the slider moves
    l_watt = new QLabel();
    QHBoxLayout *H_val = new QHBoxLayout();
    H_val->addWidget(l_pct);
    H_val->addWidget(l_watt,1);
    pb_minus = new QPushButton("-");
    pb_minus->setFixedSize(30,24);
    pb_minus->setToolTip(tr("Decrease power"));
    pb_plus = new QPushButton("+");
    pb_plus->setFixedSize(30,24);
    pb_plus->setToolTip(tr("Increase power"));
    sl_pwr = new QSlider(Qt::Horizontal);
    sl_pwr->setRange(0,100);
    sl_pwr->setValue(50);
    sl_pwr->setPageStep(5);
    QHBoxLayout *H_sl = new QHBoxLayout();
    H_sl->addWidget(pb_minus);
    H_sl->addWidget(sl_pwr,1);
    H_sl->addWidget(pb_plus);
    QHBoxLayout *H_pre = new QHBoxLayout();
    H_pre->setSpacing(3);
    const int presets[] = {10,25,35,50,100};
    const int pre_w = QFontMetrics(font()).boundingRect("100 %").width()+14;
    for (int i = 0; i < 5; ++i)
    {
        QPushButton *b = new QPushButton(QString("%1 %").arg(presets[i]));
        b->setProperty("pct",presets[i]);
        //the style gives push buttons at least 75-80 pixels: here they share the width of the row
        b->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Fixed);
        b->setMinimumWidth(pre_w);
        connect(b, SIGNAL(clicked()), this, SLOT(Preset()));
        pb_presets << b;
        H_pre->addWidget(b);
    }
    l_mem = new QLabel();
    pb_save_band = new QPushButton(tr("Save for this band"));
    QHBoxLayout *H_mem = new QHBoxLayout();
    H_mem->addWidget(l_mem,1);
    H_mem->addWidget(pb_save_band);
    pb_tune = new QPushButton(tr("Tune"));
    pb_read = new QPushButton(tr("Read from radio"));
    QHBoxLayout *H_tb = new QHBoxLayout();
    H_tb->addWidget(pb_tune);
    H_tb->addWidget(pb_read);
    QVBoxLayout *V_pwr = new QVBoxLayout();
    V_pwr->setContentsMargins(8,4,8,6);
    V_pwr->setSpacing(5);
    V_pwr->addLayout(H_val);
    V_pwr->addLayout(H_sl);
    V_pwr->addLayout(H_pre);
    V_pwr->addLayout(H_mem);
    V_pwr->addStretch(1);
    V_pwr->addLayout(H_tb);
    gb_pwr->setLayout(V_pwr);

    QGroupBox *gb_met = new QGroupBox(tr("Measurements while transmitting"));
    m_po = new HvPwrMeter(0);
    m_swr = new HvPwrMeter(1);
    m_alc = new HvPwrMeter(2);
    l_po = new QLabel("0 W");
    l_swr = new QLabel("1.0");
    l_alc = new QLabel("0 %");
    QFont fm = mono;
    fm.setBold(true);
    fm.setPointSize(fm.pointSize()+2);
    l_po->setFont(fm);
    l_swr->setFont(fm);
    l_alc->setFont(fm);
    l_po->setMinimumWidth(QFontMetrics(fm).boundingRect("2000 W").width()+4);//no jumps when the values change
    QGridLayout *G_met = new QGridLayout();
    G_met->setVerticalSpacing(4);
    G_met->addWidget(new QLabel("Po"),0,0);
    G_met->addWidget(m_po,0,1);
    G_met->addWidget(l_po,0,2);
    G_met->addWidget(new QLabel(tr("SWR")),1,0);
    G_met->addWidget(m_swr,1,1);
    G_met->addWidget(l_swr,1,2);
    G_met->addWidget(new QLabel("ALC"),2,0);
    G_met->addWidget(m_alc,2,1);
    G_met->addWidget(l_alc,2,2);
    G_met->setColumnStretch(1,1);
    cb_prot_main = new QCheckBox();
    l_event = new QLabel();
    l_event->setFont(mono);
    l_event->setWordWrap(true);
    l_event->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    l_event->setMinimumHeight(2*QFontMetrics(mono).lineSpacing()+2);//2 lines: the window does not jump with long messages
    l_block = new QLabel();
    l_block->setWordWrap(true);
    l_block->setStyleSheet("QLabel{background-color:rgb(153,27,27);color:white;padding:4px;font-weight:bold;}");
    pb_unlock = new QPushButton(tr("Unlock"));
    QHBoxLayout *H_blk = new QHBoxLayout();
    H_blk->addWidget(l_block,1);
    H_blk->addWidget(pb_unlock);
    QVBoxLayout *V_met = new QVBoxLayout();
    V_met->setContentsMargins(8,4,8,6);
    V_met->setSpacing(5);
    V_met->addLayout(G_met);
    V_met->addStretch(1);
    V_met->addWidget(cb_prot_main);
    V_met->addWidget(l_event);
    gb_met->setLayout(V_met);

    QHBoxLayout *H_main = new QHBoxLayout();
    H_main->setSpacing(6);
    H_main->addWidget(gb_pwr,1);
    H_main->addWidget(gb_met,1);
    l_status = new QLabel();
    l_status->setFont(mono);
    l_status->setWordWrap(true);//long CAT messages wrap instead of widening the window
    l_status->setStyleSheet("QLabel{color:rgb(55,65,81);}");
    QVBoxLayout *V_w1 = new QVBoxLayout(w1);
    V_w1->setContentsMargins(6,6,6,6);
    V_w1->setSpacing(5);
    V_w1->addLayout(H_head);
    V_w1->addWidget(l_status);
    V_w1->addWidget(l_notcap);
    V_w1->addLayout(H_main,1);
    V_w1->addLayout(H_blk);//SWR protection message: full width, under the two boxes
    w1->setLayout(V_w1);

    // ---------------- tab 2
    QWidget *w2 = new QWidget();
    sb_maxw = new QSpinBox();
    sb_maxw->setRange(1,2000);
    sb_maxw->setValue(100);
    sb_maxw->setSuffix(" W");
    sb_interval = new QSpinBox();
    sb_interval->setRange(100,2000);
    sb_interval->setSingleStep(50);
    sb_interval->setValue(200);
    sb_interval->setSuffix(" ms");
    cb_only_tx = new QCheckBox(tr("Read only while transmitting"));
    cb_only_tx->setChecked(true);
    QGridLayout *H_s1 = new QGridLayout();//2 rows: the Settings tab is not too wide
    H_s1->addWidget(new QLabel(tr("Maximum power of the radio")+":"),0,0);
    H_s1->addWidget(sb_maxw,0,1);
    H_s1->addWidget(new QLabel(tr("Read the meters every")+":"),1,0);
    H_s1->addWidget(sb_interval,1,1);
    H_s1->addWidget(cb_only_tx,1,2);
    H_s1->setColumnStretch(3,1);

    QGroupBox *gb_bands = new QGroupBox(tr("Power per band"));
    tw_bands = new QTableWidget(PWR_NBANDS,3);
    QStringList hl;
    hl << tr("Band") << tr("Power %") << tr("Tune %");
    tw_bands->setHorizontalHeaderLabels(hl);
    tw_bands->verticalHeader()->setVisible(false);
    tw_bands->horizontalHeader()->setStretchLastSection(true);
    cb_apply_band = new QCheckBox(tr("Set the stored power when the band changes"));
    cb_apply_band->setChecked(true);
    cb_per_mode = new QCheckBox(tr("Separate memory for each mode (e.g. 2 m MSK144 and 2 m FT8)"));
    QVBoxLayout *V_b = new QVBoxLayout();
    V_b->addWidget(tw_bands,1);
    V_b->addWidget(cb_apply_band);
    V_b->addWidget(cb_per_mode);
    gb_bands->setLayout(V_b);

    QGroupBox *gb_prot = new QGroupBox(tr("Protection"));
    cb_prot = new QCheckBox(tr("Stop TX if SWR is above"));
    cb_prot->setChecked(true);
    sb_swr_lim = new QDoubleSpinBox();
    sb_swr_lim->setRange(1.2,5.0);
    sb_swr_lim->setSingleStep(0.1);
    sb_swr_lim->setDecimals(1);
    sb_swr_lim->setValue(2.5);
    sb_swr_sec = new QDoubleSpinBox();
    sb_swr_sec->setRange(0.2,10.0);
    sb_swr_sec->setSingleStep(0.1);
    sb_swr_sec->setDecimals(1);
    sb_swr_sec->setValue(1.0);
    sb_swr_sec->setSuffix(" s");
    QHBoxLayout *H_p = new QHBoxLayout();
    H_p->addWidget(cb_prot);
    H_p->addWidget(sb_swr_lim);
    H_p->addWidget(new QLabel(tr("for more than")));
    H_p->addWidget(sb_swr_sec);
    H_p->addStretch(1);
    cb_alc_warn = new QCheckBox(tr("Warn if ALC is above 50 % (audio too high in digital modes)"));
    cb_alc_warn->setChecked(true);
    cb_po_warn = new QCheckBox(tr("Warn if Po stays below 50 % of the set power"));
    QVBoxLayout *V_p = new QVBoxLayout();
    V_p->addLayout(H_p);
    V_p->addWidget(cb_alc_warn);
    V_p->addWidget(cb_po_warn);
    gb_prot->setLayout(V_p);

    QLabel *l_xv = new QLabel(tr("With a transverter the radio measures the drive power and the SWR towards the transverter, not those at the antenna."));
    l_xv->setWordWrap(true);
    l_xv->setStyleSheet("QLabel{background-color:rgb(254,243,199);color:rgb(120,53,15);padding:4px;}");

    // connection: MSHV CAT or own connection
    QGroupBox *gb_conn = new QGroupBox(tr("Connection"));
    cb_conn = new QComboBox();
    cb_conn->addItem(tr("MSHV CAT (Icom CI-V on USB or Icom LAN)"));
    cb_conn->addItem(tr("Hamlib rigctld (TCP) - any radio, also without the MSHV CAT"));
    cb_conn->addItem(tr("Icom CI-V on its own serial port, without the MSHV CAT"));
    le_host = new QLineEdit("127.0.0.1");
    le_host->setMaximumWidth(150);
    sb_port = new QSpinBox();
    sb_port->setRange(1,65535);
    sb_port->setValue(4532);
    cb_com = new QComboBox();
    cb_com->setEditable(true);
    cb_com->setMinimumWidth(110);
    QPushButton *pb_rescan = new QPushButton(tr("Refresh"));
    cb_baud = new QComboBox();
    QStringList bl;
    bl << "4800" << "9600" << "19200" << "38400" << "57600" << "115200";
    cb_baud->addItems(bl);
    cb_baud->setCurrentIndex(5);
    le_civ = new QLineEdit("94");
    le_civ->setMaximumWidth(50);
    le_civ->setInputMask("HH");
    le_civ->setToolTip(tr("CI-V address of the radio in hex: IC-7300 94, IC-7300MK2 B6, IC-7610 98, IC-705 A4, IC-9700 A2, IC-7100 88, IC-905 AC"));
    pb_conn = new QPushButton(tr("Connect"));
    l_conn = new QLabel();
    l_conn->setWordWrap(true);
    l_conn->setFont(mono);
    QHBoxLayout *H_c1 = new QHBoxLayout();
    H_c1->addWidget(cb_conn,1);
    H_c1->addWidget(pb_conn);
    QGridLayout *H_c2 = new QGridLayout();//rigctld and CI-V on 2 rows: the Settings tab is not too wide
    H_c2->addWidget(new QLabel("rigctld:"),0,0);
    QHBoxLayout *H_c2a = new QHBoxLayout();
    H_c2a->addWidget(le_host);
    H_c2a->addWidget(new QLabel(tr("Port")+":"));
    H_c2a->addWidget(sb_port);
    H_c2a->addStretch(1);
    H_c2->addLayout(H_c2a,0,1);
    H_c2->addWidget(new QLabel("CI-V:"),1,0);
    QHBoxLayout *H_c2b = new QHBoxLayout();
    H_c2b->addWidget(cb_com);
    H_c2b->addWidget(pb_rescan);
    H_c2b->addWidget(cb_baud);
    H_c2b->addWidget(new QLabel(tr("Address")+":"));
    H_c2b->addWidget(le_civ);
    H_c2b->addStretch(1);
    H_c2->addLayout(H_c2b,1,1);
    H_c2->setColumnStretch(1,1);
    QLabel *l_cinfo = new QLabel(tr("A serial port can be used by one program only: with the own CI-V port the MSHV CAT must use another port or be off. Hamlib rigctld can be shared by several programs (e.g. rigctld -m 3073 -r COM5 -s 115200 for the IC-7300)."));
    l_cinfo->setWordWrap(true);
    QVBoxLayout *V_c = new QVBoxLayout();
    V_c->addLayout(H_c1);
    V_c->addLayout(H_c2);
    V_c->addWidget(l_conn);
    V_c->addWidget(l_cinfo);
    gb_conn->setLayout(V_c);
    connect(cb_conn, SIGNAL(currentIndexChanged(int)), this, SLOT(ConnChanged()));
    connect(pb_conn, SIGNAL(clicked()), this, SLOT(ConnApply()));
    connect(pb_rescan, SIGNAL(clicked()), this, SLOT(RefreshCom()));
    link = new HvPwrLink(this);
    connect(link, SIGNAL(Status(bool,QString)), this, SLOT(LinkStatus(bool,QString)));
    connect(link, SIGNAL(LogLine(bool,QString)), this, SLOT(LinkLog(bool,QString)));
    connect(link, SIGNAL(RfPct(int)), this, SLOT(OnRfPct(int)));
    connect(link, SIGNAL(Po(double)), this, SLOT(OnPo(double)));
    connect(link, SIGNAL(Swr(double)), this, SLOT(OnSwr(double)));
    connect(link, SIGNAL(Alc(double)), this, SLOT(OnAlc(double)));
    connect(link, SIGNAL(FreqHz(double)), this, SLOT(LinkFreq(double)));
    t_freq = new QTimer(this);
    connect(t_freq, SIGNAL(timeout()), this, SLOT(PollFreq()));
    t_freq->start(3000);
    RefreshCom();

    QVBoxLayout *V_w2 = new QVBoxLayout(w2);
    V_w2->addWidget(gb_conn);
    V_w2->addLayout(H_s1);
    V_w2->addWidget(gb_bands,1);
    V_w2->addWidget(gb_prot);
    V_w2->addWidget(l_xv);
    w2->setLayout(V_w2);

    t_fit = new QTimer(this);
    t_fit->setSingleShot(true);
    connect(t_fit, SIGNAL(timeout()), this, SLOT(FitNow()));
    tabs = new QTabWidget();
    tabs->addTab(TabPage(w1),tr("Power and SWR"));
    tabs->addTab(TabPage(w2),tr("Settings"));
    // ---------------- tab 3: CAT log
    QWidget *w3 = new QWidget();
    QLabel *l_loginfo = new QLabel(tr("CI-V frames of power and meters sent to the radio and its answers. FA = the radio refused the command (NG)."));
    l_loginfo->setWordWrap(true);
    te_log = new QPlainTextEdit();
    te_log->setReadOnly(true);
    te_log->setMaximumBlockCount(1000);
    te_log->setFont(mono);
    te_log->setMinimumSize(420,200);//the window grows to this when the CAT Log tab is opened
    QPushButton *pb_readnow = new QPushButton(tr("Read now (power and meters)"));
    QPushButton *pb_clear = new QPushButton(tr("Clear"));
    QPushButton *pb_copy = new QPushButton(tr("Copy"));
    cb_log_pause = new QCheckBox(tr("Pause"));
    QHBoxLayout *H_log = new QHBoxLayout();
    H_log->addWidget(pb_readnow);
    H_log->addStretch(1);
    H_log->addWidget(cb_log_pause);
    H_log->addWidget(pb_copy);
    H_log->addWidget(pb_clear);
    QVBoxLayout *V_w3 = new QVBoxLayout(w3);
    V_w3->addWidget(l_loginfo);
    V_w3->addWidget(te_log,1);
    V_w3->addLayout(H_log);
    w3->setLayout(V_w3);
    tabs->addTab(TabPage(w3),tr("CAT Log"));
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(TabChanged(int)));
    TabChanged(tabs->currentIndex());
    connect(pb_readnow, SIGNAL(clicked()), this, SLOT(ReadNow()));
    connect(pb_clear, SIGNAL(clicked()), this, SLOT(LogClear()));
    connect(pb_copy, SIGNAL(clicked()), this, SLOT(LogCopy()));
    QVBoxLayout *V_all = new QVBoxLayout(this);
    V_all->setContentsMargins(6,6,6,6);
    V_all->addWidget(tabs);
    setLayout(V_all);

    t_send = new QTimer(this);
    t_send->setSingleShot(true);
    connect(t_send, SIGNAL(timeout()), this, SLOT(SendPower()));
    t_poll = new QTimer(this);
    connect(t_poll, SIGNAL(timeout()), this, SLOT(PollMeters()));
    t_cap = new QTimer(this);//ask the rig state every second (CAT may start before this window is connected)
    connect(t_cap, SIGNAL(timeout()), this, SIGNAL(EmitAskCapable()));
    t_cap->start(1000);

    connect(sl_pwr, SIGNAL(valueChanged(int)), this, SLOT(SliderChanged(int)));
    connect(pb_minus, SIGNAL(clicked()), this, SLOT(Minus()));
    connect(pb_plus, SIGNAL(clicked()), this, SLOT(Plus()));
    connect(pb_save_band, SIGNAL(clicked()), this, SLOT(SaveBand()));
    connect(pb_read, SIGNAL(clicked()), this, SLOT(ReadFromRadio()));
    connect(pb_tune, SIGNAL(clicked()), this, SLOT(TuneClicked()));
    connect(pb_unlock, SIGNAL(clicked()), this, SLOT(Unlock()));
    connect(cb_prot_main, SIGNAL(toggled(bool)), cb_prot, SLOT(setChecked(bool)));
    connect(cb_prot, SIGNAL(toggled(bool)), cb_prot_main, SLOT(setChecked(bool)));
    connect(sb_maxw, SIGNAL(valueChanged(int)), this, SLOT(SettingsChanged()));
    connect(sb_interval, SIGNAL(valueChanged(int)), this, SLOT(SettingsChanged()));
    connect(cb_only_tx, SIGNAL(toggled(bool)), this, SLOT(SettingsChanged()));
    connect(cb_apply_band, SIGNAL(toggled(bool)), this, SLOT(SettingsChanged()));
    connect(cb_per_mode, SIGNAL(toggled(bool)), this, SLOT(SettingsChanged()));
    connect(cb_prot, SIGNAL(toggled(bool)), this, SLOT(SettingsChanged()));
    connect(sb_swr_lim, SIGNAL(valueChanged(double)), this, SLOT(SettingsChanged()));
    connect(sb_swr_sec, SIGNAL(valueChanged(double)), this, SLOT(SettingsChanged()));
    connect(cb_alc_warn, SIGNAL(toggled(bool)), this, SLOT(SettingsChanged()));
    connect(cb_po_warn, SIGNAL(toggled(bool)), this, SLOT(SettingsChanged()));
    connect(tw_bands, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(TableChanged(QTableWidgetItem*)));

    ReadSettings();
    cb_prot_main->setChecked(cb_prot->isChecked());
    FillTable();
    f_block = false;
    SettingsChanged();
    UpdatePowerLabels();
    UpdateEnabled();
    pb_unlock->setHidden(true);
    l_block->setHidden(true);
    Event(QString(),false);
    UpdateConnWidgets();
    if (cb_conn->currentIndex() != 0) QTimer::singleShot(500, this, SLOT(ConnApply()));
}
HvPowerSwrW::~HvPowerSwrW()
{}
bool HvPowerSwrW::WasVisible()
{
    return f_was_visible;
}
void HvPowerSwrW::closeEvent(QCloseEvent *e)
{
    f_closing = true;
    SaveSettings();
    f_closing = false;
    emit EmitClosed();
    e->accept();
}
// Size of the window = size of the open tab. A QTabWidget is as big as its biggest
// tab (Settings): the content of the tabs not shown is hidden, so it does not count,
// and the Power and SWR tab gets only the space it needs.
QWidget *HvPowerSwrW::TabPage(QWidget *content)
{
    QWidget *p = new QWidget();
    QVBoxLayout *v = new QVBoxLayout(p);
    v->setContentsMargins(0,0,0,0);
    v->addWidget(content);
    tab_pages << content;
    return p;
}
void HvPowerSwrW::TabChanged(int idx)
{
    for (int i = 0; i < tab_pages.count(); ++i) tab_pages.at(i)->setVisible(i == idx);
    FitLater(true);
}
void HvPowerSwrW::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    if (e->spontaneous()) return;//restored from the taskbar: keep the size
    Fit(true);//before the window appears
    FitLater(true);//and again when everything is laid out
}
void HvPowerSwrW::FitLater(bool exact)
{
    if (exact) f_fit_exact = true;
    if (t_fit && isVisible()) t_fit->start(0);
}
void HvPowerSwrW::FitNow()
{
    bool exact = f_fit_exact;
    f_fit_exact = false;
    Fit(exact);
}
void HvPowerSwrW::Fit(bool exact)
{
    // Power and SWR tab: exact = as small as its content, otherwise same width and taller if needed.
    // Other tabs: big enough for their content (never smaller than now).
    if (!isVisible() || isMinimized() || isMaximized() || isFullScreen()) return;
    QCoreApplication::sendPostedEvents(0, QEvent::LayoutRequest);//sizes of the tab just changed
    tabs->updateGeometry();
    if (layout()) layout()->activate();
    const QSize min = minimumSizeHint();
    const bool main_tab = (tabs->currentIndex() == 0);
    int w = (main_tab && exact) ? sizeHint().width() : width();
    if (!main_tab) w = qMax(w, sizeHint().width());
    w = qMax(w, min.width());
    int h = hasHeightForWidth() ? heightForWidth(w) : -1;
    if (h <= 0) h = sizeHint().height();
    h = qMax(h, min.height());
    if (!(main_tab && exact)) h = qMax(h, height());
    QSize s(w,h);
    QScreen *sc = QGuiApplication::screenAt(frameGeometry().center());
    if (!sc) sc = QGuiApplication::primaryScreen();
    if (sc)
    {
        QRect av = sc->availableGeometry();
        QSize fr = frameGeometry().size() - size();//title bar and borders
        s = s.boundedTo(av.size() - fr).expandedTo(min);
        QRect g(frameGeometry().topLeft(), s + fr);
        int x = g.x(), y = g.y();
        if (g.right() > av.right()) x = qMax(av.left(), av.right() - g.width() + 1);
        if (g.bottom() > av.bottom()) y = qMax(av.top(), av.bottom() - g.height() + 1);
        if (x != g.x() || y != g.y()) move(x,y);
    }
    if (s != size()) resize(s);
}
void HvPowerSwrW::ReadSettings()
{
    QFile f(s_set_path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&f);
    while (!in.atEnd())
    {
        QString l = in.readLine().trimmed();
        int eq = l.indexOf('=');
        if (eq <= 0) continue;
        QString k = l.left(eq), v = l.mid(eq+1);
        if (k=="maxw") sb_maxw->setValue(v.toInt());
        else if (k=="interval") sb_interval->setValue(v.toInt());
        else if (k=="only_tx") cb_only_tx->setChecked(v=="1");
        else if (k=="apply_band") cb_apply_band->setChecked(v=="1");
        else if (k=="per_mode") cb_per_mode->setChecked(v=="1");
        else if (k=="prot") cb_prot->setChecked(v=="1");
        else if (k=="swr_lim") sb_swr_lim->setValue(v.toDouble());
        else if (k=="swr_sec") sb_swr_sec->setValue(v.toDouble());
        else if (k=="alc_warn") cb_alc_warn->setChecked(v=="1");
        else if (k=="po_warn") cb_po_warn->setChecked(v=="1");
        else if (k=="pct") sl_pwr->setValue(v.toInt());
        else if (k=="conn") cb_conn->setCurrentIndex(qBound(0,v.toInt(),2));
        else if (k=="host") le_host->setText(v);
        else if (k=="port") sb_port->setValue(v.toInt());
        else if (k=="com") cb_com->setEditText(v);
        else if (k=="baud") { int bi = cb_baud->findText(v); if (bi >= 0) cb_baud->setCurrentIndex(bi); }
        else if (k=="civ") le_civ->setText(v);
        else if (k=="geom") restoreGeometry(QByteArray::fromBase64(v.toLatin1()));
        else if (k=="visible") f_was_visible = (v=="1");
        else if (k.startsWith("mem:")) mem_pwr.insert(k.mid(4),v.toInt());
        else if (k.startsWith("tune:")) mem_tune.insert(k.mid(5),v.toInt());
    }
    f.close();
    cb_prot_main->setChecked(cb_prot->isChecked());
}
void HvPowerSwrW::SaveSettings()
{
    if (f_block) return;
    QFile f(s_set_path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&f);
    out << "maxw=" << sb_maxw->value() << "\n";
    out << "interval=" << sb_interval->value() << "\n";
    out << "only_tx=" << (cb_only_tx->isChecked() ? "1" : "0") << "\n";
    out << "apply_band=" << (cb_apply_band->isChecked() ? "1" : "0") << "\n";
    out << "per_mode=" << (cb_per_mode->isChecked() ? "1" : "0") << "\n";
    out << "prot=" << (cb_prot->isChecked() ? "1" : "0") << "\n";
    out << "swr_lim=" << QString::number(sb_swr_lim->value(),'f',1) << "\n";
    out << "swr_sec=" << QString::number(sb_swr_sec->value(),'f',1) << "\n";
    out << "alc_warn=" << (cb_alc_warn->isChecked() ? "1" : "0") << "\n";
    out << "po_warn=" << (cb_po_warn->isChecked() ? "1" : "0") << "\n";
    out << "pct=" << (f_tune ? s_saved_pct : sl_pwr->value()) << "\n";
    out << "conn=" << cb_conn->currentIndex() << "\n";
    out << "host=" << le_host->text().trimmed() << "\n";
    out << "port=" << sb_port->value() << "\n";
    out << "com=" << cb_com->currentText().trimmed() << "\n";
    out << "baud=" << cb_baud->currentText() << "\n";
    out << "civ=" << le_civ->text().trimmed() << "\n";
    out << "geom=" << QString::fromLatin1(saveGeometry().toBase64()) << "\n";
    out << "visible=" << ((isVisible() && !f_closing) ? "1" : "0") << "\n";
    for (QMap<QString,int>::const_iterator it = mem_pwr.constBegin(); it != mem_pwr.constEnd(); ++it)
        out << "mem:" << it.key() << "=" << it.value() << "\n";
    for (QMap<QString,int>::const_iterator it = mem_tune.constBegin(); it != mem_tune.constEnd(); ++it)
        out << "tune:" << it.key() << "=" << it.value() << "\n";
    f.close();
}
void HvPowerSwrW::FillTable()
{
    bool b = f_block;
    f_block = true;
    for (int i = 0; i < PWR_NBANDS; ++i)
    {
        QString bn = bands_names[i];
        QTableWidgetItem *it0 = new QTableWidgetItem(bn);
        it0->setFlags(Qt::ItemIsEnabled);
        tw_bands->setItem(i,0,it0);
        QTableWidgetItem *it1 = new QTableWidgetItem(mem_pwr.contains(bn) ? QString::number(mem_pwr.value(bn)) : QString());
        tw_bands->setItem(i,1,it1);
        QTableWidgetItem *it2 = new QTableWidgetItem(QString::number(mem_tune.value(bn,10)));
        tw_bands->setItem(i,2,it2);
    }
    f_block = b;
}
void HvPowerSwrW::TableChanged(QTableWidgetItem *it)
{
    if (f_block || !it) return;
    int r = it->row(), c = it->column();
    QString bn = bands_names[r];
    bool ok = false;
    int v = it->text().trimmed().toInt(&ok);
    if (c==1)
    {
        if (ok && v >= 0 && v <= 100) mem_pwr.insert(bn,v);
        else mem_pwr.remove(bn);
    }
    else if (c==2)
    {
        if (ok && v >= 0 && v <= 100) mem_tune.insert(bn,v);
        else mem_tune.remove(bn);
    }
    UpdatePowerLabels();
    SaveSettings();
}
void HvPowerSwrW::SettingsChanged()
{
    if (f_block) return;
    m_swr->SetLimit(cb_prot->isChecked() ? sb_swr_lim->value() : -1.0);
    cb_prot_main->setText(tr("Stop TX if SWR > %1 for %2 s").arg(QString::number(sb_swr_lim->value(),'f',1))
                          .arg(QString::number(sb_swr_sec->value(),'f',1)));//short: the window stays small
    t_poll->start(sb_interval->value());
    UpdatePowerLabels();
    SaveSettings();
}
QString HvPowerSwrW::MemKey()
{
    if (s_band.isEmpty()) return QString();
    if (cb_per_mode->isChecked() && !s_mode.isEmpty()) return s_band+"_"+s_mode;
    return s_band;
}
void HvPowerSwrW::UpdatePowerLabels()
{
    int pct = sl_pwr->value();
    l_pct->setText(QString("%1 %").arg(pct));
    l_watt->setText(QString::fromUtf8("\xE2\x89\x88 ")+QString::number(pct*sb_maxw->value()/100.0,'f',0)+" W "+
                    tr("of")+" "+QString::number(sb_maxw->value())+" W");
    QString head = tr("Band")+": "+(s_band.isEmpty() ? QString("-") : s_band);
    if (!s_mode.isEmpty()) head += "   "+s_mode;
    l_head->setText(head);
    QString k = MemKey();
    if (k.isEmpty()) l_mem->setText(tr("Band memory")+": -");
    else l_mem->setText(tr("Band memory")+" "+k.replace("_"," ")+": "+
                        (mem_pwr.contains(MemKey()) ? QString::number(mem_pwr.value(MemKey()))+" %" : QString("-")));
    int tp = mem_tune.value(s_band,10);
    pb_tune->setText(f_tune ? tr("Stop Tune") : tr("Tune")+" ("+QString::number(tp*sb_maxw->value()/100.0,'f',0)+" W)");
}
void HvPowerSwrW::SliderChanged(int)
{
    UpdatePowerLabels();
    if (!f_block) t_send->start(250);
}
void HvPowerSwrW::SendPower()
{
    int lv = (int)(sl_pwr->value() * 255.0 / 100.0 + 0.5);
    RigCmd(SET_RFPOWER,QString::number(lv));
    SaveSettings();
}
void HvPowerSwrW::SetPct(int pct,bool send)
{
    bool b = f_block;
    if (!send) f_block = true;
    sl_pwr->setValue(pct);
    f_block = b;
    UpdatePowerLabels();
    if (send)
    {
        t_send->stop();
        SendPower();
    }
}
void HvPowerSwrW::Minus()
{
    sl_pwr->setValue(sl_pwr->value()-1);
}
void HvPowerSwrW::Plus()
{
    sl_pwr->setValue(sl_pwr->value()+1);
}
void HvPowerSwrW::Preset()
{
    QPushButton *b = qobject_cast<QPushButton*>(sender());
    if (b) SetPct(b->property("pct").toInt(),true);
}
void HvPowerSwrW::SaveBand()
{
    QString k = MemKey();
    if (k.isEmpty()) return;
    mem_pwr.insert(k,f_tune ? s_saved_pct : sl_pwr->value());
    FillTable();
    UpdatePowerLabels();
    SaveSettings();
}
void HvPowerSwrW::ApplyBandMemory()
{
    if (!cb_apply_band->isChecked() || f_tune) return;
    QString k = MemKey();
    if (k.isEmpty()) return;
    if (!mem_pwr.contains(k) && k != s_band && mem_pwr.contains(s_band)) k = s_band;
    if (!mem_pwr.contains(k)) return;
    SetPct(mem_pwr.value(k),f_capable);
}
void HvPowerSwrW::ReadFromRadio()
{
    RigCmd(GET_RFPOWER,QString());
}
void HvPowerSwrW::TuneClicked()
{
    if (!f_capable) return;
    if (!f_tune)
    {
        f_tune = true;
        s_saved_pct = sl_pwr->value();
        SetPct(mem_tune.value(s_band,10),true);
    }
    UpdatePowerLabels();
    emit EmitTune();
}
void HvPowerSwrW::Unlock()
{
    f_blocked = false;
    emit EmitTxBlocked(false);
    l_block->setHidden(true);
    pb_unlock->setHidden(true);
    Event(tr("TX unlocked"),false);
    FitLater(true);
}
void HvPowerSwrW::UpdateEnabled()
{
    if (l_notcap->isHidden() != f_capable)
    {
        l_notcap->setHidden(f_capable);
        FitLater(true);
    }
    QList<QWidget*> l;
    l << sl_pwr << pb_minus << pb_plus << pb_save_band << pb_tune << pb_read;
    for (int i = 0; i < pb_presets.count(); ++i) l << pb_presets.at(i);
    for (int i = 0; i < l.count(); ++i) l.at(i)->setEnabled(f_capable);
}
void HvPowerSwrW::Event(const QString &s,bool bad)
{
    if (s.isEmpty())
    {
        l_event->setText(QString());
        return;
    }
    l_event->setText(QTime::currentTime().toString("HH:mm:ss")+"  "+s);
    l_event->setStyleSheet(bad ? "QLabel{color:rgb(153,27,27);font-weight:bold;}" : "QLabel{color:rgb(55,65,81);}");
}
void HvPowerSwrW::SetCapable(bool f)
{
    f_cat_capable = f;
    UpdateCapable();
}
void HvPowerSwrW::UpdateCapable()
{
    bool f = (cb_conn->currentIndex() == 0) ? f_cat_capable : f_link_ok;
    bool was = f_capable;
    f_capable = f;
    UpdateEnabled();
    if (f && !was)
    {
        ReadFromRadio();
        ApplyBandMemory();
    }
}
void HvPowerSwrW::SetFreqStr(QString s)
{
    double hz = s.split("#").value(0).toDouble();
    QString b = BandOf(hz/1000000.0);
    if (b == s_band) return;
    s_band = b;
    AutoMaxW();
    UpdatePowerLabels();
    ApplyBandMemory();
}
void HvPowerSwrW::AutoMaxW()//IC8TEM v28 IC-9700: the Po meter full scale is the power of the band
{
    if (cb_conn->currentIndex() != 0 || !s_cat_status.contains("9700")) return;
    int w = 0;
    if (s_band == "2 m") w = 100;
    else if (s_band == "70 cm") w = 75;
    else if (s_band == "23 cm") w = 10;
    if (w > 0 && sb_maxw->value() != w) sb_maxw->setValue(w);
}
void HvPowerSwrW::SetModeName(QString m)
{
    if (m == s_mode) return;
    s_mode = m;
    UpdatePowerLabels();
    if (cb_per_mode->isChecked()) ApplyBandMemory();
}
void HvPowerSwrW::SetTx(bool f)
{
    if (f == f_tx) return;
    f_tx = f;
    if (f)
    {
        tx_since = clock.elapsed();
        swr_high_since = -1;
        po_peak = 0.0;
        l_tx->setStyleSheet("QLabel{background-color:rgb(185,28,28);color:white;font-weight:bold;padding:2px 8px;}");
        if (f_blocked)
        {
            Event(tr("TX blocked by the SWR protection: press Unlock"),true);
            QTimer::singleShot(0, this, SIGNAL(EmitStopTx()));
        }
    }
    else
    {
        l_tx->setStyleSheet("QLabel{background-color:rgb(156,163,175);color:white;font-weight:bold;padding:2px 8px;}");
        if (f_tune)
        {
            f_tune = false;
            SetPct(s_saved_pct,f_capable);
        }
        if (cb_only_tx->isChecked())
        {
            m_po->SetValue(0.0,po_peak > 0.0 ? po_peak : -1.0);
            m_alc->SetValue(0.0);
            m_swr->SetValue(1.0);
        }
        UpdatePowerLabels();
    }
}
void HvPowerSwrW::PollMeters()
{
    if (!f_capable || (!isVisible() && !cb_prot->isChecked())) return;
    if (cb_only_tx->isChecked() && !f_tx) return;
    RigCmd(GET_METERS,f_tx ? QString("tx") : QString());//IC8TEM v29: text CAT radios read Po only in TX
}
void HvPowerSwrW::SetStatus(QString s)
{
    s_cat_status = s;
    AutoMaxW();
    if (cb_conn->currentIndex() == 0 && l_status->text() != s)
    {
        l_status->setText(s);
        FitLater(false);
    }
}
void HvPowerSwrW::ReadNow()
{
    if (!f_capable)
    {
        Log(true,tr("not sent: Power and SWR not available with this rig")+" ("+l_status->text()+")");
        return;
    }
    RigCmd(GET_RFPOWER,QString());
    RigCmd(GET_METERS,f_tx ? QString("tx") : QString());//IC8TEM v29: text CAT radios read Po only in TX
}
void HvPowerSwrW::LogClear()
{
    te_log->clear();
}
void HvPowerSwrW::LogCopy()
{
    QApplication::clipboard()->setText(te_log->toPlainText());
}
QString HvPowerSwrW::Decode(const QString &hex)
{
    QStringList b = hex.split(' ',QString::SkipEmptyParts);
    if (b.count() < 6) return QString();
    QString c = b.at(4), sc = b.at(5);
    if (c=="FA") return tr("NG: the radio refused the command");
    if (c=="FB") return "OK";
    if ((c=="03" || c=="05" || c=="00") && b.count() >= 11)
    {
        double hz = 0.0, m = 1.0;
        for (int i = 5; i < 10; ++i)
        {
            bool ok = false;
            int v = b.at(i).toInt(&ok,16);
            if (!ok) return QString();
            hz += ((v>>4)*10 + (v&15)) * m;
            m *= 100.0;
        }
        return (c=="05" ? QString("set ") : QString())+QString::number(hz/1e6,'f',6)+" MHz";
    }
    if (c=="03") return "read frequency";
    if (c=="04" || c=="26") return (b.count() <= 7 && c=="26") || b.count() <= 6 ? QString("read mode") : QString("mode");
    if (c=="1C" && sc=="00") return b.count() >= 8 ? QString(b.at(6)=="01" ? "PTT ON" : "PTT OFF") : QString("read PTT");
    if (b.count() < 9) return (c=="14" && sc=="0A") ? tr("read RF power") :
                                  (c=="15" && sc=="11") ? tr("read Po") :
                                  (c=="15" && sc=="12") ? tr("read SWR") :
                                  (c=="15" && sc=="13") ? tr("read ALC") : QString();
    int v = (b.at(6)+b.at(7)).toInt();
    if (c=="14" && sc=="0A") return tr("RF power")+" "+QString::number(v)+" = "+QString::number((int)(v*100.0/255.0+0.5))+" %";
    if (c=="15" && sc=="11") return "Po "+QString::number(v)+" = "+QString::number(PoFraction(v)*sb_maxw->value(),'f',0)+" W";
    if (c=="15" && sc=="12") return tr("SWR")+" "+QString::number(v)+" = "+QString::number(SwrValue(v),'f',1);
    if (c=="15" && sc=="13") return "ALC "+QString::number(v)+" = "+QString::number(v/120.0*100.0,'f',0)+" %";
    return QString();
}
void HvPowerSwrW::Log(bool tx,const QString &hex)
{
    if (cb_log_pause->isChecked()) return;
    QString d = hex.contains("FE FE") ? Decode(hex) : QString();
    te_log->appendPlainText(QTime::currentTime().toString("HH:mm:ss.zzz")+(tx ? "  >> " : "  << ")+hex+
                            (d.isEmpty() ? QString() : "   "+d));
}
void HvPowerSwrW::SetRigInfo(int id,QString s)
{
    if (id == LOG_TX || id == LOG_RX)
    {
        Log(id == LOG_TX,s);
        return;
    }
    if (cb_conn->currentIndex() != 0) return;   // own connection in use
    if (s.startsWith("w:") && id == READ_PO)   // watts (TCI, FlexRadio, K4)
    {
        double maxw = sb_maxw->value() > 0 ? sb_maxw->value() : 100;
        OnPo(s.mid(2).toDouble() / maxw);
        return;
    }
    if (s.startsWith("f:"))   // Hamlib values: fractions 0..1, SWR as a number
    {
        double v = s.mid(2).toDouble();
        if (id == READ_RFPOWER) OnRfPct((int)(v * 100.0 + 0.5));
        else if (id == READ_PO) OnPo(v);
        else if (id == READ_ALC) OnAlc(v);
        else if (id == READ_SWR) OnSwr(v);
        return;
    }
    int raw = s.toInt();
    if (id == READ_RFPOWER) OnRfPct((int)(raw * 100.0 / 255.0 + 0.5));
    else if (id == READ_PO) OnPo(PoFraction(raw));
    else if (id == READ_ALC) OnAlc(raw / 120.0);
    else if (id == READ_SWR) OnSwr(SwrValue(raw));
}
void HvPowerSwrW::OnRfPct(int pct)
{
    if (!f_tune) SetPct(qBound(0,pct,100),false);
}
void HvPowerSwrW::OnPo(double frac)
{
    qint64 now = clock.elapsed();
    {
        last_po = qMax(0.0,frac);
        if (last_po > po_peak || now - po_peak_at > 2000)
        {
            po_peak = last_po;
            po_peak_at = now;
        }
        m_po->SetValue(last_po,po_peak);
        l_po->setText(QString::number(last_po*sb_maxw->value(),'f',0)+" W");
        if (cb_po_warn->isChecked() && f_tx && !f_tune && now - tx_since > 1500 && sl_pwr->value() > 0 &&
                last_po*100.0 < 0.5*sl_pwr->value())
            Event(tr("Po is below half of the set power"),true);
    }
}
void HvPowerSwrW::OnAlc(double a)
{
    {
        last_alc = qMax(0.0,a);
        m_alc->SetValue(last_alc);
        l_alc->setText(QString::number(qMin(999.0,last_alc*100.0),'f',0)+" %");
        if (cb_alc_warn->isChecked() && f_tx && !f_tune && last_alc > 0.5)
            Event(tr("ALC above 50 %: lower the audio level"),true);
    }
}
void HvPowerSwrW::OnSwr(double swr)
{
    qint64 now = clock.elapsed();
    {
        last_swr = swr;
        m_swr->SetValue(last_swr);
        l_swr->setText(last_swr >= 3.0 ? QString(QString::number(last_swr,'f',1)+"+") : QString::number(last_swr,'f',1));
        l_swr->setStyleSheet(last_swr < 1.8 ? "QLabel{color:rgb(22,101,52);}" :
                             (last_swr < 2.4 ? "QLabel{color:rgb(146,64,14);}" : "QLabel{color:rgb(153,27,27);}"));
        // protection (only with real output, not on noise at key-up)
        if (cb_prot->isChecked() && f_tx && last_po > 0.02 && last_swr > sb_swr_lim->value())
        {
            if (swr_high_since < 0) swr_high_since = now;
            else if (now - swr_high_since >= (qint64)(sb_swr_sec->value()*1000.0) && !f_blocked)
            {
                f_blocked = true;
                emit EmitTxBlocked(true);
                QString t = tr("TX STOPPED")+": "+tr("SWR")+" "+QString::number(last_swr,'f',1)+" "+
                            tr("check antenna and cables")+". "+tr("TX stays blocked until you press Unlock.");
                l_block->setText(t);
                l_block->setHidden(false);
                pb_unlock->setHidden(false);
                Event(tr("SWR")+" "+QString::number(last_swr,'f',1)+" - "+tr("TX STOPPED"),true);
                show();
                raise();
                FitLater(false);
                emit EmitStopTx();
            }
        }
        else
        {
            swr_high_since = -1;
            if (f_tx && last_po > 0.02 && !f_blocked)
                Event("Po "+QString::number(last_po*sb_maxw->value(),'f',0)+" W  "+tr("SWR")+" "+
                      QString::number(last_swr,'f',1)+"  ALC "+QString::number(last_alc*100.0,'f',0)+" %  OK",false);
        }
    }
}

// ---------------------------------------------------------------- own connection
void HvPowerSwrW::RigCmd(int id,const QString &s)
{
    if (cb_conn->currentIndex() == 0)
    {
        emit EmitRigCmd(id,s);
        return;
    }
    if (!f_link_ok) return;
    if (id == SET_RFPOWER) link->SetPower((int)(s.toInt()*100.0/255.0+0.5));
    else if (id == GET_RFPOWER) link->ReadPower();
    else if (id == GET_METERS) link->ReadMeters();
}
void HvPowerSwrW::UpdateConnWidgets()
{
    int k = cb_conn->currentIndex();
    le_host->setEnabled(k == 1);
    sb_port->setEnabled(k == 1);
    cb_com->setEnabled(k == 2);
    cb_baud->setEnabled(k == 2);
    le_civ->setEnabled(k == 2);
    pb_conn->setEnabled(k != 0);
}
void HvPowerSwrW::ConnChanged()
{
    UpdateConnWidgets();
    if (f_block) return;
    if (cb_conn->currentIndex() == 0)
    {
        link->Close();
        f_link_ok = false;
        l_conn->setText(QString());
        l_status->setText(s_cat_status);
        UpdateCapable();
    }
    else ConnApply();
    SaveSettings();
}
void HvPowerSwrW::ConnApply()
{
    int k = cb_conn->currentIndex();
    if (k == 0) return;
    bool ok = false;
    int civ = le_civ->text().trimmed().toInt(&ok,16);
    if (!ok) civ = 0x94;
    link->Open(k == 1 ? HvPwrLink::RIGCTLD : HvPwrLink::ICOM_SERIAL, le_host->text(), sb_port->value(),
               cb_com->currentText(), cb_baud->currentText().toInt(), civ);
    SaveSettings();
}
void HvPowerSwrW::RefreshCom()
{
    QString cur = cb_com->currentText();
    QStringList l;
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    for (int i = 0; i < ports.count(); ++i)
    {
        QString n = ports.at(i).portName;
        if (n.isEmpty() || n.endsWith(".init") || n.endsWith(".lock")) continue;
#if !defined(Q_OS_WIN)
        if (!n.startsWith("/")) n = "/dev/"+n;
#endif
        if (!l.contains(n)) l << n;
    }
    l.sort();
    cb_com->clear();
    cb_com->addItems(l);
    cb_com->setEditText(cur);
}
void HvPowerSwrW::LinkStatus(bool ok,QString s)
{
    bool was = f_link_ok;
    f_link_ok = ok;
    s_link_status = s;
    l_conn->setText(s);
    l_conn->setStyleSheet(ok ? "QLabel{color:rgb(22,101,52);}" : "QLabel{color:rgb(153,27,27);}");
    if (cb_conn->currentIndex() != 0)
    {
        if (l_status->text() != s)
        {
            l_status->setText(s);
            FitLater(false);
        }
        if (ok && !was) Log(true,s);
        UpdateCapable();
        if (ok && !was) link->ReadFreq();
    }
}
void HvPowerSwrW::LinkLog(bool tx,QString s)
{
    Log(tx,s);
}
void HvPowerSwrW::LinkFreq(double hz)
{
    if (hz > 0.0) SetFreqStr(QString::number(hz,'f',0));
}
void HvPowerSwrW::PollFreq()
{
    if (cb_conn->currentIndex() != 0 && f_link_ok && !f_tx) link->ReadFreq();
}
