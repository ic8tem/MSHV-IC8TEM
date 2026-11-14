/* MSHV HvLotwUsersW  (IC8TEM mod)
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include "hvlotwusers.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QAbstractScrollArea>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QUrl>
#include <QSslSocket>
#include <QProcess>
#include <QStandardPaths>

static QHash<QString,int> g_lotw_calls;  // call -> julian day of the last LoTW upload
static int  g_lotw_min_jd = 0;           // valid if last upload >= this day
static bool g_lotw_show = true;
#define LOTW_CSV_URL "https://lotw.arrl.org/lotw-user-activity.csv"

static QString LotwBaseCall(const QString &c)
{
    // "EA8/IK2ABC/P" -> "IK2ABC": the longest part that contains a digit and a letter
    QStringList l = c.split("/");
    QString res;
    for (int i = 0; i < l.count(); ++i)
    {
        const QString &p = l.at(i);
        bool hd = false, hl = false;
        for (int j = 0; j < p.count(); ++j)
        {
            if (p.at(j).isDigit()) hd = true;
            else if (p.at(j).isLetter()) hl = true;
        }
        if (hd && hl && p.count() > res.count()) res = p;
    }
    return res;
}
bool HvLotwUsersW::HasList()
{
    return !g_lotw_calls.isEmpty();
}
bool HvLotwUsersW::IsLotwUser(const QString &call0)
{
    if (!g_lotw_show) return false;
    return IsLotwValid(call0);
}
bool HvLotwUsersW::IsLotwValid(const QString &call0)
{
    if (g_lotw_calls.isEmpty() || call0.count() < 3) return false;
    QString call = call0.toUpper();
    QHash<QString,int>::const_iterator it = g_lotw_calls.constFind(call);
    if (it == g_lotw_calls.constEnd() && call.contains("/"))
        it = g_lotw_calls.constFind(LotwBaseCall(call));
    if (it == g_lotw_calls.constEnd()) return false;
    return it.value() >= g_lotw_min_jd;
}
bool HvLotwUsersW::IsShowActive()
{
    return g_lotw_show && !g_lotw_calls.isEmpty();
}

HvLotwUsersW::HvLotwUsersW(QString app_path,QWidget *parent)
        : QWidget(parent)
{
    f_block_save = true;
    s_app_path = app_path;
    s_csv_path = app_path+"/settings/database/lotw-user-activity.csv";
    s_set_path = app_path+"/settings/ms_lotw";

    QGroupBox *gb = new QGroupBox(tr("LoTW Users (Logbook of The World)"));
    QLabel *l_info = new QLabel(tr("LoTW user list from ARRL")+":<br><a href=\"" LOTW_CSV_URL "\">" LOTW_CSV_URL "</a><br>"+
                                tr("Press Download, or open the link, save the file and load it with Load. A copy is kept in settings/database."));
    l_info->setTextFormat(Qt::RichText);
    l_info->setTextInteractionFlags(Qt::TextBrowserInteraction);
    l_info->setOpenExternalLinks(true);
    l_info->setWordWrap(true);
    pb_download = new QPushButton(" "+tr("Download from ARRL and Load")+" ");
    connect(pb_download, SIGNAL(clicked()), this, SLOT(DownloadCsv()));
    nam = new QNetworkAccessManager(this);

    QPushButton *pb_load = new QPushButton(" "+tr("Load LoTW CSV File")+" ... ");
    connect(pb_load, SIGNAL(clicked()), this, SLOT(LoadCsvFile()));

    QLabel *l_date = new QLabel(tr("Valid if last LoTW upload is on or after")+":");
    de_min_date = new QDateEdit();
    de_min_date->setCalendarPopup(true);
    de_min_date->setDisplayFormat("yyyy-MM-dd");
    de_min_date->setDate(QDate::currentDate().addYears(-1));
    connect(de_min_date, SIGNAL(dateChanged(const QDate &)), this, SLOT(MinDateChanged(const QDate &)));
    QHBoxLayout *H_date = new QHBoxLayout();
    H_date->setContentsMargins(0,0,0,0);
    H_date->addWidget(l_date);
    H_date->addWidget(de_min_date);
    H_date->addStretch(1);

    cb_show = new QCheckBox(tr("Show a black dot at the right of decoded lines from LoTW users"));
    cb_show->setChecked(true);
    connect(cb_show, SIGNAL(toggled(bool)), this, SLOT(ShowChanged(bool)));

    l_status = new QLabel();
    l_status->setWordWrap(true);

    QHBoxLayout *H_load = new QHBoxLayout();
    H_load->setContentsMargins(0,0,0,0);
    H_load->addWidget(pb_download);
    H_load->addWidget(pb_load);
    H_load->addStretch(1);

    QVBoxLayout *V_gb = new QVBoxLayout();
    V_gb->setContentsMargins(6,6,6,6);
    V_gb->setSpacing(8);
    V_gb->addWidget(l_info);
    V_gb->addLayout(H_load);
    V_gb->addLayout(H_date);
    V_gb->addWidget(cb_show);
    V_gb->addWidget(l_status);
    gb->setLayout(V_gb);

    QVBoxLayout *V_l = new QVBoxLayout(this);
    V_l->setContentsMargins(4,4,4,4);
    V_l->addWidget(gb);
    V_l->addStretch(1);
    setLayout(V_l);

    ReadSettings();
    QDate last;
    QHash<QString,int> tmp;
    if (QFile::exists(s_csv_path) && ParseCsv(s_csv_path,tmp,last) > 0) g_lotw_calls = tmp;
    g_lotw_min_jd = (int)de_min_date->date().toJulianDay();
    g_lotw_show = cb_show->isChecked();
    f_block_save = false;
    RefreshStatus();
}
HvLotwUsersW::~HvLotwUsersW()
{}
int HvLotwUsersW::ParseCsv(QString path,QHash<QString,int> &out,QDate &last)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return -1;
    out.clear();
    out.reserve(200000);
    last = QDate();
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        QStringList l = line.split(",");
        if (l.count() < 2) continue;
        QString call = l.at(0).trimmed().toUpper();
        call.remove("\"");
        QString sd = l.at(1).trimmed();
        sd.remove("\"");
        QDate d = QDate::fromString(sd,"yyyy-MM-dd");
        if (call.isEmpty() || !d.isValid()) continue;// header or bad line
        int jd = (int)d.toJulianDay();
        QHash<QString,int>::iterator it = out.find(call);
        if (it == out.end()) out.insert(call,jd);
        else if (jd > it.value()) it.value() = jd;
        if (!last.isValid() || d > last) last = d;
    }
    file.close();
    return out.count();
}
void HvLotwUsersW::LoadCsvFile()
{
    QString fn = QFileDialog::getOpenFileName(this,tr("Open LoTW CSV File"),QDir::homePath(),
                 "LoTW CSV (*.csv);;All files (*.*)");
    if (fn.isEmpty()) return;
    QHash<QString,int> tmp;
    QDate last;
    int n = ParseCsv(fn,tmp,last);
    if (n <= 0)
    {
        QMessageBox::warning(this,"MSHV",tr("No valid lines found in this file.")+"\n"+
                             tr("Expected format")+": CALL,YYYY-MM-DD,HH:MM:SS");
        return;
    }
    if (QFileInfo(fn).absoluteFilePath() != QFileInfo(s_csv_path).absoluteFilePath())
    {
        QDir().mkpath(s_app_path+"/settings/database");
        QFile::remove(s_csv_path);
        QFile::copy(fn,s_csv_path);
    }
    g_lotw_calls = tmp;
    RefreshStatus();
    RefreshDecodeLists();
}
void HvLotwUsersW::DownloadCsv()
{
    if (!QSslSocket::supportsSsl())
    {
        // Qt without OpenSSL (e.g. Windows Qt 5.15): use curl of the system
        // (Windows 10/11, macOS and most Linux have it).
        QString curl = QStandardPaths::findExecutable("curl");
        if (curl.isEmpty())
        {
            QMessageBox::warning(this,"MSHV",tr("HTTPS is not available here.")+"\n"+
                                 tr("Open the link in the browser, save the file and load it with Load")+":\n"+LOTW_CSV_URL);
            return;
        }
        QDir().mkpath(s_app_path+"/settings/database");
        QProcess *pr = new QProcess(this);
        pr->setProperty("tmp",s_csv_path+".tmp");
        QStringList args;
        args<<"-L"<<"-s"<<"-S"<<"-f"<<"--max-time"<<"300"<<"-o"<<(s_csv_path+".tmp")<<LOTW_CSV_URL;
        connect(pr, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(CurlFinished(int,QProcess::ExitStatus)));
        pb_download->setEnabled(false);
        l_status->setText(tr("Downloading")+" "+LOTW_CSV_URL+" ... (curl)");
        pr->start(curl,args);
        return;
    }
    QNetworkRequest req(QUrl(LOTW_CSV_URL));
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    req.setHeader(QNetworkRequest::UserAgentHeader, "MSHV-IC8TEM");
    QNetworkReply *r = nam->get(req);
    pb_download->setEnabled(false);
    l_status->setText(tr("Downloading")+" "+LOTW_CSV_URL+" ...");
    connect(r, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(DownloadProgress(qint64,qint64)));
    connect(r, SIGNAL(finished()), this, SLOT(DownloadFinished()));
}
void HvLotwUsersW::DownloadProgress(qint64 got,qint64)
{
    l_status->setText(tr("Downloading")+" ... "+QString("%1").arg((double)got/1048576.0,0,'f',1)+" MB");
}
void HvLotwUsersW::DownloadFinished()
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender());
    pb_download->setEnabled(true);
    if (!r) return;
    r->deleteLater();
    if (r->error() != QNetworkReply::NoError)
    {
        RefreshStatus();
        QMessageBox::warning(this,"MSHV",tr("Download failed")+": "+r->errorString()+"\n\n"+
                             tr("Open the link in the browser, save the file and load it with Load")+":\n"+LOTW_CSV_URL);
        return;
    }
    QByteArray data = r->readAll();
    QDir().mkpath(s_app_path+"/settings/database");
    QString tmp = s_csv_path+".tmp";
    QFile f(tmp);
    if (!f.open(QIODevice::WriteOnly) || f.write(data)!=data.size())
    {
        f.close();
        QFile::remove(tmp);
        RefreshStatus();
        QMessageBox::warning(this,"MSHV",tr("Cannot write")+" "+tmp);
        return;
    }
    f.close();
    UseDownloaded(tmp);
}
void HvLotwUsersW::CurlFinished(int code,QProcess::ExitStatus st)
{
    QProcess *pr = qobject_cast<QProcess*>(sender());
    pb_download->setEnabled(true);
    QString tmp = s_csv_path+".tmp";
    QString err;
    if (pr)
    {
        err = QString::fromLocal8Bit(pr->readAllStandardError()).trimmed();
        pr->deleteLater();
    }
    if (st!=QProcess::NormalExit || code!=0)
    {
        QFile::remove(tmp);
        RefreshStatus();
        QMessageBox::warning(this,"MSHV",tr("Download failed")+": "+err+"\n\n"+
                             tr("Open the link in the browser, save the file and load it with Load")+":\n"+LOTW_CSV_URL);
        return;
    }
    UseDownloaded(tmp);
}
void HvLotwUsersW::UseDownloaded(QString tmp)
{
    QHash<QString,int> h;
    QDate last;
    int n = ParseCsv(tmp,h,last);
    if (n <= 0)
    {
        QFile::remove(tmp);
        RefreshStatus();
        QMessageBox::warning(this,"MSHV",tr("The downloaded file is not a LoTW user list."));
        return;
    }
    QFile::remove(s_csv_path);
    QFile::rename(tmp,s_csv_path);
    g_lotw_calls = h;
    RefreshStatus();
    RefreshDecodeLists();
}
void HvLotwUsersW::MinDateChanged(const QDate &d)
{
    g_lotw_min_jd = (int)d.toJulianDay();
    SaveSettings();
    RefreshStatus();
    RefreshDecodeLists();
}
void HvLotwUsersW::ShowChanged(bool f)
{
    g_lotw_show = f;
    SaveSettings();
    RefreshDecodeLists();
}
void HvLotwUsersW::RefreshStatus()
{
    if (g_lotw_calls.isEmpty())
    {
        l_status->setText(tr("No LoTW list loaded."));
        return;
    }
    int valid = 0;
    int last_jd = 0;
    for (QHash<QString,int>::const_iterator it = g_lotw_calls.constBegin(); it != g_lotw_calls.constEnd(); ++it)
    {
        if (it.value() >= g_lotw_min_jd) valid++;
        if (it.value() > last_jd) last_jd = it.value();
    }
    l_status->setText(tr("Calls in the list")+": "+QString("%1").arg(g_lotw_calls.count())+"     "+
                      tr("Valid from the selected date")+": "+QString("%1").arg(valid)+"\n"+
                      tr("Most recent upload in the file")+": "+QDate::fromJulianDay(last_jd).toString("yyyy-MM-dd"));
}
void HvLotwUsersW::RefreshDecodeLists()
{
    // repaint every decode list (DecodeList is a QTreeView)
    QWidgetList all = QApplication::allWidgets();
    for (int i = 0; i < all.count(); ++i)
    {
        if (all.at(i)->inherits("DecodeList"))
        {
            QAbstractScrollArea *sa = qobject_cast<QAbstractScrollArea*>(all.at(i));
            if (sa) sa->viewport()->update();
        }
    }
}
void HvLotwUsersW::ReadSettings()
{
    QFile file(s_set_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.startsWith("lotw_min_date="))
        {
            QDate d = QDate::fromString(line.mid(14),"yyyy-MM-dd");
            if (d.isValid()) de_min_date->setDate(d);
        }
        else if (line.startsWith("lotw_show_dot="))
            cb_show->setChecked(line.mid(14)=="1");
    }
    file.close();
}
void HvLotwUsersW::SaveSettings()
{
    if (f_block_save) return;
    QFile file(s_set_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    out << "lotw_min_date=" << de_min_date->date().toString("yyyy-MM-dd") << "\n";
    out << "lotw_show_dot=" << (cb_show->isChecked() ? "1" : "0") << "\n";
    file.close();
}
