/* MSHV-IC8TEM  Calls that heard me: reception reports of my callsign from PSKReporter.
 * Idea from WsjtxUtils.Searchlight by KC3PIB (MIT, https://github.com/KC3PIB/WsjtxUtils.Searchlight);
 * no code taken from it.
 *
 * About one minute after the start, then every 5 minutes (as PSKReporter asks) while I
 * transmit, the reports of the last 30 minutes in which my call was received are read from
 * https://retrieve.pskreporter.info/query. The decode lists underline (fluorescent yellow by
 * default, Text Highlight) the calls of the stations that received me on the band in use.
 *
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef HVPSKHEARD_H
#define HVPSKHEARD_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QProcess>

class QNetworkAccessManager;
class QNetworkReply;
class QTimer;

class HvPskHeard : public QObject
{
    Q_OBJECT
public:
    explicit HvPskHeard(QObject *parent = 0);
    virtual ~HvPskHeard();

    // decode lists (GUI thread): did the station with this call receive me on the band in use?
    static bool IsHeard(const QString &call);
    static bool Active();

    void SetMyCall(const QString &call);
    void SetBand(const QString &band);   // "144 MHz" as in lst_bands
    void NoteTx(bool on);

    // helpers, public for the tests
    static QString BaseCall(const QString &call);
    static int BandIndex(qint64 hz);     // index in lst_bands, -1 = no band
    typedef QHash<QString, QHash<int, qint64> > HeardMap;   // call -> band -> last report (UTC s)
    static bool Parse(const QByteArray &xml, HeardMap &exact, QString &err);

public slots:
    void SetEnabled(bool);

signals:
    void Updated();          // repaint the decode lists
    void Status(QString);    // shown in Text Highlight

private slots:
    void Tick();
    void NetFinished();
    void CurlFinished(int,QProcess::ExitStatus);
    void TimedOut();

private:
    QNetworkAccessManager *nam;
    QNetworkReply *reply;
    QProcess *proc;
    QTimer *timer;
    QTimer *t_to;
    QString my_call;
    qint64 t_start;          // ms: first request about one minute after this
    qint64 t_last_query;     // ms, 0 = never
    qint64 t_last_tx;        // ms, 0 = never
    bool f_tx;
    QString s_updated;       // HH:mm UTC of the last answer
    QString s_error;
    void Query();
    void Done(const QByteArray &body,int http,const QString &err);
    void SendStatus();
};
#endif
