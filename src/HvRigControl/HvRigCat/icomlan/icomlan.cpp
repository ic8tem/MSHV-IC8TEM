/* MSHV-IC8TEM  Icom LAN (RS-BA1 network protocol) rig control and audio
 * See icomlan.h for the description and the credits.
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include "icomlan.h"
#include "../icom/icom.h"
#include "../icom/icom_def.h"
#define _NETWORK_RIGS_
#include "../network/network_def.h"
#include <QCoreApplication>
#include <QHostInfo>
#include <QAtomicInt>
#include <QMetaObject>
#include <QRandomGenerator>
#include <math.h>

extern void _SetRxAudioTci_(int *raw2,int cr2,int k_res);
extern int  _GetRxAudioReadTci_();

// ---------------------------------------------------------------- wire
#define IL_T_IDLE       0
#define IL_T_RETRANSMIT 1
#define IL_T_THERE      3
#define IL_T_HERE       4
#define IL_T_DISCONNECT 5
#define IL_T_READY      6
#define IL_T_PING       7
#define IL_TIMEOUT_MS   10000
#define IL_TXBUFFER_MS  150
#define IL_FRAME_BYTES  1920       // 20 ms, 48 kHz mono S16LE
#define IL_MAX_ATTEMPTS 3

// Credential obfuscation table of the Icom network protocol (characters 0x20..0x7e).
static const unsigned char il_passcode_tab[95] =
{
    0x47,0x5d,0x4c,0x42,0x66,0x20,0x23,0x46,0x4e,0x57,0x45,0x3d,0x67,0x76,0x60,0x41,
    0x62,0x39,0x59,0x2d,0x68,0x7e,0x7c,0x65,0x7d,0x49,0x29,0x72,0x73,0x78,0x21,0x6e,
    0x5a,0x5e,0x4a,0x3e,0x71,0x2c,0x2a,0x54,0x3c,0x3a,0x63,0x4f,0x43,0x75,0x27,0x79,
    0x5b,0x35,0x70,0x48,0x6b,0x56,0x6f,0x34,0x32,0x6c,0x30,0x61,0x6d,0x7b,0x2f,0x4b,
    0x64,0x38,0x2b,0x2e,0x50,0x40,0x3f,0x55,0x33,0x37,0x25,0x77,0x24,0x26,0x74,0x6a,
    0x28,0x53,0x4d,0x69,0x22,0x5c,0x44,0x31,0x36,0x58,0x3b,0x7a,0x51,0x5f,0x52
};
static bool IlPasscodeValid(const QString &s)
{
    if (s.isEmpty() || s.count() > 16) return false;
    for (int i = 0; i < s.count(); ++i)
    {
        ushort c = s.at(i).unicode();
        if (c < 0x20 || c > 0x7e) return false;
    }
    return true;
}
static QByteArray IlPasscode(const QString &s)
{
    QByteArray out(16,0);
    for (int i = 0; i < s.count() && i < 16; ++i)
    {
        int c = (int)s.at(i).unicode() + i;
        if (c > 126) c = 32 + c % 127;
        out[i] = (char)il_passcode_tab[c - 32];
    }
    return out;
}
static inline void PutLE16(QByteArray &p,int o,quint16 v)
{
    p[o] = (char)(v & 0xff);
    p[o+1] = (char)((v >> 8) & 0xff);
}
static inline void PutLE32(QByteArray &p,int o,quint32 v)
{
    for (int i = 0; i < 4; ++i) p[o+i] = (char)((v >> (8*i)) & 0xff);
}
static inline void PutBE16(QByteArray &p,int o,quint16 v)
{
    p[o] = (char)((v >> 8) & 0xff);
    p[o+1] = (char)(v & 0xff);
}
static inline void PutBE32(QByteArray &p,int o,quint32 v)
{
    for (int i = 0; i < 4; ++i) p[o+i] = (char)((v >> (8*(3-i))) & 0xff);
}
static inline quint8  B(const QByteArray &p,int o)
{
    return (quint8)p.at(o);
}
static inline quint16 LE16(const QByteArray &p,int o)
{
    return (quint16)(B(p,o) | (B(p,o+1) << 8));
}
static inline quint32 LE32(const QByteArray &p,int o)
{
    return (quint32)B(p,o) | ((quint32)B(p,o+1) << 8) | ((quint32)B(p,o+2) << 16) | ((quint32)B(p,o+3) << 24);
}
static inline quint16 BE16(const QByteArray &p,int o)
{
    return (quint16)((B(p,o) << 8) | B(p,o+1));
}
static inline quint32 BE32(const QByteArray &p,int o)
{
    return ((quint32)B(p,o) << 24) | ((quint32)B(p,o+1) << 16) | ((quint32)B(p,o+2) << 8) | (quint32)B(p,o+3);
}
static QByteArray IlPacket(int len,quint16 type,quint32 local,quint32 remote,quint16 seq)
{
    QByteArray p(len,0);
    PutLE32(p,0,(quint32)len);
    PutLE16(p,4,type);
    PutLE16(p,6,seq);
    PutBE32(p,8,local);
    PutBE32(p,12,remote);
    return p;
}
static inline int IlDistance(quint16 a,quint16 b)
{
    return (int)((quint16)(a - b));
}
static inline bool IlAfter(quint16 a,quint16 b)
{
    int d = IlDistance(a,b);
    return d > 0 && d < 0x8000;
}
static inline double IlHermite(const double *x,double t)
{
    double c0 = x[1];
    double c1 = 0.5*(x[2]-x[0]);
    double c2 = x[0] - 2.5*x[1] + 2.0*x[2] - 0.5*x[3];
    double c3 = 0.5*(x[3]-x[0]) + 1.5*(x[1]-x[2]);
    return ((c3*t + c2)*t + c1)*t + c0;
}

// ---------------------------------------------------------------- hooks
#define IL_RING (48000*4)
static qint16     il_ring[IL_RING];
static QAtomicInt il_w(0);
static QAtomicInt il_r(0);
static QAtomicInt il_tx_rate(48000);
static QAtomicInt il_tx_dev(0);
static QAtomicInt il_tx_keyed(0);
static QAtomicInt il_alive(0);
static QAtomicInt il_rx44k(0);
static inline int IlRingUsed()
{
    int u = il_w.loadAcquire() - il_r.loadAcquire();
    if (u < 0) u += IL_RING;
    return u;
}
void _IcomLanSetTxDevice_(bool on)
{
    il_tx_dev.storeRelease(on ? 1 : 0);
}
bool _IcomLanTxActive_()
{
    return il_tx_dev.loadAcquire() != 0;
}
void _SetTxAudioRateTci_(int rate)
{
    if (rate >= 8000 && rate <= 192000) il_tx_rate.storeRelease(rate);
}
void _IcomLanSetRx44k_(bool on)
{
    il_rx44k.storeRelease(on ? 1 : 0);
}
bool _SetTxAudioIcomLan_(int *raw)
{
    // Rawplayer thread: 4096 ints = 2048 stereo frames, 24-bit scale.
    int rate = il_tx_rate.loadAcquire();
    if (rate < 8000) rate = 48000;
    if (!il_alive.loadAcquire() || !il_tx_keyed.loadAcquire())
    {
        // Nobody to send to: keep the producer in real time.
        QThread::usleep((unsigned long)((2048LL*1000000LL)/rate));
        return true;
    }
    int guard = 900;                                // ~4.5 s ceiling
    while (guard-- > 0)
    {
        if (IlRingUsed() < rate - 2048) break;      // keep at most ~1 s queued
        if (!il_tx_keyed.loadAcquire()) return true;
        QThread::usleep(5000);
    }
    int wp = il_w.loadAcquire();
    for (int i = 0; i < 2048; ++i)
    {
        int s = raw[2*i] / 256;
        if (s > 32767) s = 32767;
        if (s < -32768) s = -32768;
        il_ring[wp] = (qint16)s;
        if (++wp >= IL_RING) wp = 0;
    }
    il_w.storeRelease(wp);
    return true;
}

// ---------------------------------------------------------------- stream
IcomLanStream::IcomLanStream()
{
    sock = 0;
    role = 0;
    remote_port = 0;
    local_id = 0;
    remote_id = 0;
    tx_seq = 1;
    ping_seq = 0;
    started = false;
    ready = false;
    have_last = false;
    last_delivered = 0;
    gap_since = -1;
    last_rx = last_handshake = last_ping = last_idle = 0;
}

// ---------------------------------------------------------------- core
IcomLanCore::IcomLanCore()
        : QObject(0)
{
    want = false;
    running = false;
    connected = false;
    attempts = 0;
    s_port = 50001;
    s_civ_port = 50002;
    s_audio_port = 50003;
    tick = new QTimer(this);
    txtick = new QTimer(this);
    txtick->setTimerType(Qt::PreciseTimer);
    retry = new QTimer(this);
    retry->setSingleShot(true);
    connect(tick, SIGNAL(timeout()), this, SLOT(Tick()));
    connect(txtick, SIGNAL(timeout()), this, SLOT(TxTick()));
    connect(retry, SIGNAL(timeout()), this, SLOT(Retry()));
    clock.start();
    for (int i = 0; i < 3; ++i) st[i].role = i;
    tx_keyed = false;
    tx_primed = false;
    tx_next = 0;
    tx_frac = 0.0;
    tx_lead = 0;
    rx_frac = 0.0;
    for (int i = 0; i < 4; ++i)
    {
        tx_x[i] = 0.0;
        rx_x[i] = 0.0;
    }
    radio_civ_addr = 0;
}
IcomLanCore::~IcomLanCore()
{
    il_alive.storeRelease(0);
    il_tx_keyed.storeRelease(0);
}
qint64 IcomLanCore::Now()
{
    return clock.elapsed();
}
void IcomLanCore::Start(QString host,int port,QString user,QString pass,int civ_port,int audio_port)
{
    s_host = host.trimmed();
    s_port = (port > 0 && port < 65534) ? port : 50001;
    s_civ_port = (civ_port > 0 && civ_port < 65536) ? civ_port : s_port + 1;      // empty = control + 1
    s_audio_port = (audio_port > 0 && audio_port < 65536) ? audio_port : s_port + 2;// empty = control + 2
    s_user = user;
    s_pass = pass;
    if (!IlPasscodeValid(s_user) || !IlPasscodeValid(s_pass))
    {
        want = false;
        emit Status(tr("Icom LAN: user and password must be 1-16 ASCII characters"),0);
        return;
    }
    want = true;
    attempts = 0;
    Begin();
}
void IcomLanCore::Stop()
{
    want = false;
    retry->stop();
    if (running) CloseAll(true);
    emit Status(tr("Disconnected"),0);
}
void IcomLanCore::Retry()
{
    if (want) Begin();
}
bool IcomLanCore::BindStream(int i,quint16 rport)
{
    IcomLanStream &s = st[i];
    s = IcomLanStream();
    s.role = i;
    s.remote_port = rport;
    s.local_id = QRandomGenerator::global()->generate();
    if (!s.local_id) s.local_id = 0x49433854;
    s.sock = new QUdpSocket(this);
    if (!s.sock->bind(QHostAddress(QHostAddress::AnyIPv4), 0)) return false;
    if (i==0) connect(s.sock, SIGNAL(readyRead()), this, SLOT(ReadCtrl()));
    else if (i==1) connect(s.sock, SIGNAL(readyRead()), this, SLOT(ReadCiv()));
    else connect(s.sock, SIGNAL(readyRead()), this, SLOT(ReadAudio()));
    return true;
}
void IcomLanCore::Begin()
{
    CloseAll(false);
    addr = QHostAddress();
    if (!addr.setAddress(s_host))
    {
        QHostInfo hi = QHostInfo::fromName(s_host);
        QList<QHostAddress> l = hi.addresses();
        for (int i = 0; i < l.count(); ++i)
        {
            if (l.at(i).protocol()==QAbstractSocket::IPv4Protocol)
            {
                addr = l.at(i);
                break;
            }
        }
        if (addr.isNull() && !l.isEmpty()) addr = l.at(0);
    }
    if (addr.isNull())
    {
        want = false;
        emit Status(tr("Icom LAN: unknown address")+" "+s_host,0);
        return;
    }
    inner_seq = 0;
    serial_seq = 0;
    audio_seq = 0;
    request_id = (quint16)((QRandomGenerator::global()->generate() & 0xffff) | 1);
    auth_id.clear();
    radio_id.clear();
    radio_name.clear();
    radio_civ_addr = 0;
    authenticated = requested = granted = civ_seen = false;
    pending_renew.clear();
    last_auth = renew_due = last_renew_sent = last_serial_open = 0;
    connected = false;
    tx_keyed = false;
    il_tx_keyed.storeRelease(0);
    rx_frac = 0.0;
    for (int i = 0; i < 4; ++i) rx_x[i] = 0.0;
    bool ok = BindStream(0,(quint16)s_port) && BindStream(1,(quint16)s_civ_port) && BindStream(2,(quint16)s_audio_port);
    if (!ok)
    {
        CloseAll(false);
        want = false;
        emit Status(tr("Icom LAN: cannot open UDP sockets"),0);
        return;
    }
    running = true;
    started_at = Now();
    StartStream(0);
    tick->start(20);
    txtick->start(5);
    emit Status(tr("Connecting to")+" "+s_host+" "+tr("Ports")+" "+QString("%1/%2/%3").arg(s_port).arg(s_civ_port).arg(s_audio_port)+
                (attempts>0 ? "  "+tr("Try")+": "+QString("%1").arg(attempts+1) : QString()),1);
}
void IcomLanCore::CloseAll(bool polite)
{
    il_alive.storeRelease(0);
    il_tx_keyed.storeRelease(0);
    tx_keyed = false;
    if (polite)
    {
        if (st[1].ready && st[1].sock)
        {
            QByteArray p = IlPacket(22,IL_T_IDLE,st[1].local_id,st[1].remote_id,0);
            p[16] = (char)0xc0;
            p[17] = 1;
            PutBE16(p,19,serial_seq++);
            p[21] = 0;
            Tracked(1,p);
        }
        if (st[0].ready && st[0].sock && !auth_id.isEmpty()) SendAuth(1);
        for (int i = 0; i < 3; ++i)
        {
            if (st[i].sock && st[i].remote_id)
                Raw(i,IlPacket(16,IL_T_DISCONNECT,st[i].local_id,st[i].remote_id,0));
        }
    }
    tick->stop();
    txtick->stop();
    for (int i = 0; i < 3; ++i)
    {
        if (st[i].sock)
        {
            st[i].sock->disconnect(this);
            st[i].sock->close();
            st[i].sock->deleteLater();
        }
        st[i] = IcomLanStream();
        st[i].role = i;
    }
    running = false;
    connected = false;
}
void IcomLanCore::Fail(QString why)
{
    if (!running) return;
    bool was_connected = connected;
    bool retry_ok = !why.startsWith("!");
    if (!retry_ok) why = why.mid(1);
    CloseAll(true);
    int max = was_connected ? IL_MAX_ATTEMPTS + 2 : IL_MAX_ATTEMPTS;
    if (was_connected) attempts = 0;
    if (want && retry_ok && attempts < max - 1)
    {
        attempts++;
        emit Status("Icom LAN: "+why+" - "+tr("retrying"),1);
        retry->start(2000);
    }
    else
    {
        want = false;
        emit Status("Icom LAN: "+why,0);
    }
}
void IcomLanCore::StartStream(int i)
{
    IcomLanStream &s = st[i];
    qint64 t = Now();
    s.started = true;
    s.last_rx = s.last_handshake = s.last_ping = s.last_idle = t;
    Handshake(i);
}
void IcomLanCore::Handshake(int i)
{
    IcomLanStream &s = st[i];
    QByteArray p = IlPacket(16, s.remote_id ? IL_T_READY : IL_T_THERE, s.local_id, s.remote_id, s.remote_id ? 1 : 0);
    Raw(i,p);
    Raw(i,p);
}
void IcomLanCore::Raw(int i,const QByteArray &p)
{
    if (!st[i].sock) return;
    st[i].sock->writeDatagram(p, addr, st[i].remote_port);
}
void IcomLanCore::Tracked(int i,QByteArray p)
{
    IcomLanStream &s = st[i];
    quint16 seq = s.tx_seq;
    s.tx_seq = (quint16)(seq + 1);
    PutLE16(p,6,seq);
    if (s.replay.contains(seq)) s.replay_order.removeAll(seq);
    s.replay.insert(seq,p);
    s.replay_order.append(seq);
    while (s.replay_order.count() > 256) s.replay.remove(s.replay_order.takeFirst());
    Raw(i,p);
}
void IcomLanCore::ReadCtrl()
{
    if (!st[0].sock) return;
    QUdpSocket *sk = st[0].sock;
    while (sk->hasPendingDatagrams())
    {
        QByteArray d;
        d.resize((int)sk->pendingDatagramSize());
        QHostAddress from;
        quint16 fport = 0;
        sk->readDatagram(d.data(), d.size(), &from, &fport);
        if (!from.isEqual(addr, QHostAddress::TolerantConversion)) continue;
        Receive(0,d);
        if (!st[0].sock) return;
    }
}
void IcomLanCore::ReadCiv()
{
    if (!st[1].sock) return;
    QUdpSocket *sk = st[1].sock;
    while (sk->hasPendingDatagrams())
    {
        QByteArray d;
        d.resize((int)sk->pendingDatagramSize());
        QHostAddress from;
        quint16 fport = 0;
        sk->readDatagram(d.data(), d.size(), &from, &fport);
        if (!from.isEqual(addr, QHostAddress::TolerantConversion)) continue;
        Receive(1,d);
        if (!st[1].sock) return;
    }
}
void IcomLanCore::ReadAudio()
{
    if (!st[2].sock) return;
    QUdpSocket *sk = st[2].sock;
    while (sk->hasPendingDatagrams())
    {
        QByteArray d;
        d.resize((int)sk->pendingDatagramSize());
        QHostAddress from;
        quint16 fport = 0;
        sk->readDatagram(d.data(), d.size(), &from, &fport);
        if (!from.isEqual(addr, QHostAddress::TolerantConversion)) continue;
        Receive(2,d);
        if (!st[2].sock) return;
    }
}
void IcomLanCore::Receive(int i,const QByteArray &p)
{
    IcomLanStream &s = st[i];
    if (!s.started || p.size() < 16 || p.size() > 4096) return;
    const quint32 len = LE32(p,0);
    const quint16 type = LE16(p,4);
    const quint16 seq = LE16(p,6);
    const quint32 sender = BE32(p,8);
    const quint32 receiver = BE32(p,12);
    if (len != (quint32)p.size() &&
            !(type==IL_T_PING && p.size()==21 && len==0) &&
            !(type==IL_T_RETRANSMIT && len==24 && p.size()>=24 && (p.size()-16)%4==0)) return;
    if (receiver != s.local_id || (s.remote_id && sender != s.remote_id)) return;
    if (!s.remote_id)
    {
        if (type!=IL_T_HERE || p.size()!=16 || !sender) return;
        s.remote_id = sender;
        s.last_rx = Now();
        Handshake(i);
        return;
    }
    s.last_rx = Now();
    if (!s.ready)
    {
        if (type==IL_T_READY && p.size()==16)
        {
            s.ready = true;
            if (i==0)
            {
                QByteArray l = IlPacket(128,IL_T_IDLE,s.local_id,s.remote_id,0);
                PutBE32(l,16,128-16);
                l[20] = 1;
                l[21] = 0;
                PutBE16(l,22,NextInner());
                PutLE16(l,26,request_id);
                l.replace(64,16,IlPasscode(s_user));
                l.replace(80,16,IlPasscode(s_pass));
                l.replace(96,7,QByteArray("icom-pc"));
                Tracked(0,l);
                emit Status(tr("Connecting to")+" "+s_host+" - "+tr("login"),1);
            }
            else if (i==1)
            {
                OpenSerial();
                MediaReady();
            }
            else MediaReady();
        }
        return;
    }
    if (type==IL_T_PING && p.size()==21)
    {
        if (B(p,16)==0)
        {
            QByteArray r = IlPacket(21,IL_T_PING,s.local_id,s.remote_id,seq);
            r[16] = 1;
            for (int k = 17; k < 21; ++k) r[k] = p.at(k);
            Raw(i,r);
        }
        return;
    }
    if (type==IL_T_DISCONNECT)
    {
        Fail(tr("stream closed by the radio"));
        return;
    }
    if (type==IL_T_RETRANSMIT)
    {
        QList<QPair<quint16,quint16> > ranges;
        if (p.size()==16) ranges.append(qMakePair(seq,seq));
        for (int at = 16; at + 4 <= p.size(); at += 4)
        {
            quint16 a = LE16(p,at), b = LE16(p,at+2);
            if (a || b) ranges.append(qMakePair(a,b));
        }
        int budget = 10;
        for (int r = 0; r < ranges.count() && budget > 0; ++r)
        {
            quint16 a = ranges.at(r).first, b = ranges.at(r).second;
            int cnt = IlDistance(b,a);
            for (int n = 0; n <= cnt && budget-- > 0; ++n)
            {
                quint16 q = (quint16)(a + n);
                if (s.replay.contains(q)) Raw(i,s.replay.value(q));
                else Raw(i,IlPacket(16,IL_T_IDLE,s.local_id,s.remote_id,q));
            }
        }
        return;
    }
    if (type!=IL_T_IDLE) return;
    if (i==0)
    {
        if (p.size() > 16) ControlPayload(p);
        return;
    }
    if (!s.have_last)
    {
        s.have_last = true;
        s.last_delivered = seq;
        Deliver(i,p);
    }
    else if (IlAfter(seq,s.last_delivered))
    {
        if (!s.reorder.contains(seq)) s.reorder.insert(seq,p);
        Drain(i);
        if (!s.reorder.isEmpty() && s.gap_since < 0) s.gap_since = Now();
        if (s.reorder.count() > 256) SkipGap(i);
    }
}
void IcomLanCore::Deliver(int i,const QByteArray &p)
{
    if (p.size() <= 16) return;
    if (i==1)
    {
        if (p.size() > 21 && B(p,16)==0xc1 && LE16(p,17)==p.size()-21)
        {
            civ_seen = true;
            emit CivReceived(p.mid(21));
        }
    }
    else if (i==2) AudioPayload(p);
}
void IcomLanCore::Drain(int i)
{
    IcomLanStream &s = st[i];
    quint16 next = (quint16)(s.last_delivered + 1);
    while (s.reorder.contains(next))
    {
        QByteArray p = s.reorder.take(next);
        s.last_delivered = next;
        Deliver(i,p);
        if (!st[i].sock) return;
        next = (quint16)(next + 1);
    }
    if (s.reorder.isEmpty()) s.gap_since = -1;
}
void IcomLanCore::SkipGap(int i)
{
    IcomLanStream &s = st[i];
    if (s.reorder.isEmpty()) return;
    int best = 0x10000;
    quint16 bseq = 0;
    for (QMap<quint16,QByteArray>::const_iterator it = s.reorder.constBegin(); it != s.reorder.constEnd(); ++it)
    {
        int d = IlDistance(it.key(),s.last_delivered);
        if (d < best)
        {
            best = d;
            bseq = it.key();
        }
    }
    s.last_delivered = (quint16)(bseq - 1);
    s.gap_since = -1;
    Drain(i);
    if (!s.reorder.isEmpty()) s.gap_since = Now();
}
quint16 IcomLanCore::NextInner()
{
    quint16 v = inner_seq;
    inner_seq = (quint16)(inner_seq + 1);
    return v;
}
quint16 IcomLanCore::SendAuth(int kind)
{
    IcomLanStream &s = st[0];
    quint16 seq = NextInner();
    QByteArray p = IlPacket(64,IL_T_IDLE,s.local_id,s.remote_id,0);
    PutBE32(p,16,64-16);
    p[20] = 1;
    p[21] = (char)kind;
    PutBE16(p,22,seq);                // big-endian: MK2 token renewal breaks otherwise
    p.replace(26,6,auth_id.left(6));
    Tracked(0,p);
    return seq;
}
void IcomLanCore::Renew()
{
    quint16 seq = SendAuth(5);
    pending_renew.insert(seq,Now());
    while (pending_renew.count() > 4) pending_renew.erase(pending_renew.begin());
    last_renew_sent = Now();
}
void IcomLanCore::ControlPayload(const QByteArray &p)
{
    const int n = p.size();
    const qint64 now = Now();
    if (n==96 && auth_id.isEmpty())
    {
        if (LE16(p,26) != request_id) return;
        if (BE32(p,48) != 0)
        {
            Fail("!"+tr("the radio rejected user or password"));
            return;
        }
        auth_id = p.mid(26,6);
        SendAuth(2);
        Renew();
    }
    else if (n==64 && B(p,20)==2 && B(p,21)==5)
    {
        quint16 seq = BE16(p,22);
        if (!pending_renew.contains(seq)) return;
        quint32 resp = LE32(p,48);
        bool initial = !authenticated && !granted;
        bool same = (p.mid(26,6)==auth_id);
        if (!same && !(initial && resp==0xffffffff)) return;
        if (resp && !(initial && resp==0xffffffff))
        {
            Fail(tr("token renewal rejected"));
            return;
        }
        if (!same) auth_id = p.mid(26,6);
        pending_renew.clear();
        authenticated = true;
        last_auth = now;
        renew_due = now + (initial ? 30000 : 60000);
        RequestStreams();
    }
    else if (n>=168 && (n-66)%102==0 && radio_id.isEmpty())
    {
        radio_id = p.mid(66,16);
        radio_name = p.mid(82,32);
        int z = radio_name.indexOf('\0');
        if (z >= 0) radio_name = radio_name.left(z);
        radio_civ_addr = B(p,148);
        RequestStreams();
    }
    else if (n==144)
    {
        if (!granted && requested && authenticated && B(p,20)==3 && B(p,21)==0 &&
                !radio_id.isEmpty() && p.mid(32,16)==radio_id && B(p,96)==1)
        {
            granted = true;
            auth_id = p.mid(26,6);
            emit Status(tr("Connecting to")+" "+s_host+" - "+tr("opening CI-V and audio"),1);
            StartStream(1);
            StartStream(2);
        }
        else if (connected && B(p,20)==3 && B(p,21)==0 && LE32(p,96)==0 && B(p,100)==0 &&
                 LE32(p,132)==0 && p.mid(26,6)==auth_id && p.mid(32,16)==radio_id)
        {
            Fail(tr("the radio released the streams (another client?)"));
        }
    }
    else if (n==80)
    {
        if (B(p,48)==0xff && B(p,49)==0xff && B(p,50)==0xff)
        {
            if (!granted) Fail("!"+tr("authentication failed"));
        }
        else if (B(p,48)==0 && B(p,49)==0 && B(p,50)==0 && B(p,64)==1)
        {
            Fail(tr("the radio ended the session"));
        }
    }
}
void IcomLanCore::RequestStreams()
{
    if (requested || !authenticated || radio_id.isEmpty()) return;
    requested = true;
    IcomLanStream &s = st[0];
    QByteArray p = IlPacket(144,IL_T_IDLE,s.local_id,s.remote_id,0);
    PutBE32(p,16,144-16);
    p[20] = 1;
    p[21] = 3;
    PutBE16(p,22,NextInner());
    p.replace(26,6,auth_id.left(6));
    p.replace(32,16,radio_id.left(16));
    QByteArray nm = radio_name.left(32);
    p.replace(64,nm.size(),nm);
    p.replace(96,16,IlPasscode(s_user));
    p[112] = 1;                         // RX audio
    p[113] = 1;                         // TX audio
    p[114] = 4;                         // RX codec: LPCM 16 bit mono
    p[115] = 4;                         // TX codec
    PutBE32(p,116,48000);
    PutBE32(p,120,48000);
    PutBE32(p,124,st[1].sock ? st[1].sock->localPort() : 0);
    PutBE32(p,128,st[2].sock ? st[2].sock->localPort() : 0);
    PutBE32(p,132,IL_TXBUFFER_MS);
    p[136] = 1;
    Tracked(0,p);
    emit Status(tr("Connecting to")+" "+s_host+" - "+tr("radio")+" "+QString::fromLatin1(radio_name),1);
}
void IcomLanCore::OpenSerial()
{
    IcomLanStream &s = st[1];
    if (!s.sock) return;
    last_serial_open = Now();
    QByteArray p = IlPacket(22,IL_T_IDLE,s.local_id,s.remote_id,0);
    p[16] = (char)0xc0;
    p[17] = 1;
    PutBE16(p,19,serial_seq++);
    p[21] = 5;
    Tracked(1,p);
}
void IcomLanCore::MediaReady()
{
    if (connected || !st[1].ready || !st[2].ready) return;
    connected = true;
    attempts = 0;
    il_alive.storeRelease(1);
    QString nm = QString::fromLatin1(radio_name);
    emit RadioInfo(nm,radio_civ_addr);
    emit Status(tr("Connected to")+" "+s_host+(nm.isEmpty() ? QString() : " ("+nm+")"),2);
}
void IcomLanCore::SendCiv(QByteArray civ)
{
    if (!connected || !st[1].ready || civ.isEmpty()) return;
    IcomLanStream &s = st[1];
    QByteArray p = IlPacket(21 + civ.size(),IL_T_IDLE,s.local_id,s.remote_id,0);
    p[16] = (char)0xc1;
    PutLE16(p,17,(quint16)civ.size());
    PutBE16(p,19,serial_seq++);
    p.replace(21,civ.size(),civ);
    Tracked(1,p);
}
void IcomLanCore::SetTxKeyed(bool on)
{
    if (on)
    {
        if (tx_keyed) return;
        il_r.storeRelease(il_w.loadAcquire());   // drop anything older than this key
        tx_primed = false;
        tx_frac = 0.0;
        for (int i = 0; i < 4; ++i) tx_x[i] = 0.0;
        tx_keyed = true;
        il_tx_keyed.storeRelease(1);
    }
    else
    {
        tx_keyed = false;
        il_tx_keyed.storeRelease(0);
    }
}
void IcomLanCore::SendAudioFrame(const QByteArray &pcm)
{
    IcomLanStream &s = st[2];
    if (!s.ready) return;
    int at = 0;
    while (at < pcm.size())
    {
        int n = pcm.size() - at;
        if (n > 1364) n = 1364;               // wire split 1364 + 556 per 20 ms
        QByteArray p = IlPacket(24 + n,IL_T_IDLE,s.local_id,s.remote_id,0);
        PutBE16(p,16,0x8000);
        PutBE16(p,18,audio_seq++);
        PutBE16(p,22,(quint16)n);
        p.replace(24,n,pcm.mid(at,n));
        Tracked(2,p);
        at += n;
    }
}
void IcomLanCore::TxTick()
{
    if (!connected || !tx_keyed) return;
    const qint64 now = Now();
    int rate = il_tx_rate.loadAcquire();
    if (rate < 8000) rate = 48000;
    const double step = (double)rate / 48000.0;
    const int need = (int)ceil(960.0 * step) + 3;
    if (!tx_primed)
    {
        if (IlRingUsed() < 4096) return;      // ~85 ms of audio before starting
        tx_primed = true;
        tx_next = now;
    }
    for (int n = 0; n < 2 && now >= tx_next; ++n)
    {
        if (IlRingUsed() < need) break;
        QByteArray pcm(IL_FRAME_BYTES,0);
        int rp = il_r.loadAcquire();
        for (int k = 0; k < 960; ++k)
        {
            double y = IlHermite(tx_x,tx_frac);
            int v = (int)lrint(y);
            if (v > 32767) v = 32767;
            if (v < -32768) v = -32768;
            PutLE16(pcm,2*k,(quint16)(qint16)v);
            tx_frac += step;
            while (tx_frac >= 1.0)
            {
                tx_x[0] = tx_x[1];
                tx_x[1] = tx_x[2];
                tx_x[2] = tx_x[3];
                tx_x[3] = (double)il_ring[rp];
                if (++rp >= IL_RING) rp = 0;
                tx_frac -= 1.0;
            }
        }
        il_r.storeRelease(rp);
        SendAudioFrame(pcm);
        tx_next += 20;
    }
    if (now - tx_next > 40) tx_next = now + 20;
}
void IcomLanCore::AudioPayload(const QByteArray &p)
{
    const int n = p.size();
    if (n <= 24 || B(p,16)==0xc0 || B(p,16)==0xc1 || BE16(p,22)!=n-24 || ((n-24)%2)!=0) return;
    if (_GetRxAudioReadTci_()==0) return;            // decoder not reading yet
    const int ns = (n-24)/2;
    const bool r44 = il_rx44k.loadAcquire()!=0;
    const double step = 48000.0/44100.0;
    int cnt = 0;
    for (int i = 0; i < ns && cnt < 16000; ++i)
    {
        const int s = (int)(qint16)LE16(p,24+2*i);
        if (!r44)
        {
            rx_buf[cnt++] = s * 256;                  // 24-bit scale like TCI/Flex
            continue;
        }
        rx_x[0] = rx_x[1];
        rx_x[1] = rx_x[2];
        rx_x[2] = rx_x[3];
        rx_x[3] = (double)s;
        while (rx_frac < 1.0 && cnt < 16000)
        {
            rx_buf[cnt++] = (int)lrint(IlHermite(rx_x,rx_frac) * 256.0);
            rx_frac += step;
        }
        rx_frac -= 1.0;
    }
    if (cnt > 0) _SetRxAudioTci_(rx_buf,cnt,3);
}
void IcomLanCore::Tick()
{
    if (!running) return;
    const qint64 now = Now();
    if (!connected && now - started_at > IL_TIMEOUT_MS + 5000)
    {
        if (!st[0].ready) Fail(tr("no answer from the radio (address, port, Network Control ON?)"));
        else if (!granted) Fail(tr("no stream grant (radio busy with another client?)"));
        else Fail(tr("CI-V/audio streams did not open"));
        return;
    }
    for (int i = 0; i < 3; ++i)
    {
        IcomLanStream &s = st[i];
        if (!s.started || !s.sock) continue;
        if (now - s.last_rx >= IL_TIMEOUT_MS)
        {
            if (!st[0].ready) Fail(tr("no answer from the radio (address, port, Network Control ON?)"));
            else Fail(tr("the radio stopped answering"));
            return;
        }
        if (!s.ready)
        {
            if (now - s.last_handshake >= 500)
            {
                s.last_handshake = now;
                Handshake(i);
            }
            continue;
        }
        if (now - s.last_ping >= 500)
        {
            s.last_ping = now;
            QByteArray p = IlPacket(21,IL_T_PING,s.local_id,s.remote_id,s.ping_seq++);
            p[16] = 0;
            PutLE32(p,17,(quint32)now);
            Raw(i,p);
        }
        if (i!=2 && now - s.last_idle >= 100)
        {
            s.last_idle = now;
            Tracked(i,IlPacket(16,IL_T_IDLE,s.local_id,s.remote_id,0));
        }
        if (s.gap_since >= 0 && now - s.gap_since >= 80)
        {
            if (now - s.gap_since >= 160) SkipGap(i);
            else
            {
                int best = 0x10000;
                for (QMap<quint16,QByteArray>::const_iterator it = s.reorder.constBegin(); it != s.reorder.constEnd(); ++it)
                {
                    int d = IlDistance(it.key(),s.last_delivered);
                    if (d < best) best = d;
                }
                int cnt = qMin(10,best-1);
                for (int k = 1; k <= cnt; ++k)
                    Raw(i,IlPacket(16,IL_T_RETRANSMIT,s.local_id,s.remote_id,(quint16)(s.last_delivered + k)));
            }
        }
    }
    if (authenticated && now - last_auth >= 80000)
    {
        Fail(tr("session token expired"));
        return;
    }
    if (!auth_id.isEmpty())
    {
        if (!pending_renew.isEmpty() ? (now - last_renew_sent >= 3000) : (authenticated && now >= renew_due)) Renew();
    }
    if (st[1].ready && !civ_seen && now - last_serial_open >= 500) OpenSerial();
}

// ---------------------------------------------------------------- rig (GUI side)
IcomLanRig::IcomLanRig(int ModelID,QWidget *parent)
        : QWidget(parent)
{
    s_ModelID = ModelID;
    running = false;
    quit_done = false;
    // One entry for every Icom radio with LAN: the CI-V commands follow the
    // CI-V address the radio announces at login (IC-7300MK2 until then).
    icom = 0;
    s_icom_model = -1;
    int im = Icom::ModelFromCivAddr(0xB6,"IC-7300MK2");
    MakeIcom(im < 0 ? 0 : im);

    core = new IcomLanCore();
    core->moveToThread(&thr);
    connect(this, SIGNAL(CoreStart(QString,int,QString,QString,int,int)), core, SLOT(Start(QString,int,QString,QString,int,int)));
    connect(this, SIGNAL(CoreStop()), core, SLOT(Stop()));
    connect(this, SIGNAL(CoreSendCiv(QByteArray)), core, SLOT(SendCiv(QByteArray)));
    connect(this, SIGNAL(CoreKey(bool)), core, SLOT(SetTxKeyed(bool)));
    connect(core, SIGNAL(CivReceived(QByteArray)), this, SLOT(CivFromCore(QByteArray)));
    connect(core, SIGNAL(Status(QString,int)), this, SLOT(CoreStatus(QString,int)));
    connect(core, SIGNAL(RadioInfo(QString,int)), this, SLOT(CoreRadioInfo(QString,int)));
    thr.start(QThread::TimeCriticalPriority);
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(Quit()));
}
IcomLanRig::~IcomLanRig()
{
    Quit();
    delete core;
    if (icom) delete icom;
}
void IcomLanRig::MakeIcom(int im)
{
    if (icom && im==s_icom_model) return;
    if (icom)
    {
        icom->disconnect();
        this->disconnect(icom);
        icom->deleteLater();
    }
    s_icom_model = im;
    icom = new Icom(im);
    connect(this, SIGNAL(IcomCmd(CmdID,ptt_t,QString)), icom, SLOT(SetCmd(CmdID,ptt_t,QString)));
    connect(icom, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(CivFromIcom(char*,int)), Qt::DirectConnection);
    connect(this, SIGNAL(IcomRead(QByteArray,int)), icom, SLOT(SetReadyRead(QByteArray,int)));
    connect(icom, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SIGNAL(EmitReadedInfo(CmdID,QString)));
}
void IcomLanRig::CoreRadioInfo(QString name,int addr)
{
    if (addr <= 0) return;
    int im = Icom::ModelFromCivAddr(addr,name);
    if (im >= 0) MakeIcom(im);
}
void IcomLanRig::Quit()
{
    if (quit_done) return;
    quit_done = true;
    if (thr.isRunning())
    {
        QMetaObject::invokeMethod(core, "Stop", Qt::BlockingQueuedConnection);
        thr.quit();
        thr.wait(2000);
    }
}
void IcomLanRig::ConnectNet(QString all)
{
    QStringList l = all.split("#");
    s_host = l.value(0).trimmed();
    s_port = l.value(1).trimmed();
    QString user = l.value(7);
    QString pass = l.value(8);
    int civ_port = l.value(9).trimmed().toInt();      // 0 = control port + 1
    int audio_port = l.value(10).trimmed().toInt();   // 0 = control port + 2
    if (running)
    {
        running = false;
        emit CoreKey(false);
        emit CoreStop();
        return;
    }
    running = true;
    emit EmitNetConnInfo("<font color='red'>"+tr("Connecting to")+" "+s_host+" "+tr("Port")+" "+s_port+"</font>",true,false);
    emit CoreStart(s_host,s_port.toInt(),user,pass,civ_port,audio_port);
}
void IcomLanRig::SetOnOffCatCommand(bool f,int model_id,int fact_id)
{
    if (model_id!=s_ModelID || fact_id!=NETWORK_ID) return;
    if (!f) emit CoreKey(false);
}
void IcomLanRig::SetTciSelect(int,int,bool)
{}
void IcomLanRig::SetCmd(CmdID i,ptt_t ptt,QString str)
{
    if (i==GET_SETT)
    {
        emit EmitRigSet(rigs_network[s_ModelID]);
        return;
    }
    if (i==SET_PTT)
    {
        bool on = (ptt!=RIG_PTT_OFF);
        if (on) emit CoreKey(true);
        emit IcomCmd(i, on ? RIG_PTT_ON : RIG_PTT_OFF, str);
        if (!on) emit CoreKey(false);
        return;
    }
    emit IcomCmd(i,ptt,str);
}
void IcomLanRig::CivFromIcom(char *data,int size)
{
    if (!running || size <= 0) return;
    emit CoreSendCiv(QByteArray(data,size));
}
void IcomLanRig::CivFromCore(QByteArray b)
{
    emit IcomRead(b,b.size());
}
void IcomLanRig::CoreStatus(QString s,int state)
{
    if (state==0)
    {
        running = false;
        emit EmitNetConnInfo("<font color='red'>"+s+"</font>",false,false);
    }
    else if (state==1) emit EmitNetConnInfo("<font color='red'>"+s+"</font>",true,false);
    else emit EmitNetConnInfo("<font color='#00b300'>"+s+"</font>",true,true);
}
