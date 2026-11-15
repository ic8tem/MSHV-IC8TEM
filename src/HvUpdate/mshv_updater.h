/* MSHV-IC8TEM  automatic updates from the GitHub releases
 *
 * Every 6 hours (and from Help > Check for Updates) asks GitHub for the latest
 * release. If it is newer, it asks the user; on "Update now" it downloads the
 * package of this installation, checks it against SHA256SUMS.txt, closes MSHV
 * and installs it with a small helper script, then starts MSHV again.
 *
 *   Windows installer  -> setup.exe /SILENT /AUTOUPDATE=1 (restarts MSHV)
 *   Windows zip        -> files replaced (user data folders kept)
 *   macOS              -> MSHV-IC8TEM.app replaced
 *   Linux AppImage     -> AppImage file replaced
 *   tar.gz (Linux/BSD) -> files replaced (user data folders kept)
 *   .deb / .rpm / .txz -> package manager through pkexec (system password)
 *
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef MSHV_UPDATER_H
#define MSHV_UPDATER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QMap>
#include <QProcess>

class QWidget;
class QAction;
class QNetworkAccessManager;

class MshvUpdater : public QObject
{
    Q_OBJECT
public:
    MshvUpdater(QString app_path,QWidget *main_window,const bool *tx_flag);
    virtual ~MshvUpdater();
    QAction *ActionCheck();
    QAction *ActionAuto();
    static int CompareVersions(const QString &a,const QString &b);   // <0, 0, >0
    static QString CurrentVersion();
    QString PackageAssetName(const QString &version,QString *kind);   // "" = cannot self-update

public slots:
    void CheckNow();

private slots:
    void CheckAuto();
    void AutoToggled(bool);
    void NetFinished();
    void CurlFinished(int,QProcess::ExitStatus);
    void WaitTxAndApply();

private:
    QString s_app_path;
    QString s_set_path;
    QWidget *s_main;
    const bool *s_tx;
    QTimer *timer;
    QAction *ac_check;
    QAction *ac_auto;
    QNetworkAccessManager *nam;
    bool busy;
    bool manual;
    bool f_auto;
    QString s_skip;
    QString s_tmpdir;
    // latest release
    QString r_version;
    QString r_notes;
    QString r_page;
    QMap<QString,QString> r_assets;     // name -> url
    // download chain
    int step;                            // 1 = release info, 2 = SHA256SUMS, 3 = package
    QString d_file;
    QString d_kind;
    QString d_asset;
    QString d_sums;
    QString script;

    void ReadSettings();
    void SaveSettings();
    void Fetch(const QString &url,const QString &file);
    void StepDone(bool ok,const QString &err);
    void ReleaseInfo();
    void AskUser();
    void VerifyAndApply();
    bool StartHelper();
    void Fail(const QString &msg);
    void OpenReleasePage();
    QString SetTmpDir();
};
#endif
