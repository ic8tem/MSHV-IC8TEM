/* MSHV-IC8TEM  automatic updates from the GitHub releases
 * See mshv_updater.h.
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include "mshv_updater.h"
#include "../mshv_ic8tem_version.h"
#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QRegExp>
#include <QSslSocket>
#include <QStandardPaths>
#include <QSysInfo>
#include <QTextStream>
#include <QUrl>
#include <QWidget>

#define UPD_REPO        "ic8tem/MSHV-IC8TEM"
#define UPD_API_LATEST  "https://api.github.com/repos/" UPD_REPO "/releases/latest"
#define UPD_PAGE        "https://github.com/" UPD_REPO "/releases/latest"
#define UPD_INTERVAL_MS (6*3600*1000)       // every 6 hours
#define UPD_FIRST_MS    (90*1000)           // first check 90 s after start

#if !defined Q_OS_WIN
static QString ShQ(const QString &s)
{
    QString r = s;
    r.replace("'","'\\''");
    return "'"+r+"'";
}
#endif
#if defined Q_OS_WIN
static QString PsQ(const QString &s)
{
    QString r = s;
    r.replace("'","''");
    return "'"+r+"'";
}
#endif
//IC8TEM v31: several installations (one per radio) update each their own folder: short tag of the
//program folder for the temporary folders, so two running installations never share a download
static QString InstallTag()
{
    QByteArray p = QDir::cleanPath(QCoreApplication::applicationDirPath()).toLower().toUtf8();
    return QString::fromLatin1(QCryptographicHash::hash(p,QCryptographicHash::Sha1).toHex().left(8));
}
//IC8TEM v31: MSHV starts again with the same command line (for example --inst-name=)
static QStringList RestartArgs()
{
    QStringList a = QCoreApplication::arguments();
    if (!a.isEmpty()) a.removeFirst();
    return a;
}

MshvUpdater::MshvUpdater(QString app_path,QWidget *main_window,const bool *tx_flag)
        : QObject(main_window)
{
    s_app_path = app_path;
    s_set_path = app_path+"/settings/ms_update";
    s_main = main_window;
    s_tx = tx_flag;
    busy = false;
    manual = false;
    f_auto = true;
    step = 0;
    nam = new QNetworkAccessManager(this);
    ReadSettings();

    ac_check = new QAction(tr("Check for Updates")+" ...",this);
    connect(ac_check, SIGNAL(triggered()), this, SLOT(CheckNow()));
    ac_auto = new QAction(tr("Check for Updates Automatically (every 6 hours)"),this);
    ac_auto->setCheckable(true);
    ac_auto->setChecked(f_auto);
    connect(ac_auto, SIGNAL(toggled(bool)), this, SLOT(AutoToggled(bool)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(CheckAuto()));
    timer->start(UPD_INTERVAL_MS);
    QTimer::singleShot(UPD_FIRST_MS, this, SLOT(CheckAuto()));
}
MshvUpdater::~MshvUpdater()
{}
QAction *MshvUpdater::ActionCheck()
{
    return ac_check;
}
QAction *MshvUpdater::ActionAuto()
{
    return ac_auto;
}
QString MshvUpdater::CurrentVersion()
{
    return QString(MSHV_IC8TEM_VERSION);
}
int MshvUpdater::CompareVersions(const QString &a0,const QString &b0)
{
    QString a = a0.toLower(), b = b0.toLower();
    a.remove("ic8tem");
    b.remove("ic8tem");
    QRegExp rx("(\\d+)");
    QList<int> la, lb;
    int p = 0;
    while ((p = rx.indexIn(a,p)) != -1)
    {
        la << rx.cap(1).toInt();
        p += rx.matchedLength();
    }
    p = 0;
    while ((p = rx.indexIn(b,p)) != -1)
    {
        lb << rx.cap(1).toInt();
        p += rx.matchedLength();
    }
    int n = qMax(la.count(),lb.count());
    for (int i = 0; i < n; ++i)
    {
        int x = la.value(i,0), y = lb.value(i,0);
        if (x != y) return x < y ? -1 : 1;
    }
    return 0;
}
void MshvUpdater::ReadSettings()
{
    QFile f(s_set_path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&f);
    while (!in.atEnd())
    {
        QString l = in.readLine().trimmed();
        if (l.startsWith("auto=")) f_auto = (l.mid(5)!="0");
        else if (l.startsWith("skip=")) s_skip = l.mid(5);
    }
    f.close();
}
void MshvUpdater::SaveSettings()
{
    QFile f(s_set_path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&f);
    out << "auto=" << (f_auto ? "1" : "0") << "\n";
    out << "skip=" << s_skip << "\n";
    f.close();
}
void MshvUpdater::AutoToggled(bool f)
{
    f_auto = f;
    SaveSettings();
}
QString MshvUpdater::SetTmpDir()
{
    QString t = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (t.isEmpty()) t = QDir::tempPath();
    t += "/MSHV-IC8TEM-update-"+InstallTag();//IC8TEM v31: one folder per installation
    QDir(t).removeRecursively();
    QDir().mkpath(t);
    s_tmpdir = t;
    return t;
}
QString MshvUpdater::PackageAssetName(const QString &V,QString *kind)
{
    QString VU = V;
    VU.replace('-','_');
    QString exe_dir = QCoreApplication::applicationDirPath();
#if defined Q_OS_MACOS
    if (kind) *kind = "mac";
#if defined(__aarch64__) || defined(__arm64__)
    return "MSHV-IC8TEM-"+V+"-macos-apple-silicon.dmg";
#else
    return "MSHV-IC8TEM-"+V+"-macos-intel.dmg";
#endif
#else
    QByteArray ai = qgetenv("APPIMAGE");
    if (!ai.isEmpty())
    {
        if (kind) *kind = "appimage";
        QString a = QSysInfo::currentCpuArchitecture();
        if (a=="arm64") a = "aarch64";
        return "MSHV-IC8TEM-"+V+"-linux-"+a+".AppImage";
    }
    QFile f(exe_dir+"/mshv-package.txt");          // written by the release build
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (kind) *kind = "none";                   // built by the user
        return QString();
    }
    QString m = QString::fromUtf8(f.readLine()).trimmed();
    f.close();
    m.replace("{V}",V);
    m.replace("{VU}",VU);
    QString k = "none";
    if (m.endsWith(".zip"))
    {
        if (QFile::exists(exe_dir+"/unins000.exe"))
        {
            k = "winsetup";
            m = m.left(m.length()-4)+"-setup.exe";
        }
        else k = "winzip";
    }
    else if (m.endsWith(".tar.gz")) k = "tar";
    else if (m.endsWith(".deb")) k = "deb";
    else if (m.endsWith(".rpm")) k = "rpm";
    else if (m.endsWith(".txz")) k = "txz";
    if (kind) *kind = k;
    return (k=="none") ? QString() : m;
#endif
}
void MshvUpdater::CheckAuto()
{
    if (!f_auto || busy) return;
    manual = false;
    busy = true;
    step = 1;
    SetTmpDir();
    Fetch(UPD_API_LATEST, s_tmpdir+"/latest.json");
}
void MshvUpdater::CheckNow()
{
    if (busy) return;
    manual = true;
    busy = true;
    step = 1;
    SetTmpDir();
    Fetch(UPD_API_LATEST, s_tmpdir+"/latest.json");
}
void MshvUpdater::Fetch(const QString &url,const QString &file)
{
    d_file = file;
    if (QSslSocket::supportsSsl())
    {
        QNetworkRequest req((QUrl(url)));
        req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
        req.setHeader(QNetworkRequest::UserAgentHeader, "MSHV-IC8TEM/" MSHV_IC8TEM_VERSION);
        if (url.contains("api.github.com")) req.setRawHeader("Accept","application/vnd.github+json");
        QNetworkReply *r = nam->get(req);
        connect(r, SIGNAL(finished()), this, SLOT(NetFinished()));
        return;
    }
    QString curl = QStandardPaths::findExecutable("curl");
    if (curl.isEmpty())
    {
        StepDone(false,tr("HTTPS is not available (no OpenSSL and no curl)"));
        return;
    }
    QProcess *pr = new QProcess(this);
    QStringList args;
    args << "-L" << "-s" << "-S" << "-f" << "--max-time" << "600"
         << "-A" << "MSHV-IC8TEM/" MSHV_IC8TEM_VERSION << "-o" << file << url;
    connect(pr, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(CurlFinished(int,QProcess::ExitStatus)));
    pr->start(curl,args);
}
void MshvUpdater::NetFinished()
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender());
    if (!r) return;
    r->deleteLater();
    if (r->error() != QNetworkReply::NoError)
    {
        StepDone(false,r->errorString());
        return;
    }
    QByteArray data = r->readAll();
    QFile f(d_file);
    if (!f.open(QIODevice::WriteOnly) || f.write(data) != data.size())
    {
        f.close();
        StepDone(false,tr("cannot write")+" "+d_file);
        return;
    }
    f.close();
    StepDone(true,QString());
}
void MshvUpdater::CurlFinished(int code,QProcess::ExitStatus st)
{
    QProcess *pr = qobject_cast<QProcess*>(sender());
    QString err;
    if (pr)
    {
        err = QString::fromLocal8Bit(pr->readAllStandardError()).trimmed();
        pr->deleteLater();
    }
    if (st != QProcess::NormalExit || code != 0)
    {
        StepDone(false,err.isEmpty() ? "curl "+QString("%1").arg(code) : err);
        return;
    }
    StepDone(true,QString());
}
void MshvUpdater::StepDone(bool ok,const QString &err)
{
    if (!ok)
    {
        if (manual) Fail(tr("Update check failed")+": "+err);
        else busy = false;           // silent in automatic mode, next try in 6 hours
        return;
    }
    if (step==1) ReleaseInfo();
    else if (step==2)
    {
        step = 3;
        Fetch(r_assets.value(d_asset), s_tmpdir+"/"+d_asset);
    }
    else if (step==3) VerifyAndApply();
}
void MshvUpdater::ReleaseInfo()
{
    QFile f(s_tmpdir+"/latest.json");
    QByteArray data;
    if (f.open(QIODevice::ReadOnly))
    {
        data = f.readAll();
        f.close();
    }
    QJsonObject o = QJsonDocument::fromJson(data).object();
    QString tag = o.value("tag_name").toString();
    if (tag.isEmpty())
    {
        if (manual) Fail(tr("Update check failed")+": "+tr("no release information"));
        else busy = false;
        return;
    }
    r_version = tag.startsWith("v") ? tag.mid(1) : tag;
    r_notes = o.value("body").toString();
    r_page = o.value("html_url").toString();
    if (r_page.isEmpty()) r_page = UPD_PAGE;
    r_assets.clear();
    QJsonArray as = o.value("assets").toArray();
    for (int i = 0; i < as.count(); ++i)
    {
        QJsonObject a = as.at(i).toObject();
        r_assets.insert(a.value("name").toString(), a.value("browser_download_url").toString());
    }
    if (CompareVersions(r_version,CurrentVersion()) <= 0)
    {
        busy = false;
        if (manual)
            QMessageBox::information(s_main,"MSHV-IC8TEM",tr("You have the latest version")+": "+CurrentVersion());
        return;
    }
    if (!manual)
    {
        if (r_version==s_skip)
        {
            busy = false;
            return;
        }
        if (s_tx && *s_tx)                        // never interrupt a transmission
        {
            busy = false;
            QTimer::singleShot(120000, this, SLOT(CheckAuto()));
            return;
        }
    }
    AskUser();
}
void MshvUpdater::AskUser()
{
    QString kind;
    QString asset = PackageAssetName(r_version,&kind);
    bool can = !asset.isEmpty() && r_assets.contains(asset) && r_assets.contains("SHA256SUMS.txt");
#if !defined Q_OS_WIN && !defined Q_OS_MACOS
    if (can && (kind=="deb" || kind=="rpm" || kind=="txz") && QStandardPaths::findExecutable("pkexec").isEmpty()) can = false;
#endif
    QString notes = r_notes;
    if (notes.length() > 1500) notes = notes.left(1500)+" ...";
    QMessageBox mb(s_main);
    mb.setWindowTitle("MSHV-IC8TEM");
    mb.setIcon(QMessageBox::Information);
    mb.setText("<b>MSHV-IC8TEM "+r_version+"</b> "+tr("is available")+".<br>"+tr("Installed version")+": "+CurrentVersion()+
               "<br>"+tr("Installation")+": "+QDir::toNativeSeparators(QCoreApplication::applicationDirPath()).toHtmlEscaped());//IC8TEM v31
    QString info;
    if (can)
    {
        info = tr("Update now: MSHV downloads the new version, checks it, closes, installs it and starts again. Settings and logs are kept.");
        if (kind=="deb" || kind=="rpm" || kind=="txz") info += "<br>"+tr("The system will ask for the administrator password.");
    }
    else info = tr("This installation cannot update itself: open the download page.");
    mb.setInformativeText(info);
    if (!notes.isEmpty()) mb.setDetailedText(notes);
    QPushButton *b_up = 0;
    QPushButton *b_page = 0;
    if (can) b_up = mb.addButton(tr("Update now"),QMessageBox::AcceptRole);
    else b_page = mb.addButton(tr("Open download page"),QMessageBox::AcceptRole);
    QPushButton *b_later = mb.addButton(tr("Later"),QMessageBox::RejectRole);
    QPushButton *b_skip = mb.addButton(tr("Skip this version"),QMessageBox::DestructiveRole);
    mb.setDefaultButton(b_later);
    mb.exec();
    QAbstractButton *c = mb.clickedButton();
    if (c==b_skip)
    {
        s_skip = r_version;
        SaveSettings();
        busy = false;
        return;
    }
    if (b_page && c==b_page)
    {
        OpenReleasePage();
        busy = false;
        return;
    }
    if (!b_up || c!=b_up)
    {
        busy = false;
        return;
    }
    d_kind = kind;
    d_asset = asset;
    d_sums = s_tmpdir+"/SHA256SUMS.txt";
    step = 2;
    Fetch(r_assets.value("SHA256SUMS.txt"), d_sums);
}
void MshvUpdater::VerifyAndApply()
{
    QString pkg = s_tmpdir+"/"+d_asset;
    QString want;
    QFile fs(d_sums);
    if (fs.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&fs);
        while (!in.atEnd())
        {
            QString l = in.readLine().trimmed();
            QStringList p = l.split(QRegExp("\\s+"));
            if (p.count() >= 2)
            {
                QString n = p.at(1);
                if (n.startsWith("*")) n = n.mid(1);
                if (n==d_asset) want = p.at(0).toLower();
            }
        }
        fs.close();
    }
    QFile fp(pkg);
    if (want.isEmpty() || !fp.open(QIODevice::ReadOnly))
    {
        Fail(tr("Update not installed: checksum missing for")+" "+d_asset);
        return;
    }
    QCryptographicHash h(QCryptographicHash::Sha256);
    h.addData(&fp);
    fp.close();
    QString got = QString::fromLatin1(h.result().toHex());
    if (got != want)
    {
        QFile::remove(pkg);
        Fail(tr("Update not installed: the downloaded file is damaged (SHA256 mismatch)."));
        return;
    }
    if (!StartHelper()) return;
    QMessageBox::information(s_main,"MSHV-IC8TEM",
                             tr("MSHV-IC8TEM will now close, install version")+" "+r_version+" "+tr("and start again."));
    WaitTxAndApply();
}
void MshvUpdater::WaitTxAndApply()
{
    if (s_tx && *s_tx)
    {
        QTimer::singleShot(1000, this, SLOT(WaitTxAndApply()));
        return;
    }
    if (s_main) s_main->close();
    QTimer::singleShot(500, qApp, SLOT(quit()));
}
bool MshvUpdater::StartHelper()
{
    const QString pkg = QDir::toNativeSeparators(s_tmpdir+"/"+d_asset);
    const QString app_dir = QCoreApplication::applicationDirPath();
    const QString exe = QCoreApplication::applicationFilePath();
    const qint64 pid = QCoreApplication::applicationPid();
#if defined Q_OS_WIN
    script = s_tmpdir+"/mshv_update.ps1";
    const QString dir = QDir::toNativeSeparators(app_dir);
    QString restart = "Start-Process -FilePath "+PsQ(QDir::toNativeSeparators(exe))+" -WorkingDirectory "+PsQ(dir);
    QStringList ra = RestartArgs();
    if (!ra.isEmpty())
    {
        QStringList q;
        for (int i = 0; i < ra.count(); ++i)
        {
            QString a = ra.at(i);
            a.remove('"');
            q << "\""+a+"\"";
        }
        restart += " -ArgumentList "+PsQ(q.join(" "));
    }
    QString s;
    s += "$ErrorActionPreference = 'SilentlyContinue'\r\n";
    s += "Wait-Process -Id "+QString::number(pid)+" -Timeout 180\r\n";
    s += "Start-Sleep -Milliseconds 800\r\n";
    if (d_kind=="winsetup")
    {
        //IC8TEM v31: the installer updates the folder of THIS installation (/DIR). Before, it always
        //installed into its default folder (C:\MSHV-IC8TEM-NEW), so a second installation in another
        //folder was never updated. AUTOUPDATE=2: the installer does not start MSHV, this script starts
        //it when the installer has finished, with the same command line.
        s += "Start-Process -FilePath "+PsQ(pkg)+" -ArgumentList "+
             PsQ("/SILENT /SUPPRESSMSGBOXES /NORESTART /AUTOUPDATE=2 /DIR=\""+dir+"\"")+" -Wait\r\n";
    }
    else
    {
        s += "$t = Join-Path $env:TEMP "+PsQ("MSHV-IC8TEM-unzip-"+InstallTag())+"\r\n";//IC8TEM v31: per installation
        s += "Remove-Item $t -Recurse -Force\r\n";
        s += "Expand-Archive -Path "+PsQ(pkg)+" -DestinationPath $t -Force\r\n";
        s += "$src = Join-Path $t 'MSHV-IC8TEM'\r\n";
        s += "if (Test-Path $src) { robocopy $src "+PsQ(dir)+" /E /XD settings log RxWavs ExportLog AllTxtMonthly Screenshots /R:5 /W:1 /NFL /NDL /NJH /NJS | Out-Null }\r\n";
        s += "Remove-Item $t -Recurse -Force\r\n";
    }
    s += restart+"\r\n";
    QFile f(script);
    if (!f.open(QIODevice::WriteOnly))
    {
        Fail(tr("cannot write")+" "+script);
        return false;
    }
    f.write(s.toUtf8().prepend("\xEF\xBB\xBF"));
    f.close();
    QStringList args;
    args << "-NoProfile" << "-ExecutionPolicy" << "Bypass" << "-WindowStyle" << "Hidden" << "-File" << QDir::toNativeSeparators(script);
    if (!QProcess::startDetached("powershell.exe",args))
    {
        Fail(tr("cannot start the update helper (PowerShell)"));
        return false;
    }
    return true;
#else
    script = s_tmpdir+"/mshv_update.sh";
    QString args;                                    //IC8TEM v31: same command line after the update
    QStringList ra = RestartArgs();
    for (int i = 0; i < ra.count(); ++i) args += " "+ShQ(ra.at(i));
    QString s = "#!/bin/sh\n";
    s += "P="+QString::number(pid)+"\n";
    s += "i=0; while kill -0 $P 2>/dev/null && [ $i -lt 360 ]; do sleep 0.5; i=$((i+1)); done\n";
    s += "sleep 1\n";
    s += "PKG="+ShQ(pkg)+"\n";
    if (d_kind=="mac")
    {
        QString bundle = QDir(app_dir+"/../..").absolutePath();
        if (!QFileInfo(QFileInfo(bundle).absolutePath()).isWritable())
        {
            Fail(tr("The folder of MSHV-IC8TEM.app is not writable: install the update by hand."));
            return false;
        }
        s += "APP="+ShQ(bundle)+"\n";
        s += "M=$(mktemp -d)\n";
        s += "if hdiutil attach -nobrowse -quiet -mountpoint \"$M\" \"$PKG\"; then\n";
        s += "  NEW=$(ls -d \"$M\"/*.app | head -1)\n";
        s += "  rm -rf \"$APP.old\"\n";
        s += "  if mv \"$APP\" \"$APP.old\" && ditto \"$NEW\" \"$APP\"; then rm -rf \"$APP.old\"; else rm -rf \"$APP\"; mv \"$APP.old\" \"$APP\"; fi\n";
        s += "  hdiutil detach -quiet \"$M\"\n";
        s += "  xattr -dr com.apple.quarantine \"$APP\" 2>/dev/null\n";
        s += "fi\n";
        s += "open -n \"$APP\""+(args.isEmpty() ? QString() : " --args"+args)+"\n";
    }
    else if (d_kind=="appimage")
    {
        QString ai = QString::fromLocal8Bit(qgetenv("APPIMAGE"));
        if (!QFileInfo(QFileInfo(ai).absolutePath()).isWritable())
        {
            Fail(tr("The folder of the AppImage is not writable: install the update by hand."));
            return false;
        }
        s += "OLD="+ShQ(ai)+"\n";
        s += "chmod +x \"$PKG\" && cp -f \"$PKG\" \"$OLD.new\" && chmod +x \"$OLD.new\" && mv -f \"$OLD.new\" \"$OLD\"\n";
        s += "nohup \"$OLD\""+args+" >/dev/null 2>&1 &\n";
    }
    else if (d_kind=="tar")
    {
        if (!QFileInfo(app_dir).isWritable())
        {
            Fail(tr("The folder of MSHV-IC8TEM is not writable: install the update by hand."));
            return false;
        }
        s += "DIR="+ShQ(app_dir)+"\n";
        s += "EXE="+ShQ(exe)+"\n";
        s += "T=$(mktemp -d)\n";
        s += "if tar -xzf \"$PKG\" -C \"$T\" && cd \"$T/MSHV-IC8TEM\"; then\n";
        s += "  for f in * .[!.]*; do\n";
        s += "    [ -e \"$f\" ] || continue\n";
        s += "    case \"$f\" in settings|log|RxWavs|ExportLog|AllTxtMonthly|Screenshots) continue;; esac\n";
        s += "    cp -Rf \"$f\" \"$DIR/\"\n";
        s += "  done\n";
        s += "fi\n";
        s += "rm -rf \"$T\"\n";
        s += "cd \"$DIR\" && nohup \"$EXE\""+args+" >/dev/null 2>&1 &\n";
    }
    else
    {
        QString cmd;
        if (d_kind=="deb") cmd = "apt-get install -y --allow-downgrades \"$1\" || dpkg -i \"$1\"";
        else if (d_kind=="rpm") cmd = "if command -v dnf >/dev/null; then dnf install -y \"$1\"; elif command -v zypper >/dev/null; then zypper --non-interactive install --allow-unsigned-rpm \"$1\"; else rpm -Uvh --replacepkgs \"$1\"; fi";
        else cmd = "PATH=/sbin:/usr/sbin:$PATH upgradepkg --install-new \"$1\"";
        s += "pkexec /bin/sh -c "+ShQ(cmd)+" sh \"$PKG\"\n";
        s += "nohup /usr/bin/mshv-ic8tem"+args+" >/dev/null 2>&1 &\n";
    }
    QFile f(script);
    if (!f.open(QIODevice::WriteOnly))
    {
        Fail(tr("cannot write")+" "+script);
        return false;
    }
    f.write(s.toUtf8());
    f.close();
    f.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);
    if (!QProcess::startDetached("/bin/sh",QStringList() << script))
    {
        Fail(tr("cannot start the update helper"));
        return false;
    }
    return true;
#endif
}
void MshvUpdater::Fail(const QString &msg)
{
    busy = false;
    QMessageBox mb(s_main);
    mb.setWindowTitle("MSHV-IC8TEM");
    mb.setIcon(QMessageBox::Warning);
    mb.setText(msg);
    QPushButton *b_page = mb.addButton(tr("Open download page"),QMessageBox::AcceptRole);
    mb.addButton(QMessageBox::Close);
    mb.exec();
    if (mb.clickedButton()==b_page) OpenReleasePage();
}
void MshvUpdater::OpenReleasePage()
{
    QDesktopServices::openUrl(QUrl(r_page.isEmpty() ? QString(UPD_PAGE) : r_page));
}
