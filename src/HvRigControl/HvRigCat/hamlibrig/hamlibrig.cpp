/* MSHV-IC8TEM  "Icom IC-7300 (Hamlib)". See hamlibrig.h.
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include "hamlibrig.h"
#include "hamlib_wrap.h"
#define _NETWORK_RIGS_
#include "../network/network_def.h"

#define HL_MODEL_IC7300 3073   // RIG_MODEL_IC7300
static QMutex g_hl_port;          // one Hamlib open/close at a time (a closing rig frees the COM port first)

// ---------------------------------------------------------------- worker (own thread)
HamlibWorker::HamlibWorker(int model)
        : QObject(0)
{
    s_model = model;
    rig = 0;
    errors = 0;
}
void HamlibWorker::Err(const QString &what,int e)
{
    emit Readed(LOG_RX,"Hamlib "+what+": "+HlWrap::Error(e));
    if (++errors >= 10)
    {
        errors = 0;
        emit Status("Hamlib: "+tr("the radio does not answer")+" ("+HlWrap::Error(e)+")",2);
    }
}
void HamlibWorker::Open(QString port,int baud)
{
    CloseRig();
    if (port.isEmpty() || port=="None")
    {
        emit Status(tr("Choose the COM port of the radio in Port and the speed in Baud Rate"),0);
        return;
    }
    QMutexLocker lk(&g_hl_port);
    QString err;
    rig = HlWrap::Open(s_model,port,baud,err);
    if (!rig)
    {
        emit Status(err,0);
        return;
    }
    errors = 0;
    emit Status(tr("Connected")+" "+port+" "+QString::number(baud)+"  ("+HlWrap::Version()+")",2);
}
void HamlibWorker::CloseRig()
{
    if (!rig) return;
    QMutexLocker lk(&g_hl_port);
    HlWrap::SetPtt(rig,false);
    HlWrap::Close(rig);
    rig = 0;
    emit Status(tr("Disconnected"),0);
}
void HamlibWorker::CloseAndQuit()
{
    CloseRig();
    thread()->quit();
}
void HamlibWorker::Do(int cmd,int ptt,QString arg)
{
    if (!rig)
    {
        pend_freq.storeRelease(0); pend_mode.storeRelease(0); pend_meters.storeRelease(0); pend_power.storeRelease(0);
        return;
    }
    int e = 0;
    switch (cmd)
    {
    case SET_PTT:
        e = HlWrap::SetPtt(rig,ptt!=RIG_PTT_OFF);
        emit Readed(LOG_TX,QString("Hamlib set_ptt ")+(ptt!=RIG_PTT_OFF ? "ON" : "OFF")+(e ? " - "+HlWrap::Error(e) : QString(" OK")));
        break;
    case SET_FREQ:
        e = HlWrap::SetFreq(rig,arg.toDouble());
        emit Readed(LOG_TX,"Hamlib set_freq "+arg+(e ? " - "+HlWrap::Error(e) : QString(" OK")));
        break;
    case SET_MODE:
        e = HlWrap::SetMode(rig,arg);
        emit Readed(LOG_TX,"Hamlib set_mode "+arg+(e ? " - "+HlWrap::Error(e) : QString(" OK")));
        break;
    case GET_FREQ:
    {
        pend_freq.storeRelease(0);
        double hz = 0.0;
        e = HlWrap::GetFreq(rig,&hz);
        if (e == 0 && hz > 0.0)
        {
            errors = 0;
            emit Readed(GET_FREQ,QString::number((qlonglong)(hz+0.5)));
        }
        else Err("get_freq",e);
        break;
    }
    case GET_MODE:
    {
        pend_mode.storeRelease(0);
        QString m;
        e = HlWrap::GetMode(rig,m);
        if (e == 0) emit Readed(GET_MODE,m);
        else Err("get_mode",e);
        break;
    }
    case SET_RFPOWER:   // arg = 0..255 (as for the Icom driver)
    {
        float v = arg.toInt()/255.0f;
        e = HlWrap::SetRfPower(rig,v);
        emit Readed(LOG_TX,"Hamlib set_level RFPOWER "+QString::number(v,'f',2)+(e ? " - "+HlWrap::Error(e) : QString(" OK")));
        break;
    }
    case GET_RFPOWER:
    {
        pend_power.storeRelease(0);
        float v = 0.0f;
        e = HlWrap::GetLevel(rig,HlWrap::L_RFPOWER,&v);
        emit Readed(LOG_RX,"Hamlib RFPOWER = "+(e ? HlWrap::Error(e) : QString::number(v,'f',3)));
        if (e == 0) emit Readed(READ_RFPOWER,"f:"+QString::number(v,'f',4));
        break;
    }
    case GET_METERS:
    {
        pend_meters.storeRelease(0);
        float po = 0.0f, swr = 1.0f, alc = 0.0f;
        int e1 = HlWrap::GetLevel(rig,HlWrap::L_RFPOWER_METER,&po);
        int e2 = HlWrap::GetLevel(rig,HlWrap::L_SWR,&swr);
        int e3 = HlWrap::GetLevel(rig,HlWrap::L_ALC,&alc);
        emit Readed(LOG_RX,"Hamlib RFPOWER_METER = "+(e1 ? HlWrap::Error(e1) : QString::number(po,'f',3))+
                    "  SWR = "+(e2 ? HlWrap::Error(e2) : QString::number(swr,'f',2))+
                    "  ALC = "+(e3 ? HlWrap::Error(e3) : QString::number(alc,'f',3)));
        if (e1 == 0) emit Readed(READ_PO,"f:"+QString::number(po,'f',4));
        if (e2 == 0) emit Readed(READ_SWR,"f:"+QString::number(swr < 1.0f ? 1.0f : swr,'f',3));
        if (e3 == 0) emit Readed(READ_ALC,"f:"+QString::number(alc,'f',4));
        break;
    }
    default:
        break;
    }
}

// ---------------------------------------------------------------- rig (GUI thread)
HamlibRig::HamlibRig(int ModelID,QObject *parent)
        : QObject(parent)
{
    s_ModelID = ModelID;
    running = false;
    w = new HamlibWorker(HL_MODEL_IC7300);
    thr = new QThread();
    w->moveToThread(thr);
    connect(this, SIGNAL(WOpen(QString,int)), w, SLOT(Open(QString,int)));
    connect(this, SIGNAL(WClose()), w, SLOT(CloseRig()));
    connect(this, SIGNAL(WDo(int,int,QString)), w, SLOT(Do(int,int,QString)));
    connect(w, SIGNAL(Status(QString,int)), this, SLOT(WStatus(QString,int)));
    connect(w, SIGNAL(Readed(int,QString)), this, SLOT(WReaded(int,QString)));
    thr->start();
}
HamlibRig::~HamlibRig()
{
    // never wait here (GUI thread): the worker may be inside a slow rig_open.
    // It closes the radio and deletes itself and its thread when it is free.
    disconnect(w, 0, this, 0);
    connect(thr, SIGNAL(finished()), w, SLOT(deleteLater()));
    connect(thr, SIGNAL(finished()), thr, SLOT(deleteLater()));
    QMetaObject::invokeMethod(w, "CloseAndQuit", Qt::QueuedConnection);
}
void HamlibRig::ConnectNet(QString all)
{
    QStringList l = all.split("#");
    QString port = l.value(0).trimmed();
    int baud = l.value(1).trimmed().toInt();
    bool reopen = (l.value(11)=="reopen");//IC8TEM: Port or Baud Rate changed in Rig Control
    if (running && !reopen)
    {
        running = false;
        emit WClose();
        return;
    }
    if (port.isEmpty() || port=="None")
    {
        running = false;
        emit WClose();
        emit EmitNetConnInfo("<font color='red'>"+tr("Choose the COM port of the radio in Port and the speed in Baud Rate")+"</font>",false,false);
        return;
    }
    running = true;
    emit EmitNetConnInfo("<font color='red'>"+tr("Connecting to")+" "+port+" "+QString::number(baud)+"</font>",true,false);
    emit WOpen(port,baud);
}
void HamlibRig::SetOnOffCatCommand(bool f,int model_id,int fact_id)
{
    if (model_id!=s_ModelID || fact_id!=NETWORK_ID) return;
    if (!f) emit WDo(SET_PTT,RIG_PTT_OFF,QString());
}
void HamlibRig::SetCmd(CmdID i,ptt_t ptt,QString str)
{
    if (i==GET_SETT)
    {
        emit EmitRigSet(rigs_network[s_ModelID]);
        return;
    }
    if (!running) return;
    // reads: at most one waiting of each kind (Hamlib calls take time)
    if (i==GET_FREQ && !w->pend_freq.testAndSetOrdered(0,1)) return;
    if (i==GET_MODE && !w->pend_mode.testAndSetOrdered(0,1)) return;
    if (i==GET_METERS && !w->pend_meters.testAndSetOrdered(0,1)) return;
    if (i==GET_RFPOWER && !w->pend_power.testAndSetOrdered(0,1)) return;
    emit WDo((int)i,(int)ptt,str);
}
void HamlibRig::WStatus(QString s,int state)
{
    if (s.length() > 160)//long Hamlib messages: the whole text goes to the CAT Log
    {
        emit EmitReadedInfo(LOG_RX,s);
        s = s.left(157)+"...";
    }
    if (state==0)
    {
        running = false;
        emit EmitNetConnInfo("<font color='red'>"+s+"</font>",false,false);
    }
    else
    {
        emit EmitNetConnInfo("<font color='#00b300'>"+s+"</font>",true,true);
        emit EmitFullRigInfo("Icom IC-7300 (Hamlib)");
    }
}
void HamlibRig::WReaded(int id,QString s)
{
    emit EmitReadedInfo((CmdID)id,s);
}
