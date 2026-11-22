/* MSHV-IC8TEM  Calls that heard me (PSKReporter reception reports). See hvpskheard.h.
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include "hvpskheard.h"
#include "../mshv_ic8tem_version.h"
// MSHV's own band tables (the same as the band menu): this unit gets its own static copy
#define _BANDS_H_
#define _FREQTOBAND_H_
#include "../config_band_all.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslSocket>
#include <QStandardPaths>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QXmlStreamReader>

#define PSKH_URL      "https://retrieve.pskreporter.info/query"
#define PSKH_CONTACT  "ic8tem@gmail.com"        // PSKReporter asks automated clients for a contact
#define PSKH_UA       "MSHV-IC8TEM/" MSHV_IC8TEM_VERSION " (+https://github.com/ic8tem/MSHV-IC8TEM)"
#define PSKH_WINDOW_S 1800                      // reports of the last 30 minutes
#define PSKH_PERIOD_MS (300*1000)               // PSKReporter: at most one request every 5 minutes
#define PSKH_FIRST_MS (60*1000)                 // first request about one minute after the start

namespace
{
bool g_enabled = false;
int g_band = -1;                     // band in use, index of lst_bands
HvPskHeard::HeardMap g_exact;        // receiver call as reported -> band -> last report (UTC s)
HvPskHeard::HeardMap g_match;        // the same, also under the base call (IZ0ABC/P -> IZ0ABC)

qint64 NowMs()
{
    return QDateTime::currentMSecsSinceEpoch();
}
bool ValidCall(const QString &c)
{
    if (c.length() < 3 || c.startsWith('_') || c == "NOCALL") return false;
    bool dig = false, let = false;
    for (int i = 0; i < c.length(); ++i)
    {
        if (c.at(i).isDigit()) dig = true;
        else if (c.at(i).isLetter()) let = true;
    }
    return dig && let;
}
void Put(HvPskHeard::HeardMap &m,const QString &call,int band,qint64 t)
{
    QHash<int, qint64> &b = m[call];
    if (b.value(band, 0) < t) b[band] = t;
}
}

HvPskHeard::HvPskHeard(QObject *parent)
        : QObject(parent)
{
    nam = 0;
    reply = 0;
    proc = 0;
    t_start = NowMs();
    t_last_query = 0;
    t_last_tx = 0;
    f_tx = false;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Tick()));
    timer->start(20000);
    t_to = new QTimer(this);
    t_to->setSingleShot(true);
    connect(t_to, SIGNAL(timeout()), this, SLOT(TimedOut()));
}
HvPskHeard::~HvPskHeard()
{}
bool HvPskHeard::Active()
{
    return g_enabled && g_band >= 0 && !g_match.isEmpty();
}
bool HvPskHeard::IsHeard(const QString &call)
{
    if (!Active()) return false;
    const qint64 now = NowMs()/1000;
    QString u = call.toUpper();
    u.remove('<');
    u.remove('>');
    QStringList keys;
    keys << u;
    QString base = BaseCall(u);
    if (base != u) keys << base;
    for (int i = 0; i < keys.count(); ++i)
    {
        HeardMap::const_iterator it = g_match.constFind(keys.at(i));
        if (it == g_match.constEnd()) continue;
        QHash<int, qint64>::const_iterator b = it->constFind(g_band);
        // a report stays valid for the window plus one period (no new answer while not transmitting)
        if (b != it->constEnd() && now - b.value() <= PSKH_WINDOW_S + PSKH_PERIOD_MS/1000) return true;
    }
    return false;
}
QString HvPskHeard::BaseCall(const QString &call)
{
    QString u = call.toUpper().trimmed();
    u.remove('<');
    u.remove('>');
    if (!u.contains('/')) return u;
    QStringList parts = u.split('/');
    QString best;
    for (int i = 0; i < parts.count(); ++i)
    {
        const QString &p = parts.at(i);
        bool dig = false, let = false;
        for (int j = 0; j < p.length(); ++j)
        {
            if (p.at(j).isDigit()) dig = true;
            else if (p.at(j).isLetter()) let = true;
        }
        if (dig && let && p.length() > best.length()) best = p;
    }
    return best.isEmpty() ? u : best;
}
int HvPskHeard::BandIndex(qint64 hz)
{
    if (hz <= 0) return -1;
    for (int i = 0; i < COUNT_BANDS; ++i)
    {
        if ((unsigned long long)hz >= freq_min_max[i].min && (unsigned long long)hz <= freq_min_max[i].max) return i;
    }
    return -1;
}
bool HvPskHeard::Parse(const QByteArray &xml,HeardMap &exact,QString &err)
{
    QXmlStreamReader r(xml);
    bool root = false;
    while (!r.atEnd())
    {
        r.readNext();
        if (!r.isStartElement()) continue;
        if (r.name() == QLatin1String("receptionReports"))
        {
            root = true;
            continue;
        }
        if (r.name() != QLatin1String("receptionReport")) continue;
        QXmlStreamAttributes a = r.attributes();
        QString rc = a.value(QLatin1String("receiverCallsign")).toString().trimmed().toUpper();
        qint64 hz = a.value(QLatin1String("frequency")).toString().toLongLong();
        qint64 t = a.value(QLatin1String("flowStartSeconds")).toString().toLongLong();
        if (rc.isEmpty() || t <= 0) continue;
        int b = BandIndex(hz);
        if (b < 0) continue;
        Put(exact, rc, b, t);
    }
    if (r.hasError())
    {
        err = r.errorString();
        return false;
    }
    if (!root)
    {
        err = "no reception reports in the answer";
        return false;
    }
    return true;
}
void HvPskHeard::SetEnabled(bool f)
{
    if (g_enabled == f) return;
    g_enabled = f;
    if (f && t_last_query == 0) t_start = NowMs();
    SendStatus();
    emit Updated();
}
void HvPskHeard::SetMyCall(const QString &call)
{
    QString c = call.trimmed().toUpper();
    if (c == my_call) return;
    my_call = c;
    g_exact.clear();      // the reports were for the other call
    g_match.clear();
    t_last_query = 0;
    t_start = NowMs();
    s_updated.clear();
    s_error.clear();
    SendStatus();
    emit Updated();
}
void HvPskHeard::SetBand(const QString &band)
{
    int b = -1;
    for (int i = 0; i < COUNT_BANDS; ++i)
    {
        if (lst_bands[i] == band)
        {
            b = i;
            break;
        }
    }
    if (b == g_band) return;
    g_band = b;
    SendStatus();
    emit Updated();
}
void HvPskHeard::NoteTx(bool on)
{
    if (on || f_tx) t_last_tx = NowMs();   // start and end of a transmission
    f_tx = on;
}
void HvPskHeard::Tick()
{
    if (!g_enabled || reply || proc) return;
    if (!ValidCall(my_call)) return;
    const qint64 now = NowMs();
    if (t_last_query == 0)
    {
        if (now - t_start < PSKH_FIRST_MS) return;
    }
    else
    {
        if (now - t_last_query < PSKH_PERIOD_MS) return;
        // new reports only come while transmitting: no request after 30 minutes without TX
        if (!f_tx && (t_last_tx == 0 || now - t_last_tx > (qint64)PSKH_WINDOW_S*1000)) return;
    }
    Query();
}
void HvPskHeard::Query()
{
    t_last_query = NowMs();
    QString base = QString::fromLocal8Bit(qgetenv("MSHV_IC8TEM_PSKREPORTER_URL"));// tests only
    if (base.isEmpty()) base = PSKH_URL;
    QString url = base+"?senderCallsign="+QString::fromLatin1(QUrl::toPercentEncoding(my_call))+
                  "&flowStartSeconds=-"+QString::number(PSKH_WINDOW_S)+"&rronly=1&noactive=1"+
                  "&appcontact="+QString::fromLatin1(QUrl::toPercentEncoding(PSKH_CONTACT));
    t_to->start(30000);
    if (QSslSocket::supportsSsl())
    {
        if (!nam) nam = new QNetworkAccessManager(this);
        QNetworkRequest req((QUrl(url)));
        req.setHeader(QNetworkRequest::UserAgentHeader, PSKH_UA);
        req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
        reply = nam->get(req);
        connect(reply, SIGNAL(finished()), this, SLOT(NetFinished()));
        return;
    }
    QString curl = QStandardPaths::findExecutable("curl");// Qt without OpenSSL (Windows): curl of the system
    if (curl.isEmpty())
    {
        t_to->stop();
        Done(QByteArray(),0,tr("HTTPS is not available (no OpenSSL and no curl)"));
        return;
    }
    proc = new QProcess(this);
    QStringList a;
    a << "-s" << "-S" << "-L" << "--max-time" << "25" << "-A" << PSKH_UA << "-w" << "\n%{http_code}" << url;
    connect(proc, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(CurlFinished(int,QProcess::ExitStatus)));
    proc->start(curl,a);
}
void HvPskHeard::NetFinished()
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender());
    if (!r) return;
    r->deleteLater();
    if (r != reply) return;
    reply = 0;
    t_to->stop();
    int http = r->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray d = r->readAll();
    Done(d,http,r->error() == QNetworkReply::NoError ? QString() : r->errorString());
}
void HvPskHeard::CurlFinished(int code,QProcess::ExitStatus st)
{
    QProcess *p = qobject_cast<QProcess*>(sender());
    if (!p) return;
    p->deleteLater();
    if (p != proc) return;
    proc = 0;
    t_to->stop();
    QByteArray out = p->readAllStandardOutput();
    int nl = out.lastIndexOf('\n');
    int http = (nl >= 0) ? out.mid(nl+1).trimmed().toInt() : 0;
    QByteArray body = (nl >= 0) ? out.left(nl) : out;
    QString err;
    if (st != QProcess::NormalExit || code != 0)
    {
        err = QString::fromLocal8Bit(p->readAllStandardError()).trimmed();
        if (err.isEmpty()) err = "curl "+QString::number(code);
    }
    Done(body,http,err);
}
void HvPskHeard::TimedOut()
{
    if (reply)
    {
        QNetworkReply *r = reply;
        reply = 0;
        r->abort();
    }
    if (proc)
    {
        QProcess *p = proc;
        proc = 0;
        p->kill();
        p->deleteLater();
    }
    Done(QByteArray(),0,tr("no answer"));
}
void HvPskHeard::Done(const QByteArray &body,int http,const QString &err)
{
    QString e = err;
    if (e.isEmpty() && http != 200) e = "HTTP "+QString::number(http);
    HeardMap exact;
    if (e.isEmpty()) Parse(body,exact,e);
    if (!e.isEmpty())
    {
        // the reports already read stay valid (they expire with their time)
        QString t = QString::fromUtf8(body).simplified();
        if (!t.isEmpty() && !t.startsWith('<') && t.length() < 160) e += " - "+t;// PSKReporter's own message
        s_error = e;
        SendStatus();
        return;
    }
    s_error.clear();
    g_exact = exact;
    g_match = exact;
    for (HeardMap::const_iterator it = exact.constBegin(); it != exact.constEnd(); ++it)
    {
        QString base = BaseCall(it.key());
        if (base == it.key()) continue;
        for (QHash<int, qint64>::const_iterator b = it->constBegin(); b != it->constEnd(); ++b)
            Put(g_match, base, b.key(), b.value());
    }
    s_updated = QDateTime::currentDateTimeUtc().toString("HH:mm");
    SendStatus();
    emit Updated();
}
void HvPskHeard::SendStatus()
{
    QString s;
    if (!g_enabled) s.clear();// switched off: no status under the check box
    else if (!ValidCall(my_call)) s = tr("PSKReporter: your call is not set (Options > Macros)");
    else if (!s_error.isEmpty()) s = tr("PSKReporter: error %1").arg(s_error);
    else if (s_updated.isEmpty()) s = tr("PSKReporter: first request about one minute after the start, then every 5 minutes while you transmit");
    else
    {
        int n = 0;
        for (HeardMap::const_iterator it = g_exact.constBegin(); it != g_exact.constEnd(); ++it)
            if (it->contains(g_band)) n++;
        QString band = (g_band >= 0 && g_band < COUNT_BANDS) ? lst_bands[g_band] : QString("-");
        s = tr("Stations that heard you on %2: %1 (PSKReporter, %3 UTC)").arg(n).arg(band).arg(s_updated);
    }
    emit Status(s);
}
