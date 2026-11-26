/* MSHV-IC8TEM  Q65 Pileup: list of the recent callers. See hvq65pileupw.h.
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include "hvq65pileupw.h"

#include <QApplication>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QShowEvent>
#include <QPixmap>

#define PLW_COL_N    0
#define PLW_COL_FREQ 1
#define PLW_COL_CALL 2
#define PLW_COL_GRID 3
#define PLW_COL_EL   4
#define PLW_COL_AGE  5
#define PLW_COL_HASH 6

HvQ65PileupW::HvQ65PileupW(QWidget *parent)
        : QWidget(parent)
{
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    setWindowTitle(tr("Q65 Pileup - Callers"));
    setWindowIcon(QPixmap(":pic/ms_ico.png"));
    f_q65 = false;
    s_activity = 0;
    f_pos_known = false;

    tw = new QTreeWidget();
    tw->setRootIsDecorated(false);
    tw->setUniformRowHeights(true);
    tw->setAllColumnsShowFocus(true);
    tw->setSelectionMode(QAbstractItemView::SingleSelection);
    tw->setColumnCount(7);
    QStringList h;
    h << "N" << tr("Freq") << tr("Call") << tr("Grid") << tr("El") << tr("Age (h)") << "#";
    tw->setHeaderLabels(h);
    tw->header()->setStretchLastSection(false);
    tw->header()->setSectionsMovable(false);
    tw->headerItem()->setToolTip(PLW_COL_FREQ,tr("Audio frequency of the last decode (Hz)"));
    tw->headerItem()->setToolTip(PLW_COL_EL,tr("Moon elevation at the caller's locator (degrees)"));
    tw->headerItem()->setToolTip(PLW_COL_AGE,tr("Hours since the last decode"));
    tw->headerItem()->setToolTip(PLW_COL_HASH,tr("# = the caller copied our last transmission"));
    connect(tw, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(ItemClicked(QTreeWidgetItem*,int)));

    l_count = new QLabel();
    l_info = new QLabel();
    l_info->setWordWrap(true);
    b_refresh = new QPushButton(tr("Refresh"));
    b_erase = new QPushButton(tr("Erase list"));
    connect(b_refresh, SIGNAL(clicked()), this, SIGNAL(RefreshList()));
    connect(b_erase, SIGNAL(clicked()), this, SLOT(EraseClicked()));

    QHBoxLayout *H_b = new QHBoxLayout();
    H_b->setContentsMargins(0,0,0,0);
    H_b->setSpacing(6);
    H_b->addWidget(l_count);
    H_b->addStretch();
    H_b->addWidget(b_refresh);
    H_b->addWidget(b_erase);

    QVBoxLayout *V_l = new QVBoxLayout(this);
    V_l->setContentsMargins(4,4,4,4);
    V_l->setSpacing(4);
    V_l->addWidget(tw);
    V_l->addLayout(H_b);
    V_l->addWidget(l_info);
    setLayout(V_l);
    resize(400,430);
    SetInfo();
    Fill();
}
HvQ65PileupW::~HvQ65PileupW()
{}
QString HvQ65PileupW::InfoText(bool pileup)
{
    QString s = tr("Click: answer with Tx3 (R + locator). Ctrl+click: remove the caller.");
    if (pileup)
        s += "\n"+tr("Up to 40 callers, kept 24 hours; removed when their Moon is more than 5 degrees below the horizon or when the QSO is logged.");
    else
        s += "\n"+tr("Up to 40 callers, kept 24 hours, used for a priori decoding in the activities with locator exchange.");
    return s;
}
void HvQ65PileupW::SetInfo()
{
    QString s;
    if (!f_q65) s = tr("The list is used in Q65 mode.")+"\n";
    else if (s_activity != 14) s = tr("Q65 Pileup is off: Q65 menu, or right-click on a Q65 mode button.")+"\n";
    s += InfoText(f_q65 && s_activity == 14);
    l_info->setText(s);
}
void HvQ65PileupW::SetState(bool q65,int activity_id)
{
    if (f_q65 == q65 && s_activity == activity_id) return;
    f_q65 = q65;
    s_activity = activity_id;
    SetInfo();
}
void HvQ65PileupW::SetFont(QFont f)
{
    setFont(f);
    tw->setFont(f);
    Fill();
}
void HvQ65PileupW::SetDxCall(QString c)
{
    c = c.trimmed().toUpper();
    if (c == s_dx) return;
    s_dx = c;
    Fill();
}
void HvQ65PileupW::SetList(QStringList l)
{
    s_list = l;
    Fill();
}
void HvQ65PileupW::Fill()
{
    QString keep;
    if (tw->currentItem()) keep = tw->currentItem()->text(PLW_COL_CALL);
    tw->clear();
    int n = 0;
    int nhash = 0;
    QColor dim = palette().color(QPalette::Disabled,QPalette::Text);
    for (int i = 0; i < s_list.count(); ++i)
    {
        QStringList f = s_list.at(i).split("|");
        if (f.count()<8) continue;
        n++;
        QTreeWidgetItem *it = new QTreeWidgetItem();
        it->setText(PLW_COL_N,QString::number(n));
        it->setText(PLW_COL_FREQ,f.at(2));
        it->setText(PLW_COL_CALL,f.at(0));
        it->setText(PLW_COL_GRID,f.at(1));
        int el = f.at(3).toInt();
        if (el > -99) it->setText(PLW_COL_EL,QString::number(el));
        it->setText(PLW_COL_AGE,f.at(4));
        if (f.at(5)=="1")
        {
            it->setText(PLW_COL_HASH,"#");
            nhash++;
        }
        it->setData(PLW_COL_N,Qt::UserRole,f.at(6));//period of the last decode
        for (int c = 0; c < 7; ++c)
        {
            if (c==PLW_COL_CALL || c==PLW_COL_GRID) it->setTextAlignment(c,Qt::AlignLeft|Qt::AlignVCenter);
            else if (c==PLW_COL_HASH) it->setTextAlignment(c,Qt::AlignHCenter|Qt::AlignVCenter);
            else it->setTextAlignment(c,Qt::AlignRight|Qt::AlignVCenter);
        }
        if (el < 0 && el > -99)// Moon below the horizon there
        {
            for (int c = 0; c < 7; ++c) it->setForeground(c,QBrush(dim));
        }
        if (!s_dx.isEmpty() && (f.at(0)==s_dx || s_dx.contains("/"+f.at(0)) || s_dx.startsWith(f.at(0)+"/")))
        {
            QFont fb = tw->font();
            fb.setBold(true);
            for (int c = 0; c < 7; ++c) it->setFont(c,fb);
        }
        tw->addTopLevelItem(it);
        if (!keep.isEmpty() && f.at(0)==keep) tw->setCurrentItem(it);
    }
    // minimum widths from typical contents, so that the empty list is readable too
    static const char *smp[7] = {"00","0000","WW0WWW/P","JN70","-90","23.9","#"};
    QFontMetrics fm(tw->font());
    for (int c = 0; c < 7; ++c)
    {
        tw->resizeColumnToContents(c);
        int wmin = fm.boundingRect(QString(smp[c])).width() + 18;
        if (tw->columnWidth(c) < wmin) tw->setColumnWidth(c,wmin);
    }
    QString s = tr("Callers: %1").arg(n);
    if (nhash > 0) s += "   #: "+QString::number(nhash);
    l_count->setText(s);
}
void HvQ65PileupW::ItemClicked(QTreeWidgetItem *it,int)
{
    if (!it) return;
    QString call = it->text(PLW_COL_CALL);
    if (call.isEmpty()) return;
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        emit RemoveCaller(call);
        return;
    }
    emit StartQso(call,it->text(PLW_COL_GRID),it->data(PLW_COL_N,Qt::UserRole).toString(),
                  it->text(PLW_COL_FREQ).toInt());
}
void HvQ65PileupW::EraseClicked()
{
    if (QMessageBox::question(this,"MSHV",tr("Erase the list of Q65 callers?"),
                              QMessageBox::Yes|QMessageBox::No,QMessageBox::No) == QMessageBox::Yes)
        emit EraseList();
}
void HvQ65PileupW::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    f_pos_known = true;
    emit RefreshList();
}
QString HvQ65PileupW::GetPosXYWH()
{
    if (!f_pos_known) return "";//never opened: the window manager places it
    QRect r = normalGeometry();
    if (!r.isValid()) r = geometry();
    return QString("%1#%2#%3#%4#%5").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height()).arg(isVisible() ? 1 : 0);
}
void HvQ65PileupW::SetPosXYWH(QString s)
{
    QStringList l = s.split("#");
    if (l.count()<5) return;
    int w = l.at(2).toInt();
    int h = l.at(3).toInt();
    if (w>=200 && h>=150) resize(w,h);
    move(l.at(0).toInt(),l.at(1).toInt());
    f_pos_known = true;
    if (l.at(4)=="1") show();
}
