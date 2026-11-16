/* MSHV-IC8TEM  Power & SWR: own connection to the radio, independent from the MSHV CAT
 *
 *  - Hamlib rigctld (TCP, any radio supported by Hamlib):
 *      l RFPOWER / L RFPOWER x / l RFPOWER_METER / l SWR / l ALC / f
 *  - Icom CI-V on its own serial port (when no other program uses that port):
 *      14 0A, 15 11, 15 12, 15 13, 03
 *
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef HVPWRLINK_H
#define HVPWRLINK_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QStringList>
#include <QByteArray>
#include <QList>

class QextSerialPort;

class HvPwrLink : public QObject
{
    Q_OBJECT
public:
    enum Kind { NONE = 0, RIGCTLD = 1, ICOM_SERIAL = 2 };
    HvPwrLink(QObject *parent = 0);
    virtual ~HvPwrLink();
    void Open(int kind,const QString &host,int port,const QString &com,int baud,int civ);
    void Close();
    bool IsOpen() const { return f_ok; }
    int CurKind() const { return s_kind; }
    void SetPower(int pct);
    void ReadPower();
    void ReadMeters();
    void ReadFreq();
    static double PoFraction(int raw);   // Icom Po meter raw -> 0..1
    static double SwrValue(int raw);     // Icom SWR meter raw -> SWR

signals:
    void Status(bool ok,QString text);
    void RfPct(int);
    void Po(double);       // fraction of the maximum power
    void Swr(double);
    void Alc(double);      // 0..1 = top of the ALC zone
    void FreqHz(double);
    void LogLine(bool tx,QString);

private slots:
    void TcpConnected();
    void TcpRead();
    void TcpError();
    void SerRead();
    void Timeout();
    void Retry();

private:
    struct Cmd { QByteArray data; int tag; };
    enum Tag { T_SET = 0, T_RFP, T_PO, T_SWR, T_ALC, T_FREQ };
    int s_kind;
    bool f_ok;
    QString s_host, s_com;
    int s_port, s_baud, s_civ;
    QTcpSocket *tcp;
    QextSerialPort *ser;
    QTimer *t_to;
    QTimer *t_retry;
    QList<Cmd> queue;
    bool f_busy;
    int cur_tag;
    QByteArray inbuf;
    bool no_po_meter;
    int fails;
    void Enqueue(const QByteArray &d,int tag);
    void Next();
    void Done();
    void SetOk(bool ok,const QString &t);
    void TcpLine(const QString &l);
    void CivFrame(const QByteArray &f);
    QByteArray Civ(const QByteArray &body) const;
    static QString Hex(const QByteArray &b);
};
#endif
