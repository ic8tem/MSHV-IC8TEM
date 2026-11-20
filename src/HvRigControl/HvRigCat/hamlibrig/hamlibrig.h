/* MSHV-IC8TEM  "Icom IC-7300/IC-7610/IC-9700 (Hamlib)": the radio driven by the Hamlib library
 * (frequency, mode, PTT, RF power, Po/SWR/ALC meters).
 * Rig Control > Network: Server = serial port (e.g. COM5), Port = baud rate (e.g. 115200).
 * Hamlib calls are blocking, so they run in a worker thread.
 *
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef HAMLIBRIG_H
#define HAMLIBRIG_H

#include <QObject>
#include <QThread>
#include <QAtomicInt>
#include <QMutex>
#include "../rigdef.h"

class HamlibWorker : public QObject
{
    Q_OBJECT
public:
    HamlibWorker(int model);
    QAtomicInt pend_freq, pend_mode, pend_meters, pend_power;
signals:
    void Status(QString,int);        // 0 closed, 2 open
    void Readed(int,QString);        // CmdID, value
public slots:
    void Open(QString port,int baud);
    void CloseRig();
    void CloseAndQuit();
    void Do(int cmd,int ptt,QString arg);
private:
    int s_model;
    void *rig;
    int errors;
    QString cur_port;//IC8TEM v24
    int cur_baud;
    void Err(const QString &what,int e);
};

class HamlibRig : public QObject
{
    Q_OBJECT
public:
    HamlibRig(int ModelID,QObject *parent = 0);
    virtual ~HamlibRig();

signals:
    void EmitRigSet(RigSet);
    void EmitWriteCmd(char*data,int size);
    void EmitReadedInfo(CmdID,QString);
    void EmitNetConnInfo(QString,bool,bool);
    void EmitFullRigInfo(QString);
    void EmitCmdSent();               // IC8TEM v26: set_freq/set_mode/set_ptt sent, polling may go on
    // internal
    void WOpen(QString,int);
    void WClose();
    void WDo(int,int,QString);

public slots:
    void ConnectNet(QString);
    void SetOnOffCatCommand(bool,int,int);
    void SetCmd(CmdID,ptt_t,QString);

private slots:
    void WStatus(QString,int);
    void WReaded(int,QString);

private:
    int s_ModelID;
    bool running;
    QThread *thr;
    HamlibWorker *w;
};
#endif
