/* MSHV-IC8TEM  Locator lookup sources. See hvlocsrc.h.
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include "hvlocsrc.h"
#include "../mshv_ic8tem_version.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslSocket>
#include <QStandardPaths>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <algorithm>
#include <string.h>

#define LOCSRC_UA "MSHV-IC8TEM/" MSHV_IC8TEM_VERSION " (+https://github.com/ic8tem/MSHV-IC8TEM)"
#define LOCSRC_REFRESH_DAYS 14
#define LOCSRC_WEB_URL "https://api.g7vrd.co.uk/v1/locators/by-callsign/"
#define LOCSRC_WEB_DAYS 30          // an answer with a locator is kept 30 days
#define LOCSRC_WEB_MAX 5000

static const char *const kUrl[HvLocSrc::COUNT] =
{
    "",
    "https://downloads.sourceforge.net/project/wsjt-x-improved/Additional%20Files/CALL3.TXT",
    "https://www.bigskyspaces.com/w7gj/CALL3.TXT",
    "https://downloads.sourceforge.net/project/jtdx/grid_data.bin",
    LOCSRC_WEB_URL
};
static const char *const kFile[HvLocSrc::COUNT] =
{
    "", "CALL3_WSJT-X_Improved.TXT", "CALL3_EME_W7GJ.TXT", "grid_data.bin", "g7vrd_cache.txt"
};

// ------------------------------------------------------------------ helpers
static inline bool InR(QChar c,char lo,char hi)
{
    return c.unicode() >= (ushort)(uchar)lo && c.unicode() <= (ushort)(uchar)hi;
}
bool HvLocSrc::IsGrid(const QString &g)
{
    int n = g.size();
    if (n != 4 && n != 6) return false;
    if (!InR(g.at(0),'A','R') || !InR(g.at(1),'A','R') || !InR(g.at(2),'0','9') || !InR(g.at(3),'0','9')) return false;
    if (n == 6 && (!InR(g.at(4),'A','X') || !InR(g.at(5),'A','X'))) return false;
    return true;
}
static QString NormGrid(const QString &raw)// "fn42ab12" -> "FN42AB", "FN42" -> "FN42", else ""
{
    QString g = raw.trimmed().toUpper();
    if (g.size() >= 6 && HvLocSrc::IsGrid(g.left(6))) return g.left(6);
    if (g.size() >= 4 && HvLocSrc::IsGrid(g.left(4))) return g.left(4);
    return QString();
}
static bool LooksLikeCall(const QString &c)
{
    int n = c.size();
    if (n < 3 || n > 12) return false;
    if (c.at(0) == QChar('/') || c.at(n-1) == QChar('/') || c.contains("//")) return false;
    if (c.at(0) == QChar('Q') || c.at(0) == QChar('0')) return false;// as HvQthLoc::isValidCallsign
    bool d = false, l = false;
    for (int i = 0; i < n; ++i)
    {
        QChar ch = c.at(i);
        if (InR(ch,'0','9')) d = true;
        else if (InR(ch,'A','Z')) l = true;
        else if (ch != QChar('/')) return false;
    }
    return d && l;
}
static bool NumLike(const QString &w)// contest serial between the call and the grid ("570007")
{
    if (w.isEmpty()) return false;
    for (int i = 0; i < w.size(); ++i)
    {
        QChar c = w.at(i);
        if (!InR(c,'0','9') && !(i == 0 && (c == QChar('+') || c == QChar('-')))) return false;
    }
    return true;
}
static quint32 GridEnc(const QString &g)
{
    quint32 f1 = g.at(0).unicode()-'A', f2 = g.at(1).unicode()-'A';
    quint32 s1 = g.at(2).unicode()-'0', s2 = g.at(3).unicode()-'0';
    quint32 u1 = 0, u2 = 0;
    if (g.size() >= 6)
    {
        u1 = g.at(4).unicode()-'A'+1;
        u2 = g.at(5).unicode()-'A'+1;
    }
    return (((((f1*18+f2)*10+s1)*10+s2)*25+u1)*25+u2)+1;
}
static QString GridDec(quint32 c)
{
    if (c == 0) return QString();
    c -= 1;
    quint32 u2 = c%25; c /= 25;
    quint32 u1 = c%25; c /= 25;
    quint32 s2 = c%10; c /= 10;
    quint32 s1 = c%10; c /= 10;
    quint32 f2 = c%18; c /= 18;
    quint32 f1 = c;
    if (f1 > 17) return QString();
    QString g;
    g += QChar('A'+f1);
    g += QChar('A'+f2);
    g += QChar('0'+s1);
    g += QChar('0'+s2);
    if (u1 && u2)
    {
        g += QChar('A'+u1-1);
        g += QChar('A'+u2-1);
    }
    return g;
}
static void CallKey(const QString &call,char *k)
{
    memset(k,0,12);
    QByteArray a = call.toLatin1();
    memcpy(k,a.constData(),qMin(12,a.size()));
}
static bool RecLess(const HvLocRec &a,const HvLocRec &b)
{
    return memcmp(a.c,b.c,12) < 0;
}
static void AddRec(QVector<HvLocRec> &v,const QString &call,const QString &grid)
{
    if (call.size() > 12) return;
    HvLocRec r;
    CallKey(call,r.c);
    r.g = GridEnc(grid);
    v.append(r);
}
static void FinishTable(QVector<HvLocRec> &v)// sorted by call, one record for each call (the first one)
{
    std::stable_sort(v.begin(),v.end(),RecLess);
    int o = 0;
    for (int i = 0; i < v.size(); ++i)
    {
        if (o > 0 && memcmp(v.at(o-1).c,v.at(i).c,12) == 0) continue;
        v[o++] = v.at(i);
    }
    v.resize(o);
    v.squeeze();
}
bool HvLocTable::Find(const QString &call,QString &grid) const
{
    if (v.isEmpty() || call.isEmpty() || call.size() > 12) return false;
    HvLocRec k;
    CallKey(call,k.c);
    k.g = 0;
    QVector<HvLocRec>::const_iterator it = std::lower_bound(v.constBegin(),v.constEnd(),k,RecLess);
    if (it == v.constEnd() || memcmp(it->c,k.c,12) != 0) return false;
    grid = GridDec(it->g);
    return !grid.isEmpty();
}
// CALL3.TXT: "CALL,LOCATOR,EME flag,...", "//" comments, last record "ZZZZZZ,..."
static bool ParseCall3(const QByteArray &data,HvLocTable &t,QString &err)
{
    if (data.size() < 100 || data.trimmed().startsWith('<'))
    {
        err = "not a CALL3.TXT file";
        return false;
    }
    t.v.reserve(data.size()/24);
    int pos = 0, n = data.size(), lines = 0;
    while (pos < n)
    {
        int e = data.indexOf('\n',pos);
        if (e < 0) e = n;
        QByteArray line = data.mid(pos,e-pos).trimmed();
        pos = e+1;
        lines++;
        if (line.isEmpty()) continue;
        if (line.startsWith("//"))
        {
            if (t.info.isEmpty() && lines < 8)
            {
                int r = line.indexOf("Revised");
                if (r >= 0) t.info = QString::fromLatin1(line.mid(r)).simplified();
            }
            continue;
        }
        if (line.startsWith("ZZZZ")) continue;
        int c1 = line.indexOf(',');
        if (c1 <= 0) continue;
        int c2 = line.indexOf(',',c1+1);
        QString call = QString::fromLatin1(line.left(c1)).trimmed().toUpper();
        QString grid = NormGrid(QString::fromLatin1(c2 < 0 ? line.mid(c1+1) : line.mid(c1+1,c2-c1-1)));
        if (grid.isEmpty() || !LooksLikeCall(call)) continue;
        AddRec(t.v,call,grid);
    }
    FinishTable(t.v);
    if (t.v.size() < 300)
    {
        err = "too few valid lines ("+QString::number(t.v.size())+")";
        return false;
    }
    return true;
}
// grid_data.bin (JTDX): qCompress( QDataStream BigEndian: quint32 rows, rows x (QString call, QString grid) )
static bool ParseGridData(const QByteArray &raw,HvLocTable &t,QString &err)
{
    if (raw.size() <= 8 || (unsigned char)raw.at(4) != 0x78)
    {
        err = "not a grid_data.bin file";
        return false;
    }
    QByteArray d = qUncompress(raw);
    if (d.isEmpty())
    {
        err = "cannot decompress";
        return false;
    }
    QDataStream in(d);
    in.setByteOrder(QDataStream::BigEndian);
    quint32 rows = 0;
    in >> rows;
    if (in.status() != QDataStream::Ok || rows == 0 || rows > 2000000)
    {
        err = "bad number of rows ("+QString::number(rows)+")";
        return false;
    }
    t.v.reserve((int)rows);
    for (quint32 i = 0; i < rows; ++i)
    {
        QString k,g;
        in >> k >> g;
        if (in.status() != QDataStream::Ok)
        {
            err = "truncated at row "+QString::number(i);
            return false;
        }
        k = k.trimmed().toUpper();
        k.remove('<');
        k.remove('>');
        g = NormGrid(g);
        if (g.isEmpty() || !LooksLikeCall(k)) continue;
        AddRec(t.v,k,g);
    }
    d.clear();
    FinishTable(t.v);
    if (t.v.size() < 1000)
    {
        err = "too few valid rows ("+QString::number(t.v.size())+")";
        return false;
    }
    return true;
}

// ------------------------------------------------------------------ loader thread
HvLocLoader::HvLocLoader(int i,const QString &f,QObject *parent)
        : QThread(parent)
{
    id = i;
    file = f;
    gen = 0;
}
void HvLocLoader::run()
{
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly))
    {
        error = "cannot open "+QFileInfo(file).fileName();
        return;
    }
    QByteArray data = f.readAll();
    f.close();
    HvLocTablePtr t(new HvLocTable());
    bool ok = (id == HvLocSrc::JTDX) ? ParseGridData(data,*t,error) : ParseCall3(data,*t,error);
    if (ok) table = t;
}

// ------------------------------------------------------------------ messages
// "CQ [DX] CALL GRID", "QRZ CALL GRID", "CALL1 CALL2 [R] GRID", "CALL1 CALL2 R 570007 JN70AA", "CALL GRID",
// JT65 "CALL1 CALL2 GRID OOO". Not RR73. The grid belongs to the station that sent the message.
bool HvLocSrc::ParseMsg(const QString &msg0,QString &call,QString &grid)
{
    call.clear();
    grid.clear();
    QString m = msg0;
    m.replace('\n',' ');
    m.remove('<');
    m.remove('>');
    m = m.simplified();
    if (m.startsWith("TU; ")) m.remove(0,4);
    if (m.isEmpty()) return false;
    QStringList w = m.split(' ');
    while (!w.isEmpty())// marks of some decoders at the end
    {
        const QString &l = w.last();
        if (l == "OOO" || l == "?" || l == "*" || l == "#" || l == "^" || l == "." ||
                (l.size() == 2 && (l.at(0) == QChar('a') || l.at(0) == QChar('q')) && l.at(1).isDigit()))
            w.removeLast();
        else break;
    }
    if (w.size() < 2 || w.size() > 7) return false;
    QString g = w.last().toUpper();
    if (g == "RR73" || !IsGrid(g)) return false;
    int i = w.size()-2;
    while (i > 0 && (w.at(i) == "R" || NumLike(w.at(i)))) --i;
    QString c = w.at(i).toUpper();
    if (!LooksLikeCall(c) || IsGrid(c)) return false;
    if (i >= 1)
    {
        QString w0 = w.at(0).toUpper();
        bool cq = (w0 == "CQ" || w0 == "QRZ" || w0 == "DE");
        if (cq && i > 2) return false;
        if (!cq && !(i == 1 && LooksLikeCall(w0))) return false;
    }
    call = c;
    grid = g;
    return true;
}

// ------------------------------------------------------------------ HvLocSrc
HvLocSrc::HvLocSrc(const QString &app_path,QObject *parent)
        : QObject(parent)
{
    s_dir = app_path+"/settings/database/locsources";
    s_set = app_path+"/settings/ms_locsrc";
    for (int i = 0; i < COUNT; ++i)
    {
        f_on[i] = true;
        gen[i] = 0;
    }
    f_auto = true;
    f_block_save = true;
    rx_adds = 0;
    rx_period = 15.0;
    dl_cur = -1;
    dl_reply = 0;
    dl_proc = 0;
    f_force = false;
    web_backoff = 0;
    web_reply = 0;
    web_proc = 0;
    web_asked = 0;
    nam = new QNetworkAccessManager(this);
    t_dl_to = new QTimer(this);
    t_dl_to->setSingleShot(true);
    connect(t_dl_to, SIGNAL(timeout()), this, SLOT(DlTimeout()));
    t_web_to = new QTimer(this);
    t_web_to->setSingleShot(true);
    connect(t_web_to, SIGNAL(timeout()), this, SLOT(WebTimeout()));
    t_check = new QTimer(this);
    connect(t_check, SIGNAL(timeout()), this, SLOT(CheckUpdates()));
    ReadSettings();
    f_block_save = false;
    ReadWebCache();
    for (int s = C3; s <= JTDX; ++s)
    {
        if (f_on[s] && QFile::exists(FilePath(s))) Load(s,FilePath(s));
    }
    QTimer::singleShot(30000, this, SLOT(CheckUpdates()));// not during the start of MSHV
    t_check->start(6*3600*1000);
}
HvLocSrc::~HvLocSrc()
{
    for (int i = 0; i < loaders.count(); ++i)
    {
        HvLocLoader *l = loaders.at(i);
        disconnect(l, 0, this, 0);
        l->wait();
        delete l;
    }
    loaders.clear();
    if (dl_reply) dl_reply->abort();
    if (web_reply) web_reply->abort();
}
QString HvLocSrc::Tag(int s)
{
    switch (s)
    {
    case RX:   return "RX";
    case C3:   return "C3";
    case EME:  return "EME";
    case JTDX: return "JTDX";
    case WEB:  return "WEB";
    default:   return "DB";
    }
}
QString HvLocSrc::Name(int s)
{
    switch (s)
    {
    case RX:   return tr("Decodes of the last %1 periods").arg(Periods());
    case C3:   return "CALL3.TXT (WSJT-X Improved)";
    case EME:  return "CALL3.TXT EME (W7GJ)";
    case JTDX: return "grid_data.bin (JTDX)";
    case WEB:  return tr("Online")+" (api.g7vrd.co.uk)";
    default:   return tr("MSHV database");
    }
}
QString HvLocSrc::FilePath(int s) const
{
    return s_dir+"/"+kFile[s];
}
void HvLocSrc::ReadSettings()
{
    QFile file(s_set);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        int p = line.indexOf('=');
        if (p <= 0) continue;
        QString k = line.left(p);
        bool v = (line.mid(p+1) == "1");
        if (k == "rx") f_on[RX] = v;
        else if (k == "call3") f_on[C3] = v;
        else if (k == "eme") f_on[EME] = v;
        else if (k == "jtdx") f_on[JTDX] = v;
        else if (k == "web") f_on[WEB] = v;
        else if (k == "auto_update") f_auto = v;
    }
    file.close();
}
void HvLocSrc::SaveSettings()
{
    if (f_block_save) return;
    QFile file(s_set);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    out << "rx=" << (f_on[RX] ? 1 : 0) << "\n";
    out << "call3=" << (f_on[C3] ? 1 : 0) << "\n";
    out << "eme=" << (f_on[EME] ? 1 : 0) << "\n";
    out << "jtdx=" << (f_on[JTDX] ? 1 : 0) << "\n";
    out << "web=" << (f_on[WEB] ? 1 : 0) << "\n";
    out << "auto_update=" << (f_auto ? 1 : 0) << "\n";
    file.close();
}
void HvLocSrc::SetOn(int s,bool f)
{
    if (s < 0 || s >= COUNT || f_on[s] == f) return;
    f_on[s] = f;
    if (s >= C3 && s <= JTDX)
    {
        if (!f)
        {
            tables[s].clear();
            gen[s]++;
            dl_queue.removeAll(s);
        }
        else
        {
            if (QFile::exists(FilePath(s))) Load(s,FilePath(s));
            QTimer::singleShot(500, this, SLOT(CheckUpdates()));
        }
    }
    if (s == RX && !f) rx.clear();
    SaveSettings();
    emit EmitChanged();
}
void HvLocSrc::SetAutoUpdate(bool f)
{
    if (f_auto == f) return;
    f_auto = f;
    SaveSettings();
    if (f) QTimer::singleShot(500, this, SLOT(CheckUpdates()));
    emit EmitChanged();
}
// ------------------------------------------------------------------ RX
void HvLocSrc::AddRx(const QString &msg)
{
    if (!f_on[RX]) return;
    QString call,grid;
    if (!ParseMsg(msg,call,grid)) return;
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    QHash<QString,RxEnt>::iterator it = rx.find(call);
    if (it != rx.end())
    {
        if (!(grid.size() == 4 && it->grid.size() == 6 && it->grid.startsWith(grid))) it->grid = grid;// keep the 6 characters
        it->ms = now;
    }
    else
    {
        RxEnt e;
        e.grid = grid;
        e.ms = now;
        rx.insert(call,e);
    }
    if (++rx_adds >= 256)
    {
        rx_adds = 0;
        qint64 keep = (qint64)(qMax(rx_period,15.0)*Periods()*2000.0);
        for (it = rx.begin(); it != rx.end();)
        {
            if (now - it->ms > keep) it = rx.erase(it);
            else ++it;
        }
    }
}
// ------------------------------------------------------------------ lookups
QString HvLocSrc::FindOne(int s,const QString &call0,double period_s) const
{
    if (s < 0 || s >= COUNT || !f_on[s]) return QString();
    QString call = call0.trimmed().toUpper();
    if (call.size() < 3) return QString();
    if (s == RX)
    {
        if (period_s > 0.5) rx_period = period_s;
        QHash<QString,RxEnt>::const_iterator it = rx.constFind(call);
        if (it == rx.constEnd()) return QString();
        if (QDateTime::currentMSecsSinceEpoch() - it->ms > (qint64)(rx_period*Periods()*1000.0)) return QString();
        return it->grid;
    }
    if (s == WEB)// answers already received (kept 30 days)
    {
        QHash<QString,WebEnt>::const_iterator it = web.constFind(call);
        if (it == web.constEnd()) return QString();
        if (QDateTime::currentMSecsSinceEpoch()/1000 - it->t > (qint64)LOCSRC_WEB_DAYS*86400) return QString();
        return it->grid;
    }
    QString g;
    if (tables[s] && tables[s]->Find(call,g)) return g;
    return QString();
}
// first source that has the call; a 4 character locator becomes 6 characters
// if a later source has the same square with 6 characters
QString HvLocSrc::FindLocal(const QString &call,double period_s,int &src) const
{
    src = -1;
    QString best;
    for (int s = RX; s < COUNT; ++s)
    {
        QString g = FindOne(s,call,period_s);
        if (g.isEmpty()) continue;
        if (best.isEmpty())
        {
            best = g;
            src = s;
            if (g.size() == 6) break;
            continue;
        }
        if (g.size() == 6 && g.startsWith(best))
        {
            best = g;
            src = s;
            break;
        }
    }
    return best;
}
QString HvLocSrc::Tooltip(const QString &call0,double period_s,const QString &db_loc) const
{
    QString call = call0.trimmed().toUpper();
    QString t = tr("Locator of")+" "+call+":\n"+
                "DB   "+tr("MSHV database")+":  "+(db_loc.isEmpty() ? QString("-") : db_loc);
    for (int s = RX; s < COUNT; ++s)
    {
        QString v;
        if (!f_on[s]) v = tr("off");
        else
        {
            v = FindOne(s,call,period_s);
            if (v.isEmpty())
            {
                if (s == WEB && !web.contains(call)) v = tr("not asked");
                else if (s >= C3 && s <= JTDX && !tables[s]) v = tr("list not loaded");
                else v = "-";
            }
        }
        t += "\n"+Tag(s).leftJustified(5,' ')+Name(s)+":  "+v;
    }
    return t;
}
QString HvLocSrc::Status(int s) const
{
    if (s == RX) return tr("Calls heard with a locator")+": "+QString::number(rx.size());
    if (s == WEB)
    {
        QString t = tr("Answers kept")+": "+QString::number(web.size())+"    "+
                    tr("asked since the start")+": "+QString::number(web_asked);
        if (!web_err.isEmpty()) t += "\n"+web_err;
        return t;
    }
    if (s < C3 || s > JTDX) return QString();
    QString t;
    if (tables[s]) t = tr("Calls")+": "+QString::number(tables[s]->v.size());
    else t = tr("List not loaded");
    QFileInfo fi(FilePath(s));
    if (fi.exists()) t += "    "+tr("downloaded")+": "+fi.lastModified().toString("yyyy-MM-dd");
    if (tables[s] && !tables[s]->info.isEmpty()) t += "    ("+tables[s]->info+")";
    if (dl_cur == s) t += "    "+tr("downloading")+" ...";
    else if (dl_queue.contains(s)) t += "    "+tr("waiting to download");
    else if (!st_err[s].isEmpty()) t += "\n"+st_err[s];
    return t;
}
// ------------------------------------------------------------------ lists: load
void HvLocSrc::Load(int s,const QString &file)
{
    HvLocLoader *l = new HvLocLoader(s,file,0);
    l->gen = ++gen[s];
    loaders.append(l);
    connect(l, SIGNAL(finished()), this, SLOT(LoaderFinished()));
    l->start(QThread::LowPriority);
}
void HvLocSrc::LoaderFinished()
{
    HvLocLoader *l = qobject_cast<HvLocLoader*>(sender());
    if (!l || !loaders.contains(l)) return;
    loaders.removeAll(l);
    int s = l->id;
    bool tmp = l->file.endsWith(".tmp");
    if (l->table)
    {
        if (tmp)
        {
            QFile::remove(FilePath(s));
            if (!QFile::rename(l->file,FilePath(s)))
            {
                QFile::copy(l->file,FilePath(s));
                QFile::remove(l->file);
            }
        }
        bool used = (f_on[s] && l->gen == gen[s]);
        if (used) tables[s] = l->table;
        st_err[s].clear();
        if (used) QTimer::singleShot(0, this, SIGNAL(EmitListLoaded()));
    }
    else
    {
        if (tmp)
        {
            QFile::remove(l->file);
            st_err[s] = tr("The downloaded file is not valid")+": "+l->error;
        }
        else st_err[s] = l->error;
    }
    l->deleteLater();
    if (tmp && dl_cur == s) NextDownload();
    emit EmitChanged();
}
// ------------------------------------------------------------------ lists: download
void HvLocSrc::UpdateNow()
{
    f_force = true;
    CheckUpdates();
}
void HvLocSrc::CheckUpdates()
{
    for (int s = C3; s <= JTDX; ++s)
    {
        if (!f_on[s] || s == dl_cur || dl_queue.contains(s)) continue;
        QFileInfo fi(FilePath(s));
        bool old = !fi.exists() || fi.lastModified().daysTo(QDateTime::currentDateTime()) >= LOCSRC_REFRESH_DAYS;
        if (f_force || (f_auto && old)) dl_queue.append(s);
    }
    f_force = false;
    if (dl_cur < 0) NextDownload();
    else emit EmitChanged();
}
void HvLocSrc::NextDownload()
{
    dl_cur = -1;
    while (!dl_queue.isEmpty())
    {
        int s = dl_queue.takeFirst();
        if (!f_on[s]) continue;
        dl_cur = s;
        break;
    }
    if (dl_cur < 0)
    {
        emit EmitChanged();
        return;
    }
    QDir().mkpath(s_dir);
    st_err[dl_cur].clear();
    emit EmitChanged();
    QString url = kUrl[dl_cur];
    t_dl_to->start(300000);
    if (QSslSocket::supportsSsl())
    {
        QNetworkRequest req((QUrl(url)));
        req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
        req.setMaximumRedirectsAllowed(10);
        req.setHeader(QNetworkRequest::UserAgentHeader, LOCSRC_UA);
        dl_reply = nam->get(req);
        connect(dl_reply, SIGNAL(finished()), this, SLOT(NetFinished()));
        return;
    }
    QString curl = QStandardPaths::findExecutable("curl");// Qt without OpenSSL (Windows): curl of the system
    if (curl.isEmpty())
    {
        DownloadDone(false,QByteArray(),tr("HTTPS is not available (no OpenSSL and no curl)"));
        return;
    }
    dl_proc = new QProcess(this);
    QStringList a;
    a << "-L" << "-s" << "-S" << "-f" << "--max-time" << "280" << "-A" << LOCSRC_UA
      << "-o" << (FilePath(dl_cur)+".dl") << url;
    connect(dl_proc, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(CurlFinished(int,QProcess::ExitStatus)));
    dl_proc->start(curl,a);
}
void HvLocSrc::NetFinished()
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender());
    if (!r) return;
    r->deleteLater();
    if (r != dl_reply) return;
    dl_reply = 0;
    t_dl_to->stop();
    if (r->error() != QNetworkReply::NoError)
    {
        DownloadDone(false,QByteArray(),r->errorString());
        return;
    }
    DownloadDone(true,r->readAll(),QString());
}
void HvLocSrc::CurlFinished(int code,QProcess::ExitStatus st)
{
    QProcess *p = qobject_cast<QProcess*>(sender());
    if (!p) return;
    p->deleteLater();
    if (p != dl_proc) return;
    dl_proc = 0;
    t_dl_to->stop();
    QString dl = FilePath(dl_cur)+".dl";
    if (st != QProcess::NormalExit || code != 0)
    {
        QString err = QString::fromLocal8Bit(p->readAllStandardError()).trimmed();
        QFile::remove(dl);
        DownloadDone(false,QByteArray(),err.isEmpty() ? "curl "+QString::number(code) : err);
        return;
    }
    QFile f(dl);
    QByteArray data;
    if (f.open(QIODevice::ReadOnly))
    {
        data = f.readAll();
        f.close();
    }
    QFile::remove(dl);
    DownloadDone(true,data,QString());
}
void HvLocSrc::DlTimeout()
{
    if (dl_reply)
    {
        QNetworkReply *r = dl_reply;
        dl_reply = 0;
        r->abort();
    }
    if (dl_proc)
    {
        QProcess *p = dl_proc;
        dl_proc = 0;
        p->kill();
        p->deleteLater();
    }
    if (dl_cur >= 0) DownloadDone(false,QByteArray(),tr("no answer (timeout)"));
}
void HvLocSrc::DownloadDone(bool ok,const QByteArray &data,const QString &err)
{
    int s = dl_cur;
    if (s < 0) return;
    if (!ok || data.isEmpty())
    {
        st_err[s] = tr("Download failed")+": "+(err.isEmpty() ? tr("empty answer") : err);
        NextDownload();
        return;
    }
    QString tmp = FilePath(s)+".tmp";
    QFile f(tmp);
    if (!f.open(QIODevice::WriteOnly) || f.write(data) != data.size())
    {
        f.close();
        QFile::remove(tmp);
        st_err[s] = tr("Cannot write")+" "+tmp;
        NextDownload();
        return;
    }
    f.close();
    Load(s,tmp);// checked in the background, then it replaces the old list (NextDownload when done)
}
// ------------------------------------------------------------------ online (G7VRD)
void HvLocSrc::RequestOnline(const QString &call0)
{
    QString call = call0.trimmed().toUpper();
    if (!f_on[WEB] || !LooksLikeCall(call) || call.contains('/'))
    {
        emit EmitOnline(call,QString());
        return;
    }
    qint64 now = QDateTime::currentMSecsSinceEpoch()/1000;
    QHash<QString,WebEnt>::const_iterator it = web.constFind(call);
    if (it != web.constEnd())
    {
        qint64 age = now - it->t;
        if ((!it->grid.isEmpty() && age < (qint64)LOCSRC_WEB_DAYS*86400) || (it->grid.isEmpty() && age < 86400))
        {
            emit EmitOnline(call,it->grid);
            return;
        }
    }
    if (now < web_backoff)
    {
        emit EmitOnline(call,QString());
        return;
    }
    if (!web_cur.isEmpty())
    {
        web_next = call;// only the last one waits
        return;
    }
    WebStart(call);
}
void HvLocSrc::WebStart(const QString &call)
{
    web_cur = call;
    web_asked++;
    QString url = QString(LOCSRC_WEB_URL)+call;
    t_web_to->start(10000);
    if (QSslSocket::supportsSsl())
    {
        QNetworkRequest req((QUrl(url)));
        req.setHeader(QNetworkRequest::UserAgentHeader, LOCSRC_UA);
        req.setRawHeader("Accept","application/json");
        web_reply = nam->get(req);
        connect(web_reply, SIGNAL(finished()), this, SLOT(WebFinished()));
        return;
    }
    QString curl = QStandardPaths::findExecutable("curl");
    if (curl.isEmpty())
    {
        t_web_to->stop();
        WebDone(QByteArray(),0,tr("HTTPS is not available (no OpenSSL and no curl)"));
        return;
    }
    web_proc = new QProcess(this);
    QStringList a;
    a << "-s" << "-S" << "--max-time" << "9" << "-A" << LOCSRC_UA << "-H" << "Accept: application/json"
      << "-w" << "\n%{http_code}" << url;
    connect(web_proc, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(WebCurlFinished(int,QProcess::ExitStatus)));
    web_proc->start(curl,a);
}
void HvLocSrc::WebFinished()
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender());
    if (!r) return;
    r->deleteLater();
    if (r != web_reply) return;
    web_reply = 0;
    t_web_to->stop();
    int http = r->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray d = r->readAll();
    WebDone(d,http,r->error() == QNetworkReply::NoError ? QString() : r->errorString());
}
void HvLocSrc::WebCurlFinished(int code,QProcess::ExitStatus st)
{
    QProcess *p = qobject_cast<QProcess*>(sender());
    if (!p) return;
    p->deleteLater();
    if (p != web_proc) return;
    web_proc = 0;
    t_web_to->stop();
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
    WebDone(body,http,err);
}
void HvLocSrc::WebTimeout()
{
    if (web_reply)
    {
        QNetworkReply *r = web_reply;
        web_reply = 0;
        r->abort();
    }
    if (web_proc)
    {
        QProcess *p = web_proc;
        web_proc = 0;
        p->kill();
        p->deleteLater();
    }
    if (!web_cur.isEmpty()) WebDone(QByteArray(),0,tr("no answer (timeout)"));
}
void HvLocSrc::WebDone(const QByteArray &data,int http,const QString &err)
{
    QString call = web_cur;
    web_cur.clear();
    qint64 now = QDateTime::currentMSecsSinceEpoch()/1000;
    QString grid;
    bool ok = false;
    if (http == 200 && err.isEmpty())
    {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject())
        {
            ok = true;
            QJsonObject all = doc.object().value("locators").toObject();
            grid = NormGrid(all.value(call).toObject().value("locator").toString());
        }
    }
    if (ok)
    {
        web_err.clear();
        WebEnt e;
        e.grid = grid;
        e.t = now;
        web.insert(call,e);
        SaveWebCache();
    }
    else
    {
        web_err = tr("Online lookup failed")+": "+(http > 0 ? "HTTP "+QString::number(http)+"  " : QString())+err;
        web_backoff = now + ((http == 429) ? 900 : 120);// the service asks to slow down: 15 minutes
    }
    emit EmitOnline(call,grid);
    emit EmitChanged();
    if (!web_next.isEmpty())
    {
        QString n = web_next;
        web_next.clear();
        RequestOnline(n);
    }
}
void HvLocSrc::ReadWebCache()
{
    QFile f(FilePath(WEB));
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    qint64 now = QDateTime::currentMSecsSinceEpoch()/1000;
    QTextStream in(&f);
    while (!in.atEnd())
    {
        QStringList l = in.readLine().trimmed().split(',');
        if (l.count() < 3) continue;
        WebEnt e;
        e.grid = NormGrid(l.at(1));
        e.t = l.at(2).toLongLong();
        if (now - e.t > (qint64)LOCSRC_WEB_DAYS*86400) continue;
        if (LooksLikeCall(l.at(0))) web.insert(l.at(0),e);
    }
    f.close();
}
void HvLocSrc::SaveWebCache()
{
    if (web.size() > LOCSRC_WEB_MAX)// drop the oldest answers
    {
        QList<qint64> ts;
        for (QHash<QString,WebEnt>::const_iterator it = web.constBegin(); it != web.constEnd(); ++it) ts.append(it->t);
        std::sort(ts.begin(),ts.end());
        qint64 lim = ts.at(ts.size()-LOCSRC_WEB_MAX);
        for (QHash<QString,WebEnt>::iterator it = web.begin(); it != web.end();)
        {
            if (it->t < lim) it = web.erase(it);
            else ++it;
        }
    }
    QDir().mkpath(s_dir);
    QFile f(FilePath(WEB));
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&f);
    for (QHash<QString,WebEnt>::const_iterator it = web.constBegin(); it != web.constEnd(); ++it)
        out << it.key() << "," << it->grid << "," << it->t << "\n";
    f.close();
}

// ------------------------------------------------------------------ page 4 of "Network Configuration"
HvLocSrcW::HvLocSrcW(HvLocSrc *src,QWidget *parent)
        : QWidget(parent)
{
    s = src;
    QGroupBox *gb = new QGroupBox(tr("Locator lookup when the call is not in the MSHV database"));
    QLabel *l_info = new QLabel(tr("When the call in TO RADIO has no locator in the clicked message and in the MSHV database, "
                                   "MSHV looks in these sources, in this order. The label next to LOCATOR shows where the "
                                   "locator comes from (hold the mouse on it to see all the sources). ADD saves it in the MSHV database."));
    l_info->setWordWrap(true);
    QVBoxLayout *G = new QVBoxLayout();
    G->setContentsMargins(0,0,0,0);
    G->setSpacing(1);
    for (int i = 0; i < HvLocSrc::COUNT; ++i)
    {
        cb[i] = new QCheckBox(HvLocSrc::Tag(i)+":  "+HvLocSrc::Name(i));
        cb[i]->setChecked(s->IsOn(i));
        connect(cb[i], SIGNAL(toggled(bool)), this, SLOT(Toggled()));
        l_st[i] = new QLabel();
        l_st[i]->setWordWrap(true);
        l_st[i]->setContentsMargins(24,0,0,6);
        G->addWidget(cb[i]);
        if (i == HvLocSrc::WEB)
        {
            QLabel *l_web = new QLabel(tr("The online lookup sends the call to api.g7vrd.co.uk only when no other source has "
                                          "the locator; the answers are kept for 30 days."));
            l_web->setWordWrap(true);
            l_web->setContentsMargins(24,0,0,2);
            G->addWidget(l_web);
        }
        G->addWidget(l_st[i]);
    }

    cb_auto = new QCheckBox(tr("Download the lists and update them every %1 days").arg(LOCSRC_REFRESH_DAYS));
    cb_auto->setChecked(s->AutoUpdate());
    connect(cb_auto, SIGNAL(toggled(bool)), this, SLOT(Toggled()));
    pb_upd = new QPushButton(" "+tr("Update Now")+" ");
    connect(pb_upd, SIGNAL(clicked()), this, SLOT(UpdateNow()));
    QHBoxLayout *H_upd = new QHBoxLayout();
    H_upd->setContentsMargins(0,0,0,0);
    H_upd->addWidget(cb_auto);
    H_upd->addStretch(1);
    H_upd->addWidget(pb_upd);

    QLabel *l_links = new QLabel(tr("Sources")+": "
                                 "<a href=\"https://sourceforge.net/projects/wsjt-x-improved/files/Additional%20Files/\">WSJT-X Improved</a>, "
                                 "<a href=\"https://www.bigskyspaces.com/w7gj/\">W7GJ</a>, "
                                 "<a href=\"https://sourceforge.net/projects/jtdx/files/\">JTDX</a>, "
                                 "<a href=\"https://g7vrd.co.uk/public-apis\">G7VRD</a>");
    l_links->setTextFormat(Qt::RichText);
    l_links->setTextInteractionFlags(Qt::TextBrowserInteraction);
    l_links->setOpenExternalLinks(true);
    l_links->setWordWrap(true);

    QVBoxLayout *V_gb = new QVBoxLayout();
    V_gb->setContentsMargins(6,6,6,6);
    V_gb->setSpacing(8);
    V_gb->addWidget(l_info);
    V_gb->addLayout(G);
    V_gb->addLayout(H_upd);
    V_gb->addWidget(l_links);
    gb->setLayout(V_gb);

    QVBoxLayout *V_l = new QVBoxLayout(this);
    V_l->setContentsMargins(4,4,4,4);
    V_l->addWidget(gb);
    V_l->addStretch(1);
    setLayout(V_l);

    connect(s, SIGNAL(EmitChanged()), this, SLOT(Refresh()));
    Refresh();
}
void HvLocSrcW::Toggled()
{
    for (int i = 0; i < HvLocSrc::COUNT; ++i) s->SetOn(i,cb[i]->isChecked());
    s->SetAutoUpdate(cb_auto->isChecked());
    Refresh();
}
void HvLocSrcW::UpdateNow()
{
    s->UpdateNow();
    Refresh();
}
void HvLocSrcW::Refresh()
{
    for (int i = 0; i < HvLocSrc::COUNT; ++i)
    {
        l_st[i]->setText(s->Status(i));
        l_st[i]->setEnabled(s->IsOn(i));
    }
    pb_upd->setEnabled(!s->Busy());
}
