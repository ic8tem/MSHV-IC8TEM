/* MSHV-IC8TEM  Calls that heard me: reception reports of my callsign from PSKReporter.
 * Idea from WsjtxUtils.Searchlight by KC3PIB (MIT, https://github.com/KC3PIB/WsjtxUtils.Searchlight);
 * no code taken from it.
 *
 * About one minute after the start, then every 5 minutes (as PSKReporter asks) while I
 * transmit, the reports of the last 30 minutes in which my call was received are read from
 * https://retrieve.pskreporter.info/query. The decode lists underline (fluorescent yellow by
 * default, Text Highlight) the calls of the stations that received me on the band in use.
 *
 * Every MSHV band (135 kHz ... 248 GHz). The band of a report comes from its frequency; when
 * the frequency is missing or in no band (programs that send it in 4 bytes cannot go above
 * 4.29 GHz) it is the band I was transmitting on at the time of the report, and when the
 * frequency is the IF of a transverter (28, 144, 432, 1296, 2320 MHz) the report counts also
 * for the band I was transmitting on. The answers are added together (PSKReporter gives only
 * the last 100 records): a report counts for 35 minutes.
 *
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef HVPSKHEARD_H
#define HVPSKHEARD_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QList>
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
    struct Report
    {
        QString call;   // receiver
        qint64 hz;      // 0 = not given
        qint64 t;       // UTC s
    };
    typedef QList<Report> Reports;
    typedef QHash<QString, QHash<int, qint64> > HeardMap;   // call -> band -> last report (UTC s)
    static QString BaseCall(const QString &call);
    static int BandIndex(qint64 hz);     // index in lst_bands, -1 = no band
    static int BandOfName(const QString &band);
    static bool IsIfBand(int band);      // usual IF of transverters
    static bool Parse(const QByteArray &xml, Reports &out, QString &err);
    int TxBandAt(qint64 t) const;        // band I was transmitting on at t (UTC s), -1 = none
    QList<int> BandsOf(qint64 hz, qint64 t) const;
    void Merge(const Reports &reps);
    int CountOnBand() const;

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
    struct TxSeg
    {
        qint64 start;        // UTC s
        qint64 end;          // UTC s, -1 = still transmitting
        int band;
    };
    QList<TxSeg> tx_hist;    // my transmissions of the last 45 minutes, with their band
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
