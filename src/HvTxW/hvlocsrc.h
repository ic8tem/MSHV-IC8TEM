/* MSHV-IC8TEM  Locator lookup when the call is not in the MSHV database.
 * Order (after the clicked message and the MSHV database):
 *  RX   : the locator sent by the station in the decodes of the last 20 periods
 *  C3   : CALL3.TXT terrestrial (WSJT-X Improved, SourceForge)
 *  EME  : CALL3.TXT EME (W7GJ)
 *  JTDX : grid_data.bin (JTDX, SourceForge)
 *  WEB  : online lookup api.g7vrd.co.uk (beta service)
 * The lists are downloaded in settings/database/locsources and refreshed every 14 days.
 * The settings are in settings/ms_locsrc. The page is "Network Configuration" page 4.
 *
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef HVLOCSRC_H
#define HVLOCSRC_H

#include <QObject>
#include <QWidget>
#include <QHash>
#include <QVector>
#include <QSharedPointer>
#include <QStringList>
#include <QThread>
#include <QProcess>

class QNetworkAccessManager;
class QNetworkReply;
class QTimer;
class QCheckBox;
class QLabel;
class QPushButton;

struct HvLocRec
{
    char c[12];      // call, ASCII, zero padded
    quint32 g;       // grid code (see GridEnc)
};

class HvLocTable
{
public:
    QVector<HvLocRec> v;
    QString info;    // e.g. "Revised 31 December, 2025" (W7GJ)
    bool Find(const QString &call,QString &grid) const;
};
typedef QSharedPointer<HvLocTable> HvLocTablePtr;

class HvLocLoader : public QThread   // reads and checks one list in the background
{
    Q_OBJECT
public:
    HvLocLoader(int id,const QString &file,QObject *parent = 0);
    int id;
    int gen;
    QString file;
    HvLocTablePtr table;
    QString error;
protected:
    void run();
};

class HvLocSrc : public QObject
{
    Q_OBJECT
public:
    enum { RX = 0, C3, EME, JTDX, WEB, COUNT };
    HvLocSrc(const QString &app_path,QObject *parent = 0);
    virtual ~HvLocSrc();

    static bool ParseMsg(const QString &msg,QString &call,QString &grid);// "CQ K1ABC FN42" -> K1ABC FN42
    static bool IsGrid(const QString &g);                                 // 4 or 6 characters
    static QString Tag(int s);                                            // RX C3 EME JTDX WEB
    static QString Name(int s);
    static int Periods() { return 20; }

    bool IsOn(int s) const { return (s >= 0 && s < COUNT) ? f_on[s] : false; }
    void SetOn(int s,bool f);
    bool AutoUpdate() const { return f_auto; }
    void SetAutoUpdate(bool f);

    void AddRx(const QString &msg);                        // a decoded message (live decodes only)
    QString FindOne(int s,const QString &call,double period_s) const;   // "" = not found
    QString FindLocal(const QString &call,double period_s,int &src) const;// RX, C3, EME, JTDX
    QString Tooltip(const QString &call,double period_s,const QString &db_loc) const;
    void RequestOnline(const QString &call);                // answer with EmitOnline()
    QString Status(int s) const;
    bool Busy() const { return dl_cur >= 0 || !dl_queue.isEmpty(); }
    void UpdateNow();

signals:
    void EmitOnline(QString call,QString grid);              // grid "" = not found
    void EmitChanged();                                      // status changed (page)
    void EmitListLoaded();                                   // a list is ready: lookups may find more

private slots:
    void CheckUpdates();
    void NetFinished();
    void CurlFinished(int,QProcess::ExitStatus);
    void DlTimeout();
    void LoaderFinished();
    void WebFinished();
    void WebCurlFinished(int,QProcess::ExitStatus);
    void WebTimeout();

private:
    struct RxEnt { QString grid; qint64 ms; };
    struct WebEnt { QString grid; qint64 t; };
    QString s_dir;
    QString s_set;
    bool f_on[COUNT];
    bool f_auto;
    bool f_block_save;
    HvLocTablePtr tables[COUNT];
    int gen[COUNT];                  // only the last load of a list is used
    QList<HvLocLoader*> loaders;
    QString st_err[COUNT];
    QHash<QString,RxEnt> rx;
    int rx_adds;
    mutable double rx_period;
    QNetworkAccessManager *nam;
    QTimer *t_check;
    QTimer *t_dl_to;
    QList<int> dl_queue;
    int dl_cur;
    QNetworkReply *dl_reply;
    QProcess *dl_proc;
    bool f_force;
    QHash<QString,WebEnt> web;
    QString web_cur;
    QString web_next;
    qint64 web_backoff;
    QNetworkReply *web_reply;
    QProcess *web_proc;
    QTimer *t_web_to;
    QString web_err;
    int web_asked;

    QString FilePath(int s) const;
    void ReadSettings();
    void SaveSettings();
    void Load(int s,const QString &file);
    void NextDownload();
    void DownloadDone(bool ok,const QByteArray &data,const QString &err);
    void WebDone(const QByteArray &data,int http,const QString &err);
    void WebStart(const QString &call);
    void ReadWebCache();
    void SaveWebCache();
};

class HvLocSrcW : public QWidget   // page 4 of "Network Configuration"
{
    Q_OBJECT
public:
    HvLocSrcW(HvLocSrc *src,QWidget *parent = 0);

private slots:
    void Toggled();
    void Refresh();
    void UpdateNow();

private:
    HvLocSrc *s;
    QCheckBox *cb[HvLocSrc::COUNT];
    QLabel *l_st[HvLocSrc::COUNT];
    QCheckBox *cb_auto;
    QPushButton *pb_upd;
};
#endif
