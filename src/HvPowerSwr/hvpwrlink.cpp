/* MSHV-IC8TEM  Power & SWR: own connection to the radio. See hvpwrlink.h.
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include "hvpwrlink.h"
#include "../HvRigControl/qexsp_1_2rc/qextserialport.h"

HvPwrLink::HvPwrLink(QObject *parent)
        : QObject(parent)
{
    s_kind = NONE;
    f_ok = false;
    s_port = 4532;
    s_baud = 115200;
    s_civ = 0x94;
    tcp = 0;
    ser = 0;
    f_busy = false;
    cur_tag = -1;
    no_po_meter = false;
    fails = 0;
    t_to = new QTimer(this);
    t_to->setSingleShot(true);
    connect(t_to, SIGNAL(timeout()), this, SLOT(Timeout()));
    t_retry = new QTimer(this);
    t_retry->setSingleShot(true);
    connect(t_retry, SIGNAL(timeout()), this, SLOT(Retry()));
}
HvPwrLink::~HvPwrLink()
{
    Close();
}
double HvPwrLink::PoFraction(int raw)   // Icom Po meter: 0=0%, 143=50%, 213=100%
{
    if (raw <= 0) return 0.0;
    if (raw <= 143) return 0.5 * raw / 143.0;
    return 0.5 + 0.5 * (raw - 143) / 70.0;
}
double HvPwrLink::SwrValue(int raw)     // Icom SWR meter: 0=1.0, 48=1.5, 80=2.0, 120=3.0
{
    if (raw <= 0) return 1.0;
    if (raw <= 48) return 1.0 + 0.5 * raw / 48.0;
    if (raw <= 80) return 1.5 + 0.5 * (raw - 48) / 32.0;
    return 2.0 + (raw - 80) / 40.0;
}
QString HvPwrLink::Hex(const QByteArray &b)
{
    QString s;
    for (int i = 0; i < b.size(); ++i)
        s += QString("%1 ").arg((unsigned char)b.at(i),2,16,QChar('0')).toUpper();
    return s.trimmed();
}
void HvPwrLink::SetOk(bool ok,const QString &t)
{
    f_ok = ok;
    emit Status(ok,t);
}
void HvPwrLink::Open(int kind,const QString &host,int port,const QString &com,int baud,int civ)
{
    Close();
    s_kind = kind;
    s_host = host.trimmed().isEmpty() ? QString("127.0.0.1") : host.trimmed();
    s_port = port;
    s_com = com.trimmed();
    s_baud = baud;
    s_civ = civ & 0xFF;
    no_po_meter = false;
    fails = 0;
    if (kind == RIGCTLD)
    {
        tcp = new QTcpSocket(this);
        connect(tcp, SIGNAL(connected()), this, SLOT(TcpConnected()));
        connect(tcp, SIGNAL(readyRead()), this, SLOT(TcpRead()));
        connect(tcp, SIGNAL(disconnected()), this, SLOT(TcpError()));
        connect(tcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(TcpError()));
        SetOk(false,"Hamlib rigctld "+s_host+":"+QString::number(s_port)+"  ...");
        tcp->connectToHost(s_host,(quint16)s_port);
    }
    else if (kind == ICOM_SERIAL)
    {
        if (s_com.isEmpty())
        {
            SetOk(false,"Icom CI-V: no serial port");
            return;
        }
        ser = new QextSerialPort(s_com, QextSerialPort::EventDriven, this);
        ser->setBaudRate((BaudRateType)s_baud);
        ser->setDataBits(DATA_8);
        ser->setParity(PAR_NONE);
        ser->setStopBits(STOP_1);
        ser->setFlowControl(FLOW_OFF);
        ser->setTimeout(50);
        if (!ser->open(QIODevice::ReadWrite))
        {
            QString e = ser->errorString();
            delete ser;
            ser = 0;
            SetOk(false,"Icom CI-V "+s_com+": "+tr("cannot open the port (used by another program?)")+" "+e);
            t_retry->start(5000);
            return;
        }
        ser->setDtr(false);
        ser->setRts(false);
        connect(ser, SIGNAL(readyRead()), this, SLOT(SerRead()));
        SetOk(true,"Icom CI-V "+s_com+" "+QString::number(s_baud)+" "+tr("address")+" "+
              QString("%1h").arg(s_civ,2,16,QChar('0')).toUpper());
    }
    else SetOk(false,QString());
}
void HvPwrLink::Close()
{
    t_to->stop();
    t_retry->stop();
    queue.clear();
    f_busy = false;
    inbuf.clear();
    if (tcp)
    {
        tcp->disconnect(this);
        tcp->abort();
        tcp->deleteLater();
        tcp = 0;
    }
    if (ser)
    {
        ser->disconnect(this);
        ser->close();
        delete ser;
        ser = 0;
    }
    f_ok = false;
}
void HvPwrLink::Retry()
{
    if (s_kind != NONE && !f_ok) Open(s_kind,s_host,s_port,s_com,s_baud,s_civ);
}
// ---------------------------------------------------------------- commands
QByteArray HvPwrLink::Civ(const QByteArray &body) const
{
    QByteArray f;
    f.append((char)0xFE).append((char)0xFE).append((char)s_civ).append((char)0xE0);
    f.append(body);
    f.append((char)0xFD);
    return f;
}
void HvPwrLink::Enqueue(const QByteArray &d,int tag)
{
    if (!f_ok) return;
    for (int i = 0; i < queue.count(); ++i)     // no duplicates of a pending read
        if (queue.at(i).tag == tag && tag != T_SET) return;
    if (queue.count() > 20) return;
    Cmd c;
    c.data = d;
    c.tag = tag;
    queue.append(c);
    Next();
}
void HvPwrLink::Next()
{
    if (f_busy || queue.isEmpty() || !f_ok) return;
    Cmd c = queue.takeFirst();
    f_busy = true;
    cur_tag = c.tag;
    if (s_kind == RIGCTLD && tcp)
    {
        tcp->write(c.data);
        emit LogLine(true,QString::fromLatin1(c.data).trimmed());
    }
    else if (s_kind == ICOM_SERIAL && ser)
    {
        ser->write(c.data);
        emit LogLine(true,Hex(c.data));
    }
    t_to->start(s_kind == RIGCTLD ? 2000 : 600);
}
void HvPwrLink::Done()
{
    t_to->stop();
    f_busy = false;
    cur_tag = -1;
    fails = 0;
    Next();
}
void HvPwrLink::Timeout()
{
    emit LogLine(false,tr("no answer"));
    f_busy = false;
    cur_tag = -1;
    if (++fails >= 6 && s_kind == ICOM_SERIAL)
    {
        emit Status(f_ok,"Icom CI-V "+s_com+": "+tr("the radio does not answer (check CI-V address and baud rate)"));
        fails = 0;
    }
    Next();
}
void HvPwrLink::SetPower(int pct)
{
    pct = qBound(0,pct,100);
    if (s_kind == RIGCTLD)
        Enqueue(QString("L RFPOWER %1\n").arg(pct/100.0,0,'f',2).toLatin1(),T_SET);
    else if (s_kind == ICOM_SERIAL)
    {
        int v = (int)(pct*255.0/100.0+0.5);
        QByteArray b;
        b.append((char)0x14).append((char)0x0A);
        b.append((char)(v/100));                         // BCD 0x0H
        b.append((char)((((v%100)/10)<<4) | (v%10)));     // BCD 0xTU
        Enqueue(Civ(b),T_SET);
    }
}
void HvPwrLink::ReadPower()
{
    if (s_kind == RIGCTLD) Enqueue("l RFPOWER\n",T_RFP);
    else if (s_kind == ICOM_SERIAL) Enqueue(Civ(QByteArray("\x14\x0A",2)),T_RFP);
}
void HvPwrLink::ReadMeters()
{
    if (s_kind == RIGCTLD)
    {
        if (!no_po_meter) Enqueue("l RFPOWER_METER\n",T_PO);
        Enqueue("l SWR\n",T_SWR);
        Enqueue("l ALC\n",T_ALC);
    }
    else if (s_kind == ICOM_SERIAL)
    {
        Enqueue(Civ(QByteArray("\x15\x11",2)),T_PO);
        Enqueue(Civ(QByteArray("\x15\x12",2)),T_SWR);
        Enqueue(Civ(QByteArray("\x15\x13",2)),T_ALC);
    }
}
void HvPwrLink::ReadFreq()
{
    if (s_kind == RIGCTLD) Enqueue("f\n",T_FREQ);
    else if (s_kind == ICOM_SERIAL) Enqueue(Civ(QByteArray("\x03",1)),T_FREQ);
}
// ---------------------------------------------------------------- rigctld
void HvPwrLink::TcpConnected()
{
    SetOk(true,"Hamlib rigctld "+s_host+":"+QString::number(s_port));
    Next();
}
void HvPwrLink::TcpError()
{
    if (!tcp) return;
    QString e = tcp->errorString();
    tcp->disconnect(this);
    tcp->deleteLater();
    tcp = 0;
    t_to->stop();
    f_busy = false;
    queue.clear();
    SetOk(false,"Hamlib rigctld "+s_host+":"+QString::number(s_port)+": "+e+" - "+tr("retry in 5 s"));
    t_retry->start(5000);
}
void HvPwrLink::TcpRead()
{
    if (!tcp) return;
    inbuf += tcp->readAll();
    int i;
    while ((i = inbuf.indexOf('\n')) >= 0)
    {
        QString l = QString::fromLatin1(inbuf.left(i)).trimmed();
        inbuf.remove(0,i+1);
        if (!l.isEmpty()) TcpLine(l);
    }
}
void HvPwrLink::TcpLine(const QString &l)
{
    emit LogLine(false,l);
    if (!f_busy) return;
    int tag = cur_tag;
    if (l.startsWith("RPRT"))
    {
        if (tag == T_PO && l != "RPRT 0") no_po_meter = true;   // old Hamlib: no RFPOWER_METER
        Done();
        return;
    }
    bool ok = false;
    double v = l.toDouble(&ok);
    if (ok)
    {
        if (tag == T_RFP) emit RfPct((int)(v*100.0+0.5));
        else if (tag == T_PO) emit Po(v);
        else if (tag == T_SWR) emit Swr(v < 1.0 ? 1.0 : v);
        else if (tag == T_ALC) emit Alc(v);
        else if (tag == T_FREQ) emit FreqHz(v);
    }
    Done();
}
// ---------------------------------------------------------------- Icom CI-V serial
void HvPwrLink::SerRead()
{
    if (!ser) return;
    inbuf += ser->readAll();
    for (;;)
    {
        int s = inbuf.indexOf(QByteArray("\xFE\xFE",2));
        if (s < 0)
        {
            if (inbuf.size() > 256) inbuf.clear();
            return;
        }
        if (s > 0) inbuf.remove(0,s);
        int e = inbuf.indexOf((char)0xFD);
        if (e < 0) return;
        QByteArray f = inbuf.left(e+1);
        inbuf.remove(0,e+1);
        while (f.size() > 2 && (unsigned char)f.at(2) == 0xFE) f.remove(0,1);
        CivFrame(f);
    }
}
static int bcd(unsigned char b)
{
    return (b>>4)*10 + (b&0x0F);
}
void HvPwrLink::CivFrame(const QByteArray &f)
{
    if (f.size() < 5) return;
    unsigned char to = f.at(2), from = f.at(3);
    if (from == 0xE0) return;                      // echo of our command
    emit LogLine(false,Hex(f));
    if (to != 0xE0 && to != 0x00) return;
    if (from != (unsigned char)s_civ)
    {
        emit Status(true,"Icom CI-V "+s_com+": "+tr("the radio answers with address")+" "+
                    QString("%1h").arg(from,2,16,QChar('0')).toUpper()+" - "+tr("set it in Settings"));
        return;
    }
    unsigned char c = f.at(4);
    if ((c == 0x00 || c == 0x03) && f.size() >= 11)  // frequency (also transceive 00)
    {
        double hz = 0.0, m = 1.0;
        for (int i = 5; i < 10; ++i)
        {
            hz += bcd((unsigned char)f.at(i)) * m;
            m *= 100.0;
        }
        emit FreqHz(hz);
        if (c == 0x03 && f_busy && cur_tag == T_FREQ) Done();
        return;
    }
    if (c == 0xFB || c == 0xFA)
    {
        if (f_busy) Done();
        return;
    }
    if ((c == 0x14 || c == 0x15) && f.size() >= 9)
    {
        unsigned char sc = f.at(5);
        int raw = bcd((unsigned char)f.at(6))*100 + bcd((unsigned char)f.at(7));
        if (c == 0x14 && sc == 0x0A) emit RfPct((int)(raw*100.0/255.0+0.5));
        else if (c == 0x15 && sc == 0x11) emit Po(PoFraction(raw));
        else if (c == 0x15 && sc == 0x12) emit Swr(SwrValue(raw));
        else if (c == 0x15 && sc == 0x13) emit Alc(raw/120.0);
        if (f_busy) Done();
    }
}
