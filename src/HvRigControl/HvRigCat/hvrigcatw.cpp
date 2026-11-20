/* MSHV HvRigCat
 * Copyright 2015 Hrisimir Hristov, LZ2HV
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include "hvrigcatw.h"
//#include <QtGui>

#include "yaesu/yaesu_def.h"
#include "yaesu/yaesu.h"
#include "icom/icom_def.h"
#include "icom/icom.h"
#include "kenwood/kenwood_def.h"
#include "kenwood/kenwood.h"
#include "elecraft/elecraft_def.h"
#include "elecraft/elecraft.h"
#include "tentec/tentec_def.h"
#include "tentec/tentec.h"
#include "alinco/alinco_def.h"
#include "alinco/alinco.h"
#include "jrc/jrc_def.h"
#include "jrc/jrc.h"
#include "drake/drake_def.h"
#include "drake/drake.h"
#include "icmarine/icmarine_def.h"
#include "icmarine/icmarine.h"
#include "pcr/pcr_def.h"
#include "pcr/pcr.h"
#include "racal/racal_def.h"
#include "racal/racal.h"
#include "sdrs/sdrs_def.h"
#include "sdrs/sdrs.h"
#define _NETWORK_RIGS_
#include "network/network_def.h"
#include "network/network.h"
#include "icomlan/icomlan.h" //IC8TEM Icom LAN
#include "hamlibrig/hamlibrig.h" //IC8TEM Icom IC-7300 (Hamlib)
#include "mits/mits_def.h"
#include "mits/mits.h"
////// omnirig /////////////
#if defined _WIN32_
#include "omnirig/omnirighv_def.h"
#include "omnirig/omnirighv.h"
#endif
////// omnirig end /////////

#include <unistd.h>   // uslepp Linux and windows
//#include <windows.h>  // Slepp windows
/*#if defined _LINUX_
#include <unistd.h>
#endif

#if defined _WIN32_
#include <windows.h>
#endif
*/
typedef struct
{
    QString name;
    int facid;
    int model;
}
rigstruct;
static QList<rigstruct>lmodel;
bool hv_sort_nameorder(const rigstruct &d1,const rigstruct &d2)
{
    return d1.name.toLower() < d2.name.toLower();
}
#define POLLMAX 4
#define POLLRST (((POLLMAX*60000)/400)+200)//400=minpoll*2
HvRigCat::HvRigCat( QWidget *parent )
        : QWidget(parent)
{
    //s_mode = 11;//tci
    tci_select = 0;
    s_active_model_id = -100;
    s_active_fact_id = -100;
    have_read_data_rts_on = 0;
    c_poll_comm = -1;
    s_port_type = RIG_PORT_NONE;
    s_port_poen = false;
    pwr_k3_tm = 0;//IC8TEM
    pwr_tx = false;//IC8TEM v29
    vita_rx = 0;
    vita_tx = false;

    f_rig_activ = false;
    QLabel *l_rb = new QLabel("RIG:");
    RigsBoxs = new QComboBox();
    //RigsBoxs->setFixedWidth(215);
    RigsBoxs->setMinimumWidth(225);

    rb_ptt = new QRadioButton("PTT");
    rb_ptt_mic = new QRadioButton("PTT MIC");
    rb_ptt_data = new QRadioButton("PTT DATA");
    sb_pollint = new QSpinBox();
    sb_pollint->setPrefix("Poll, int  ");
    sb_pollint->setSuffix("  ms");
    sb_pollint->setRange(200,1000);
    sb_pollint->setSingleStep(100);
    sb_pollint->setValue(1000);
    sb_pollint->findChild<QLineEdit*>()->setReadOnly(true);
    //sb_pollint->setStyleSheet("QSpinBox {selection-color: black; selection-background-color: white;}");

    connect(rb_ptt, SIGNAL(toggled(bool)), this, SLOT(PttChanget(bool)));
    connect(rb_ptt_mic, SIGNAL(toggled(bool)), this, SLOT(PttChanget(bool)));
    connect(rb_ptt_data, SIGNAL(toggled(bool)), this, SLOT(PttChanget(bool)));
    connect(sb_pollint, SIGNAL(valueChanged(int)), this, SLOT(SBPollIntChanged(int)));

    SetPttCaps(false);
    s_ptt_on_type = RIG_PTT_ON;

    QHBoxLayout *H_L = new QHBoxLayout();
    H_L->setContentsMargins(1,1,1,1);
    H_L->setSpacing(4);
    H_L->addWidget(l_rb);
    H_L->addWidget(RigsBoxs);
    H_L->addWidget(rb_ptt);
    H_L->addWidget(rb_ptt_mic);
    H_L->addWidget(rb_ptt_data);
    H_L->addWidget(sb_pollint);

    //H_L->removeWidget(H_L->itemAt(0)->widget());
    this->setLayout(H_L);

    //QStringList ls;
    rigstruct rstruc;
    for (int x=0; x<15; ++x)//2.76.1  15=count facid`s
    {
        int crigs = 0;
        int rigid = x;
        RigSet *rigs_x;// = NULL;
        QString fnam = "";
        if      (x==0)
        {
            crigs = YAESU_COUNT;
            rigs_x = rigs_yeasu;
            fnam = "Yaesu ";
        }
        else if (x==1)
        {
            crigs = ICOM_COUNT;
            rigs_x = rigs_icom;
            fnam = "Icom ";
        }
        else if (x==2)
        {
            crigs = KENWOOD_COUNT;
            rigs_x = rigs_kenwood;
            fnam = "Kenwood ";
        }
        else if (x==3)
        {
            crigs = ELECRAFT_COUNT;
            rigs_x = rigs_elecraft;
            fnam = "Elecraft ";
        }
        else if (x==4)
        {
            crigs = TENTEC_COUNT;
            rigs_x = rigs_tentec;
            fnam = "Ten-Tec ";
        }
        else if (x==5)
        {
            crigs = ALINCO_COUNT;
            rigs_x = rigs_alinco;
            fnam = "Alinco ";
        }
        else if (x==6)
        {
            crigs = JRC_COUNT;
            rigs_x = rigs_jrc;
            fnam = "JRC ";
        }
        else if (x==7)
        {
            crigs = DRAKE_COUNT;
            rigs_x = rigs_drake;
            fnam = "Drake ";
        }
        else if (x==8)
        {
            crigs = ICMARINE_COUNT;
            rigs_x = rigs_icmarine;
            fnam = "Icom ";
        }
        else if (x==9)
        {
            crigs = PCR_COUNT;
            rigs_x = rigs_pcr;
            fnam = "Icom ";
        }
        else if (x==10)
        {
            crigs = RACAL_COUNT;
            rigs_x = rigs_racal;
            fnam = "Racal ";
        }
        else if (x==11)
        {
#if defined _WIN32_
            crigs = OMNIRIGHV_COUNT;
            rigs_x = rigs_omnirighv;
            fnam = "OmniRig ";
#else
            continue;
#endif
        }
        else if (x==12)
        {
            crigs = SDRS_COUNT;
            rigs_x = rigs_sdrs;
        }
        else if (x==13)
        {
            crigs = NETWORK_COUNT;
            rigs_x = rigs_network;
        }
        else if (x==14)
        {
            crigs = MITS_COUNT;
            rigs_x = rigs_mits;
        }
        for (int i=0; i<crigs; ++i)
        {
            QString rn = fnam+rigs_x[i].name;
            QString rn0 = rigs_x[i].name;
            if      (x==0 && (rn0.startsWith("Ailunce ") ||       //Yaesu
            				  rn0.startsWith("Radioddity ") ||
            				  rn0.startsWith("Guohe "))) rn = rn0;
            else if (x==1 &&  rn0.startsWith("Xiegu ")) rn = rn0; //Icom
            else if (x==2 && (rn0.startsWith("SDRPlay ") ||		  //Kenwood
                              rn0.startsWith("QRPLabs ") ||
                              rn0.startsWith("Malachite ") ||
                              rn0.startsWith("Hilberling ") ||
                              rn0.startsWith("OpenHPSDR ") ||
                              rn0.startsWith("SDR Radio ") ||
                              rn0.startsWith("BG2FX ") ||
                              rn0.startsWith("Lab599 ") ||
                              rn0.startsWith("DL2MAN "))) rn = rn0;
            rstruc.name = rn;
            rstruc.facid = rigid;
            rstruc.model = i;
            lmodel.append(rstruc);
        }
    }
    std::sort(lmodel.begin(),lmodel.end(),hv_sort_nameorder);
    rstruc.name = "None";
    rstruc.facid = -1;
    rstruc.model = -1;
    lmodel.prepend(rstruc);

    QStringList lst_outb;
    for (int i = 0; i<lmodel.count(); i++)
    {
        lst_outb << lmodel.at(i).name;
        //if (lmodel.at(i).facid==11) 
        //printf("%s %s %d %d\n",qPrintable(QString("%1").arg(i).leftJustified(3,' ')),qPrintable(lmodel.at(i).name.leftJustified(30,' ')),lmodel.at(i).facid,lmodel.at(i).model);
    }
    RigsBoxs->addItems(lst_outb);
    RigsBoxs->setMaxVisibleItems(12);//2.76.1
    connect(RigsBoxs, SIGNAL(activated(int)), this, SLOT(SetRig(int)));

    polling_timer = new QTimer();
    connect(polling_timer, SIGNAL(timeout()), this, SLOT(PollingTimerReadRig()));
    f_rig_active_never_stop = false;//2.76.1 Flex Slice
    max4min = (POLLMAX*60000)/(sb_pollint->value()*2);
    c_rig_cat_active_and_read = POLLRST;//reset  ((POLLMAX*60000)/400)+200=700 400=minpoll*2

    s_f_ptt = false;
    tx_rx_timer = new QTimer();
    connect(tx_rx_timer, SIGNAL(timeout()), this, SLOT(StartPttTimer()));

////// omnirig /////////////
#if defined _WIN32_
    omnirig_reinit_timer = new QTimer();
    connect(omnirig_reinit_timer, SIGNAL(timeout()), this, SLOT(SetOmniRigReinit()));
#endif
////// omnirig end /////////////
}
HvRigCat::~HvRigCat()
{}
/*
void HvRigCat::CatStopPttIfClose()
{
	if ((s_port_type == RIG_PORT_SERIAL || s_port_type == RIG_PORT_NETWORK) && s_port_poen && s_f_ptt)
	{
		emit SetCmd(SET_PTT,RIG_PTT_OFF,"");
		//qDebug()<<"CLOSE DestroyPort()"<<s_port_poen<<s_f_ptt;
	}
}
*/
/*void HvRigCat::SetMode(int m)//tci for samplerate
{
	s_mode = m;
	emit EmitMode(s_mode);
}*/
/*bool HvRigCat::hv_sort_nameorder(const rigstruct &d1,const rigstruct &d2)
{
    return d1.name.toLower() < d2.name.toLower();
}*/
/*void HvRigCat::SetFont(QFont fo)
{
    sb_pollint->setFont(fo);
}*/
void HvRigCat::SBPollIntChanged(int i)
{	//qDebug()<<"old"<<max4min<<POLLRST;
    max4min = (POLLMAX*60000)/(i*2); //qDebug()<<"new"<<max4min<<POLLRST;
    if ((s_port_type == RIG_PORT_SERIAL || s_port_type == RIG_PORT_NETWORK) && s_port_poen) polling_timer->start(i);
}
void HvRigCat::SetTciSelect(int i,int vr,bool vt)
{
    tci_select = i;
    vita_rx = vr;
    vita_tx = vt;
    emit EmitTciSelect(tci_select,vr,vt);
}
void HvRigCat::DestroyRig()
{
    if (f_rig_activ)
    {
        //qDebug()<<"DestroyRig()";
        emit DesleteRig();
    }
    f_rig_activ = false;
}
////// omnirig /////////////
#if defined _WIN32_
void HvRigCat::SetOmniRigChanged()//from omni rig reinit
{
    omnirig_reinit_timer->start(752);//hi hi
}
void HvRigCat::SetOmniRigReinit()
{
    omnirig_reinit_timer->stop();
    int index = RigsBoxs->currentIndex();
    if (index >= 0)
    {
        RigsBoxs->setCurrentIndex(index);
        SetRig(index);//tova opravia i SetPttCaps();
    }
}
#endif
////// omnirig end /////////////
void HvRigCat::SetRig(int index)
{
    StartStopPollingTimer(false);
    //if (index!=0)//not up 0 object
    //{
    //	RigSet sett = 0;

    rigstruct rstruc;
    rstruc.facid = lmodel.at(index).facid;
    rstruc.model = lmodel.at(index).model;
    rstruc.name = lmodel.at(index).name;

    QString rig0info = rstruc.name;
    if (rstruc.facid == NETWORK_ID) rig0info.remove(" TCP");
    if (rstruc.facid == SDRS_ID) rig0info.remove(" Ser");
    if (rstruc.name == "Ham Radio Deluxe") rig0info = "N/A (HRD)";
    if (rstruc.name == "FLRig") rig0info = "N/A (FLRig)";
    if (rstruc.name == "DX Lab Suite Commander") rig0info = "N/A (DXLab)";
    if (rstruc.name.contains("TCI Client RX")) rig0info = "N/A (TCI)";
    if (rstruc.name == "Hamlib NET rigctl") rig0info = "N/A (NETRigCtl)";//HLNETRigCtl
    if (rstruc.name.contains("OmniRig Rig ")) rig0info = "N/A (OmniRig)";
    emit EmitFullRigInfo(rig0info);//2.76.1 for pskreporter
    //qDebug()<<"STOP TIMER SetRig="<<index<<rstruc.facid<<rstruc.model<<rstruc.name<<"rig0info="<<rig0info;

    DestroyRig();
    s_active_model_id = -100;
    s_active_fact_id = -100;
    f_rig_active_never_stop = false;//2.76.1 Flex Slice

    have_read_data_rts_on = 0;

    if (rstruc.facid == YAESU_ID)
    {
        Yaesu *TYaesu = new Yaesu(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TYaesu, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TYaesu, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TYaesu, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TYaesu, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TYaesu, SLOT(SetReadyRead(QByteArray,int)));
        connect(TYaesu, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        connect(this, SIGNAL(EmitOnOffCatCommand(bool,int,int)), TYaesu, SLOT(SetOnOffCatCommand(bool,int,int)));
        have_read_data_rts_on = 3; //2.56 need to be here
        s_active_model_id = rstruc.model;
        s_active_fact_id = YAESU_ID;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(false);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TYaesu, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == ICOM_ID)
    {
        Icom *TIcom = new Icom(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TIcom, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TIcom, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TIcom, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TIcom, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TIcom, SLOT(SetReadyRead(QByteArray,int)));
        connect(TIcom, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        //connect(this, SIGNAL(EmitOnOffCatCommand(bool,int,int)), TIcom, SLOT(SetOnOffCatCommand(bool,int,int)));
        have_read_data_rts_on = 3; //2.56 need to be here
        //s_active_model_id = lst_outb.at(2).toInt();
        //s_active_fact_id = ICOM_ID;
        s_active_model_id = rstruc.model;//IC8TEM mod: needed by the Power & SWR window (was never set for Icom)
        s_active_fact_id = ICOM_ID;//IC8TEM mod
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(false);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TIcom, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == KENWOOD_ID)
    {
        Kenwood *TKenwood = new Kenwood(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TKenwood, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TKenwood, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TKenwood, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TKenwood, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TKenwood, SLOT(SetReadyRead(QByteArray,int)));
        connect(TKenwood, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        connect(this, SIGNAL(EmitOnOffCatCommand(bool,int,int)), TKenwood, SLOT(SetOnOffCatCommand(bool,int,int)));
        have_read_data_rts_on = 3;//2.56 need to be here
        /*if 	(rstruc.name=="Kenwood TS-940S")   have_read_data_rts_on = 2;
              else if (rstruc.name=="Kenwood TS-950SDX") have_read_data_rts_on = 1;
              else if (rstruc.name=="Kenwood TS-990s")   have_read_data_rts_on = 0;
              else have_read_data_rts_on = 3;*/
        s_active_model_id = rstruc.model;
        s_active_fact_id = KENWOOD_ID;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(true);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TKenwood, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == ELECRAFT_ID)
    {
        Elecraft *TElecraft = new Elecraft(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TElecraft, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TElecraft, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TElecraft, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TElecraft, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TElecraft, SLOT(SetReadyRead(QByteArray,int)));
        connect(TElecraft, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        have_read_data_rts_on = 3;
        s_active_model_id = rstruc.model;//IC8TEM v29: needed by the Power & SWR window (was never set for Elecraft)
        s_active_fact_id = ELECRAFT_ID;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(false);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TElecraft, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == TENTEC_ID)
    {
        Tentec *TTentec = new Tentec(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TTentec, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TTentec, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TTentec, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TTentec, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TTentec, SLOT(SetReadyRead(QByteArray,int)));
        connect(TTentec, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        have_read_data_rts_on = 3;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(false);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TTentec, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == ALINCO_ID)
    {
        Alinco *TAlinco = new Alinco(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TAlinco, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TAlinco, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TAlinco, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TAlinco, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TAlinco, SLOT(SetReadyRead(QByteArray,int)));
        connect(TAlinco, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        have_read_data_rts_on = 3;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(false);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TAlinco, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == JRC_ID)
    {
        Jrc *TJrc = new Jrc(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TJrc, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TJrc, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TJrc, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TJrc, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TJrc, SLOT(SetReadyRead(QByteArray,int)));
        connect(TJrc, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        have_read_data_rts_on = 3;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(false);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TJrc, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == DRAKE_ID)
    {
        Drake *TDrake = new Drake(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TDrake, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TDrake, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TDrake, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TDrake, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TDrake, SLOT(SetReadyRead(QByteArray,int)));
        connect(TDrake, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        have_read_data_rts_on = 3;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(false);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TDrake, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == ICMARINE_ID)
    {
        Icmarine *TIcmarine = new Icmarine(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TIcmarine, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TIcmarine, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TIcmarine, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TIcmarine, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TIcmarine, SLOT(SetReadyRead(QByteArray,int)));
        connect(TIcmarine, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        have_read_data_rts_on = 3;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(false);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TIcmarine, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == PCR_ID)
    {
        Pcr *TPcr = new Pcr(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TPcr, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TPcr, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        //connect(TPcr, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TPcr, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        //connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TPcr, SLOT(SetReadyRead(QByteArray,int)));
        //connect(TPcr, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        have_read_data_rts_on = 3;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(false);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TPcr, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == RACAL_ID)
    {
        Racal *TRacal = new Racal(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TRacal, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TRacal, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TRacal, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TRacal, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TRacal, SLOT(SetReadyRead(QByteArray,int)));
        connect(TRacal, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        have_read_data_rts_on = 3;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(false);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TRacal, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
////// omnirig /////////////
#if defined _WIN32_
    else if (rstruc.facid == OMNIRIGHV_ID)
    {
        OmniRigHV *TOmniRigHV = new OmniRigHV(rstruc.model);

        connect(TOmniRigHV, SIGNAL(EmitOmniRigActive(bool,QString)), this, SIGNAL(EmitOmniRigActive(bool,QString)));
        connect(this, SIGNAL(EmitOmniRigPttMethod(bool*)), TOmniRigHV, SLOT(SetOmniRigPttMethod(bool*)));
        connect(TOmniRigHV, SIGNAL(EmitOmniRigChanged()), this, SLOT(SetOmniRigChanged()));

        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TOmniRigHV, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TOmniRigHV, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TOmniRigHV, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TOmniRigHV, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        //connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TOmniRigHV, SLOT(SetReadyRead(QByteArray,int)));
        connect(TOmniRigHV, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        have_read_data_rts_on = 3;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(false);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TOmniRigHV, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = TOmniRigHV->InitOmniRigHV();
        if (!f_rig_activ)
        {
            //qDebug()<<"OmniExcept DeleteLater"<<f1;
            TOmniRigHV->deleteLater();
        }
    }
#endif
////// omnirig end /////////
    else if (rstruc.facid == NETWORK_ID && IS_HAMLIB(rstruc.model))//IC8TEM Icom IC-7300/7610/9700 (Hamlib)
    {
        HamlibRig *THamlib = new HamlibRig(rstruc.model);
        connect(this, SIGNAL(EmitNetSP(QString)), THamlib, SLOT(ConnectNet(QString)));
        connect(THamlib, SIGNAL(EmitNetConnInfo(QString,bool,bool)), this, SIGNAL(EmitNetConnInfo(QString,bool,bool)));
        connect(THamlib, SIGNAL(EmitFullRigInfo(QString)), this, SIGNAL(EmitFullRigInfo(QString)));
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), THamlib, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(THamlib, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(this, SIGNAL(DesleteRig()), THamlib, SLOT(deleteLater()));
        connect(THamlib, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        connect(THamlib, SIGNAL(EmitCmdSent()), this, SLOT(RestartPolling()));//IC8TEM v26
        connect(this, SIGNAL(EmitOnOffCatCommand(bool,int,int)), THamlib, SLOT(SetOnOffCatCommand(bool,int,int)));
        have_read_data_rts_on = 3;
        s_active_model_id = rstruc.model;
        s_active_fact_id = NETWORK_ID;
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(THamlib, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == NETWORK_ID && IS_ICOMLAN(rstruc.model))//IC8TEM Icom LAN
    {
        IcomLanRig *TIcomLan = new IcomLanRig(rstruc.model);
        connect(this, SIGNAL(EmitNetSP(QString)), TIcomLan, SLOT(ConnectNet(QString)));
        connect(TIcomLan, SIGNAL(EmitNetConnInfo(QString,bool,bool)), this, SIGNAL(EmitNetConnInfo(QString,bool,bool)));
        connect(TIcomLan, SIGNAL(EmitFullRigInfo(QString)), this, SIGNAL(EmitFullRigInfo(QString)));
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TIcomLan, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TIcomLan, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(this, SIGNAL(DesleteRig()), TIcomLan, SLOT(deleteLater()));
        connect(TIcomLan, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        connect(TIcomLan, SIGNAL(EmitCmdSent()), this, SLOT(RestartPolling()));//IC8TEM v26
        connect(this, SIGNAL(EmitOnOffCatCommand(bool,int,int)), TIcomLan, SLOT(SetOnOffCatCommand(bool,int,int)));
        have_read_data_rts_on = 3;
        s_active_model_id = rstruc.model;
        s_active_fact_id = NETWORK_ID;
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TIcomLan, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == NETWORK_ID)
    {
        Network *TNetwork = new Network(rstruc.model);

        connect(this, SIGNAL(EmitNetSP(QString)), TNetwork, SLOT(ConnectNet(QString)));
        connect(TNetwork, SIGNAL(EmitNetConnInfo(QString,bool,bool)), this, SIGNAL(EmitNetConnInfo(QString,bool,bool)));
        connect(TNetwork,SIGNAL(EmitFullRigInfo(QString)),this,SIGNAL(EmitFullRigInfo(QString)));//2.76.1 for pskreporter
        //connect(TNetwork,SIGNAL(EmitFlexSliceReady()),this,SIGNAL(EmitFlexSliceReady()));//flex native vita-49

        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TNetwork, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TNetwork, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TNetwork, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TNetwork, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        //connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TNetwork, SLOT(SetReadyRead(QByteArray,int)));
        connect(TNetwork, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        //tci

        connect(this, SIGNAL(EmitOnOffCatCommand(bool,int,int)), TNetwork, SLOT(SetOnOffCatCommand(bool,int,int)));
        have_read_data_rts_on = 3; //2.56 need to be here
        s_active_model_id = rstruc.model;
        s_active_fact_id = NETWORK_ID;
        connect(this, SIGNAL(EmitTciSelect(int,int,bool)), TNetwork, SLOT(SetTciSelect(int,int,bool)));//2.59 tci
        SetTciSelect(tci_select,vita_rx,vita_tx);//2.59 tci
        if (rstruc.name.contains("FlexRadio SmartSDR Slice ")) f_rig_active_never_stop = true;//2.76.1 Flex Slice

        //connect(this, SIGNAL(EmitMode(int)), TNetwork, SLOT(SetMode(int)));//tci
        //SetMode(s_mode);//tci
        //end tci
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(true);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TNetwork, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else if (rstruc.facid == SDRS_ID)
    {
        SDRs *TSDRs = new SDRs(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TSDRs, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TSDRs, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TSDRs, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TSDRs, SLOT(deleteLater()));

        ////////////////////////////////////////////////////////new read com
        connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TSDRs, SLOT(SetReadyRead(QByteArray,int)));
        connect(TSDRs, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        connect(this, SIGNAL(EmitOnOffCatCommand(bool,int,int)), TSDRs, SLOT(SetOnOffCatCommand(bool,int,int)));
        have_read_data_rts_on = 3; //2.56 need to be here
        s_active_model_id = rstruc.model;
        s_active_fact_id = SDRS_ID;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(true);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TSDRs, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;

    }
    else if (rstruc.facid == MITS_ID)
    {
        Mits *TMits = new Mits(rstruc.model);
        connect(this, SIGNAL(SetCmd(CmdID,ptt_t,QString)), TMits, SLOT(SetCmd(CmdID,ptt_t,QString)));
        connect(TMits, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        connect(TMits, SIGNAL(EmitWriteCmd(char*,int)), this, SLOT(SetWriteCmd(char*,int)));
        connect(this, SIGNAL(DesleteRig()), TMits, SLOT(deleteLater()));//deleteLater()

        ////////////////////////////////////////////////////////new read com
        connect(this, SIGNAL(EmitReadyRead(QByteArray,int)), TMits, SLOT(SetReadyRead(QByteArray,int)));
        connect(TMits, SIGNAL(EmitReadedInfo(CmdID,QString)), this, SLOT(SetReadedInfo(CmdID,QString)));
        connect(this, SIGNAL(EmitOnOffCatCommand(bool,int,int)), TMits, SLOT(SetOnOffCatCommand(bool,int,int)));
        connect(TMits, SIGNAL(EmitPttDtr(bool)), this, SIGNAL(EmitPttDtr(bool)));// sea-235
        if (rstruc.name=="SEA-235") have_read_data_rts_on = 1; //2.73 need to be here
        else have_read_data_rts_on = 3;
        s_active_model_id = rstruc.model;
        s_active_fact_id = MITS_ID;
        ////////////////////////////////////////////////////////end new read com

        //SetPttCaps(false);
        emit SetCmd(GET_SETT,RIG_PTT_OFF,"");
        disconnect(TMits, SIGNAL(EmitRigSet(RigSet)), this, SLOT(SetRigSet(RigSet)));
        f_rig_activ = true;
    }
    else
    {
        SetPttCaps(false);//Reset if None
        RigSet sett;
        sett.ptt_type = RIG_PTT_NONE;
        sett.port_type = RIG_PORT_NONE;
        sett.serial_rate_min = 4800;
        sett.serial_rate_max = 4800;
        sett.serial_data_bits = 8;
        sett.serial_stop_bits = 2;
        sett.serial_parity = PAR_NONE;
        sett.serial_flow_type = FLOW_OFF;
        sett.write_delay = 0;
        sett.post_write_delay = 100;
        sett.timeout = 100;
        sett.retry = 0;
        SetRigSet(sett);
    }
}
void HvRigCat::SetRigSet(RigSet sett)
{
    //if(f_rig_activ)
    //if (sett.ptt_type == RIG_PTT_NONE || sett.ptt_type == RIG_PTT_RIG)
    //SetPttCaps(false);
    if (sett.ptt_type == RIG_PTT_RIG_MICDATA) SetPttCaps(true);
    else SetPttCaps(false);
    s_port_type = sett.port_type;//imporetent to be here
    int net_model_id = -100;//2.76.1
    if (s_active_fact_id == NETWORK_ID) net_model_id = s_active_model_id;
    emit EmitSetRigSet(sett,have_read_data_rts_on,net_model_id); //qDebug()<<"RIG_SETT"<<sett.name;
}
void HvRigCat::SetPttCaps(bool f)
{
    //qDebug()<<"RIG_ComboBox"<<f;
    if (f)
    {
        rb_ptt_mic->setEnabled(true);
        rb_ptt_data->setEnabled(true);
    }
    else
    {
        rb_ptt->setChecked(true);
        rb_ptt_mic->setEnabled(false);
        rb_ptt_data->setEnabled(false);
    }
}
void HvRigCat::PttChanget(bool)
{
    if (rb_ptt->isChecked()) s_ptt_on_type = RIG_PTT_ON;
    if (rb_ptt_mic->isChecked()) s_ptt_on_type = RIG_PTT_ON_MIC;
    if (rb_ptt_data->isChecked()) s_ptt_on_type = RIG_PTT_ON_DATA;
}
/*void HvRigCat::InsertItemModel(QStringList list)
{
    if (!list.isEmpty())
    {
        QList<QStandardItem *> qlsi;
        for (QStringList::iterator it =  list.begin(); it != list.end(); ++it)
        {
            QStandardItem *item = new QStandardItem(QString(*it));
            //item->setEditable(false);
            qlsi.append(item);
        }
        //model.appendRow(qlsi);
        model.insertRow(0, qlsi);// za tozi slu4ei e taka HV
        //this->setCurrentIndex(model.index(model.rowCount()-1,0));
    }
    //qDebug()<<list.count();
}*/
void HvRigCat::SetRigName(QString s)//from read settings
{
    if (s.startsWith("Icom IC-") && s.endsWith(" LAN")) s = "Icom LAN";//IC8TEM ic8tem.4-.7 names
    int index = RigsBoxs->findText(s, Qt::MatchCaseSensitive);
    if (index >= 0)
    {
        RigsBoxs->setCurrentIndex(index);
        SetRig(index);//tova opravia i SetPttCaps();
    }
}
void HvRigCat::SetPttCatType(QString s)
{
    QStringList ls=s.split("#");
    ls<<"1000"<<"0";
    if (ls.at(0)=="0") rb_ptt->setChecked(true);
    if (ls.at(0)=="1") rb_ptt_mic->setChecked(true);
    if (ls.at(0)=="2") rb_ptt_data->setChecked(true);
    int ip = ls.at(1).toInt();
    if (ip>199 && ip<1001) sb_pollint->setValue(ip);
}
void HvRigCat::SetWriteCmd(char*data,int size)
{
    //QByteArray array(data, size);
    //qDebug()<<"WriteCmd="<<(QString(array.toHex()))<<size;
    polling_timer->stop();//block timer
    emit EmitWriteCmd(data,size);
    //if (s_port_type == RIG_PORT_SERIAL && s_port_poen) // && s_cb_read_data_rts_on
    if ((s_port_type == RIG_PORT_SERIAL || s_port_type == RIG_PORT_NETWORK) && s_port_poen)
    {
        polling_timer->start(sb_pollint->value());
    }
}
void HvRigCat::RestartPolling()
{
    // IC8TEM: set_freq, set_mode and set_ptt stop the polling; the drivers that write to the
    // port of MSHV start it again in SetWriteCmd. Hamlib and Icom LAN do not write through MSHV,
    // so the frequency and the mode were read no more after the first command (band button, F, TX).
    if ((s_port_type == RIG_PORT_SERIAL || s_port_type == RIG_PORT_NETWORK) && s_port_poen)
        polling_timer->start(sb_pollint->value());
}
void HvRigCat::StartPttTimer()
{
    //qDebug()<<"set_ptt_timer"<<QTime::currentTime().toString("ss:zzz")<<s_f_ptt;
    if (s_f_ptt) emit SetCmd(SET_PTT,s_ptt_on_type,"");
    else 		 emit SetCmd(SET_PTT,RIG_PTT_OFF,"");
    tx_rx_timer->stop();
}
void HvRigCat::set_ptt(bool f, bool imd)//2.38  immediately
{
    //if (s_port_type == RIG_PORT_SERIAL && s_port_poen)
    if ((s_port_type == RIG_PORT_SERIAL || s_port_type == RIG_PORT_NETWORK) && s_port_poen)
    {
        polling_timer->stop();
        s_f_ptt = f;
        if (imd) //2.38 immediately
            StartPttTimer();
        else
            tx_rx_timer->start(50);//2.17=50ms 1.66=100ms wait to stop poling timer tx start at 250ms in MsPlayerHV
    }
}
void HvRigCat::set_mode(QString str)
{
    //if (s_port_type == RIG_PORT_SERIAL && s_port_poen)
    if ((s_port_type == RIG_PORT_SERIAL || s_port_type == RIG_PORT_NETWORK) && s_port_poen)
    {
        polling_timer->stop();
        usleep(50000);
        //if (s_port_type == RIG_PORT_SERIAL && s_port_poen)
        emit SetCmd(SET_MODE,RIG_PTT_OFF,str);//RIG_PTT_OFF fictive
    }
}
void HvRigCat::set_freq(QString sfreq)
{
    //qDebug()<<"set_freq"<<s_port_poen<<s_port_type<<sfreq;
    //if (s_port_type == RIG_PORT_SERIAL && s_port_poen)
    if ((s_port_type == RIG_PORT_SERIAL || s_port_type == RIG_PORT_NETWORK) && s_port_poen)
    {
        polling_timer->stop();
        usleep(50000);
        //if (s_port_type == RIG_PORT_SERIAL && s_port_poen)
        emit SetCmd(SET_FREQ,RIG_PTT_OFF,sfreq);//RIG_PTT_OFF fictive
    }
}
void HvRigCat::SetReadyRead(QByteArray a,int i)
{
    emit EmitReadyRead(a,i);
    if (PwrTextFamily() > 0)//IC8TEM: answers to the Power & SWR commands (the driver ignores them)
    {
        pwr_rx_buf.append(a.left(i));
        int p;
        while ((p = pwr_rx_buf.indexOf(';')) >= 0)
        {
            QByteArray tok = pwr_rx_buf.left(p).trimmed();
            pwr_rx_buf.remove(0,p+1);
            if (!tok.isEmpty()) PwrTextParse(tok);
        }
        if (pwr_rx_buf.size() > 512) pwr_rx_buf.clear();
    }
}
void HvRigCat::SetReadedInfo(CmdID i,QString str)
{
    if (i==GET_FREQ)
    {
        if (c_rig_cat_active_and_read == POLLRST) emit EmitCatAactiveAndRead(true,true);//2.53 true to TxWidget
        c_rig_cat_active_and_read = 0;
        emit EmitGetedFreq(str);
    }
    else if (i==GET_MODE) emit EmitGetedMode(str);
    else if (i==READ_RFPOWER || i==READ_PO || i==READ_SWR || i==READ_ALC || i==LOG_TX || i==LOG_RX) emit EmitPwrInfo((int)i,str);//IC8TEM
    //qDebug()<<"READ INFO="<<i<<str;
}
bool HvRigCat::PwrCapable()//IC8TEM Power & SWR: Icom CI-V (USB) or Icom LAN, CAT running
{
    if (!s_port_poen || !f_rig_activ) return false;
    return (s_active_fact_id==ICOM_ID || (s_active_fact_id==NETWORK_ID && (IS_ICOMLAN(s_active_model_id) || IS_HAMLIB(s_active_model_id)))
            || PwrTextFamily() > 0
            || (s_active_fact_id==NETWORK_ID && (s_active_model_id==3 || s_active_model_id==4 || s_active_model_id==6)));//TCI, FlexRadio 6xxx
}
QString HvRigCat::PwrStatus()//IC8TEM Power & SWR: why it is (not) available
{
    QString rig = get_rig_name();
    if (rig.isEmpty() || rig=="None") rig = "-";
    QString cat = s_port_poen ? "CAT ON" : "CAT OFF";
    QString kind;
    if (s_active_fact_id==ICOM_ID) kind = "Icom CI-V";
    else if (s_active_fact_id==NETWORK_ID && IS_ICOMLAN(s_active_model_id)) kind = "Icom LAN";
    else if (s_active_fact_id==NETWORK_ID && IS_HAMLIB(s_active_model_id)) kind = "Hamlib";
    else if (PwrTextFamily()==1 || PwrTextFamily()==8) kind = "Yaesu CAT";
    else if (PwrTextFamily()==2 || PwrTextFamily()==3 || PwrTextFamily()==6 || PwrTextFamily()==7) kind = "Kenwood CAT";
    else if (PwrTextFamily()==4 || PwrTextFamily()==5) kind = "Elecraft CAT";
    else if (PwrTextFamily()==9) kind = "PowerSDR/Thetis CAT";
    else if (s_active_fact_id==NETWORK_ID && (s_active_model_id==3 || s_active_model_id==4)) kind = "TCI";
    else if (s_active_fact_id==NETWORK_ID && s_active_model_id==6) kind = "FlexRadio";
    else if (s_active_fact_id==11) kind = "OmniRig";//OMNIRIGHV_ID (Windows only header)
    else if (s_active_fact_id==NETWORK_ID) kind = "Network (TCI/Flex/Hamlib...)";
    else kind = "CAT";
    return rig+"  |  "+kind+"  |  "+cat;
}
void HvRigCat::SendPwrCmd(int id,QString str)//IC8TEM
{
    if (!PwrCapable()) return;
    if (id!=SET_RFPOWER && id!=GET_RFPOWER && id!=GET_METERS) return;
    if (id==GET_METERS) pwr_tx = (str=="tx" || s_f_ptt);//IC8TEM v29: TX also with PTT by RTS/DTR (window tells it)
    if (PwrTextFamily() > 0)
    {
        PwrTextCmd(id,str);
        return;
    }
    emit SetCmd((CmdID)id,RIG_PTT_OFF,str);
}
//IC8TEM ---------------------------------------------------------------- text CAT radios
// Commands and meter scales as in Hamlib 4.6 (newcat.c, ft450, ft9000, ts590.c, ts2000.c, ts870s.c, ts850.c,
// ts890s.c, ts990s.c, k3.c, flex6xxx.c):
//  1 Yaesu NEWCAT     PCnnn (W) / RM5 Po / RM6 SWR / RM4 ALC (0-255)
//  8 Yaesu FTDX-9000  PCnnn (W, 200 W) / RM08 Po / RM09 SWR / RM07 ALC
//  2 Kenwood TS-590, TS-480 (and clones)  PCnnn / RM; -> RM1 SWR, RM2 COMP, RM3 ALC (0-30) / SM0 = Po while transmitting
//  3 Kenwood TS-2000 (and SDR Console)    as 2, RM; gives only the meter chosen on the radio
//  6 Kenwood TS-870S, TS-850, TS-690S, TS-450S  RM; -> the meter chosen on the radio (RM1 SWR, RM3 ALC, 0-30);
//    PCnnn only on TS-870S (TS-450S: read only; TS-850, TS-690S: no power by CAT)
//  7 Kenwood TS-890S, TS-990S  RM11; RM21; (ALC and SWR readings on) RM; -> RM1 ALC, RM2 SWR (0-70); TS-890S: SM; = Po in TX
//  4 Elecraft K3      PCnnn (W) / TM; BG; -> BGnnT (TM0 power 0-10+, TM1 ALC 0-7) / SWnnn = SWR x10
//  5 Elecraft K4      PCnnnH / TM; -> ALC, CMP, FWD W, SWR x10
//  9 PowerSDR, Thetis ZZPCnnn (%) / ZZRM5 Po (W) / ZZRM8 SWR
QString HvRigCat::PwrModelName()// the model without the maker: the list shows "Yaesu FT-991", the table "FT-991"
{
    if (!f_rig_activ || s_active_model_id < 0) return QString();
    if (s_active_fact_id==YAESU_ID && s_active_model_id < YAESU_COUNT) return QString(rigs_yeasu[s_active_model_id].name);
    if (s_active_fact_id==KENWOOD_ID && s_active_model_id < KENWOOD_COUNT) return QString(rigs_kenwood[s_active_model_id].name);
    if (s_active_fact_id==ELECRAFT_ID && s_active_model_id < ELECRAFT_COUNT) return QString(rigs_elecraft[s_active_model_id].name);
    if (s_active_fact_id==SDRS_ID && s_active_model_id < SDRS_COUNT) return QString(rigs_sdrs[s_active_model_id].name);
    return QString();
}
int HvRigCat::PwrTextFamily()
{
    QString n = PwrModelName();
    if (n.isEmpty()) return 0;
    if (s_active_fact_id==YAESU_ID)
    {
        if (n=="FT-991" || n=="FT-710" || n=="FT-DX10" || n=="FT-DX101" || n=="FT-891" || n=="FT-DX1200" ||
                n=="FT-DX3000" || n=="FT-DX5000" || n=="FT-950" || n=="FTX-1" || n=="FT-2000" ||
                n=="FT-450" || n=="FT-450D") return 1;
        if (n=="FTDX-9000") return 8;
        return 0;
    }
    if (s_active_fact_id==KENWOOD_ID)
    {
        if (n=="TS-590S" || n=="TS-590SG" || n=="TS-480" || n=="BG2FX FX4/C/CR/L" || n=="Hilberling PT-8000A") return 2;
        if (n=="TS-2000" || n=="SDR Radio SDRConsole") return 3;
        if (n=="TS-870S" || n=="TS-850" || n=="TS-690S" || n=="TS-450S") return 6;
        if (n=="TS-890S" || n=="TS-990s") return 7;
        return 0;
    }
    if (s_active_fact_id==ELECRAFT_ID && n=="K3/KX3") return 4;
    if (s_active_fact_id==ELECRAFT_ID && n=="K4") return 5;
    if (s_active_fact_id==SDRS_ID && (n=="FlexRadio/Apache PowerSDR" || n=="Thetis")) return 9;
    return 0;
}
static bool PwrIs200W(const QString &n)// PC in watts up to 200
{
    return (n=="TS-990s" || n=="FTDX-9000");
}
void HvRigCat::PwrTextCmd(int id,const QString &str)
{
    int fam = PwrTextFamily();
    QString n = PwrModelName();
    QString c;
    if (id==SET_RFPOWER)
    {
        int pct = (int)(str.toInt()*100.0/255.0+0.5);
        if (pct > 100) pct = 100;
        if (pct < 0) pct = 0;
        if (fam==9) c = QString("ZZPC%1;").arg(pct,3,10,QChar('0'));
        else if (fam==6 && n!="TS-870S") return;//TS-850, TS-690S, TS-450S: no power setting by CAT
        else
        {
            if (fam!=4 && fam!=5 && pct < 5) pct = 5;//Yaesu and Kenwood: 5..100 W
            if (fam==5 && pct < 1) pct = 1;
            int w = PwrIs200W(n) ? pct*2 : pct;
            if (fam==5) c = QString("PC%1H;").arg(pct,3,10,QChar('0'));//K4: H = 1 W units
            else c = QString("PC%1;").arg(w,3,10,QChar('0'));
        }
    }
    else if (id==GET_RFPOWER)
    {
        if (fam==9) c = "ZZPC;";
        else if (fam==6 && n!="TS-870S" && n!="TS-450S") return;
        else c = "PC;";
    }
    else if (id==GET_METERS)
    {
        if (fam==1) c = "RM5;RM6;RM4;";
        else if (fam==2 || fam==3) c = pwr_tx ? "RM;SM0;" : "RM;";
        else if (fam==4) c = "TM;BG;SW;";
        else if (fam==5) c = "TM;";//K4: ALC, CMP, forward power, SWR in one answer
        else if (fam==6) c = "RM;";
        else if (fam==7) c = (pwr_tx && n=="TS-890S") ? "RM11;RM21;RM;SM;" : "RM11;RM21;RM;";
        else if (fam==8) c = "RM08;RM09;RM07;";
        else if (fam==9 && pwr_tx) c = "ZZRM5;ZZRM8;";
    }
    if (c.isEmpty()) return;
    emit EmitPwrInfo((int)LOG_TX,c);
    pwr_tx_buf = c.toLatin1();
    SetWriteCmd(pwr_tx_buf.data(),pwr_tx_buf.size());
}
static double PwrCal(double raw,const double (*t)[2],int n)//IC8TEM linear interpolation
{
    if (raw <= t[0][0]) return t[0][1];
    for (int i = 1; i < n; ++i)
        if (raw <= t[i][0]) return t[i-1][1] + (t[i][1]-t[i-1][1])*(raw-t[i-1][0])/(t[i][0]-t[i-1][0]);
    return t[n-1][1];
}
static double PwrFirstNumber(const QString &s)// "ZZRM5 50 W" -> 50, "ZZRM81.5" -> 1.5
{
    int i = 0;
    while (i < s.length() && !s.at(i).isDigit()) i++;
    int j = i;
    while (j < s.length() && (s.at(j).isDigit() || s.at(j)==QChar('.'))) j++;
    return s.mid(i,j-i).toDouble();
}
void HvRigCat::PwrTextParse(const QByteArray &tok)
{
    int fam = PwrTextFamily();
    QString n = PwrModelName();
    QString t = QString::fromLatin1(tok);
    bool mine = false;
    if (fam==9) mine = t.startsWith("ZZPC") || t.startsWith("ZZRM5") || t.startsWith("ZZRM8");
    else
    {
        mine = t.startsWith("PC") || t.startsWith("RM");
        if ((fam==2 || fam==3) && t.startsWith("SM0")) mine = true;
        if (fam==7 && n=="TS-890S" && t.startsWith("SM")) mine = true;
        if (fam==4 && (t.startsWith("BG") || t.startsWith("SW") || t.startsWith("TM"))) mine = true;
        if (fam==5 && t.startsWith("TM")) mine = true;
    }
    if (!mine) return;
    emit EmitPwrInfo((int)LOG_RX,t+";");
    if (fam==9)
    {
        if (t.startsWith("ZZPC"))
            emit EmitPwrInfo((int)READ_RFPOWER,"f:"+QString::number(qBound(0.0,PwrFirstNumber(t.mid(4))/100.0,1.0),'f',4));
        else if (!pwr_tx) return;
        else if (t.startsWith("ZZRM5")) emit EmitPwrInfo((int)READ_PO,"w:"+QString::number(PwrFirstNumber(t.mid(5)),'f',1));
        else if (t.startsWith("ZZRM8"))
        {
            double swr = PwrFirstNumber(t.mid(5));
            emit EmitPwrInfo((int)READ_SWR,"f:"+QString::number(swr < 1.0 ? 1.0 : swr,'f',3));
        }
        return;
    }
    if (t.startsWith("PC") && t.length() >= 5)
    {
        double w = t.mid(2,3).toInt();
        if ((fam==4 || fam==5) && t.length() >= 6 && (t.at(5)=='0' || t.at(5)=='L')) w /= 10.0;//K3 PCnnnx x=0 / K4 PCnnnL: tenths of watt
        if (PwrIs200W(n)) w /= 2.0;
        emit EmitPwrInfo((int)READ_RFPOWER,"f:"+QString::number(qMin(w/100.0,1.0),'f',4));
        return;
    }
    if (fam==1 && t.startsWith("RM") && t.length() >= 6)
    {
        QChar m = t.at(2);
        double raw = t.mid(3,3).toInt();
        if (m=='5')//Po, watts
        {
            static const double c991[][2] = {{0,0},{10,0.8},{50,8},{100,26},{150,54},{200,92},{250,140}};
            static const double c10[][2]  = {{0,0},{27,5},{94,25},{147,50},{176,75},{205,100}};
            static const double cdef[][2] = {{0,0},{148,50},{255,100}};
            double w;
            if (n=="FT-991" || n=="FT-450" || n=="FT-450D") w = PwrCal(raw,c991,7);
            else if (n=="FT-DX10" || n=="FT-710" || n=="FT-DX101") w = PwrCal(raw,c10,6);
            else w = PwrCal(raw,cdef,3);
            emit EmitPwrInfo((int)READ_PO,"f:"+QString::number(qMin(w/100.0,1.0),'f',4));
        }
        else if (m=='6')//SWR
        {
            static const double s10[][2]  = {{0,1.0},{26,1.2},{52,1.5},{89,2.0},{126,3.0},{173,4.0},{236,5.0},{255,25.0}};
            static const double sdef[][2] = {{0,1.0},{12,1.0},{39,1.35},{65,1.5},{89,2.0},{242,5.0}};
            double swr = (n=="FT-DX10" || n=="FT-710" || n=="FT-DX101") ? PwrCal(raw,s10,8) : PwrCal(raw,sdef,6);
            emit EmitPwrInfo((int)READ_SWR,"f:"+QString::number(swr,'f',3));
        }
        else if (m=='4')//ALC: 1.0 = half scale
            emit EmitPwrInfo((int)READ_ALC,"f:"+QString::number(raw/128.0,'f',4));
        return;
    }
    if (fam==8 && t.startsWith("RM0") && t.length() >= 7)//FTDX-9000 RM08nnn RM09nnn RM07nnn
    {
        QString m = t.mid(2,2);
        double raw = t.mid(4,3).toInt();
        if (m=="08")
        {
            static const double c9000[][2] = {{0,0},{35,10},{53,20},{80,40},{97,60},{119,80},{137,100},{154,120},
                                              {167,140},{177,160},{188,180},{197,200}};
            emit EmitPwrInfo((int)READ_PO,"w:"+QString::number(PwrCal(raw,c9000,12),'f',1));
        }
        else if (m=="09")
        {
            static const double sdef[][2] = {{0,1.0},{12,1.0},{39,1.35},{65,1.5},{89,2.0},{242,5.0}};
            emit EmitPwrInfo((int)READ_SWR,"f:"+QString::number(PwrCal(raw,sdef,6),'f',3));
        }
        else if (m=="07") emit EmitPwrInfo((int)READ_ALC,"f:"+QString::number(raw/128.0,'f',4));
        return;
    }
    if ((fam==2 || fam==3) && t.startsWith("RM") && t.length() >= 4)
    {
        QChar m = t.at(2);
        double raw = t.mid(3).toInt();
        if (m=='1')
        {
            static const double s590[][2] = {{0,1.0},{6,1.5},{12,2.0},{18,3.0},{30,10.0}};
            emit EmitPwrInfo((int)READ_SWR,"f:"+QString::number(PwrCal(raw,s590,5),'f',3));
        }
        else if (m=='3') emit EmitPwrInfo((int)READ_ALC,"f:"+QString::number(raw/(fam==3 ? 20.0 : 30.0),'f',4));
        return;
    }
    if (fam==6 && t.startsWith("RM") && t.length() >= 4)//TS-870S, TS-850, TS-690S, TS-450S: 0-30
    {
        QChar m = t.at(2);
        double raw = t.mid(3).toInt();
        if (m=='1')
        {
            double swr = (raw >= 27.0) ? 10.0 : 60.0/(30.0-raw)-1.0;
            emit EmitPwrInfo((int)READ_SWR,"f:"+QString::number(swr < 1.0 ? 1.0 : swr,'f',3));
        }
        else if (m=='3') emit EmitPwrInfo((int)READ_ALC,"f:"+QString::number(raw/30.0,'f',4));
        return;
    }
    if (fam==7 && t.startsWith("RM") && t.length() >= 4)//TS-890S, TS-990S: RM1 ALC, RM2 SWR (0-70)
    {
        QChar m = t.at(2);
        double raw = t.mid(3).toInt();
        if (m=='1') emit EmitPwrInfo((int)READ_ALC,"f:"+QString::number(raw/35.0,'f',4));
        else if (m=='2')
        {
            double swr;
            if (n=="TS-990s")
            {
                static const double s990[][2] = {{0,1.0},{7,1.5},{36,3.0},{43,6.0},{70,10.0}};
                swr = PwrCal(raw,s990,5);
            }
            else if (raw < 12) swr = 1.0 + raw/22.0;
            else if (raw < 24) swr = 1.5 + (raw-11)/24.0;
            else if (raw < 36) swr = 2.0 + (raw-23)/12.0;
            else swr = 3.0 + (raw-35)/6.0;
            emit EmitPwrInfo((int)READ_SWR,"f:"+QString::number(swr,'f',3));
        }
        return;
    }
    if (fam==7 && t.startsWith("SM") && t.length() >= 3)//TS-890S: SMnnnn while transmitting = Po (0-70)
    {
        if (!pwr_tx) return;
        static const double p890[][2] = {{0,0},{5,5},{10,10},{19,25},{35,50},{59,100},{70,150}};
        emit EmitPwrInfo((int)READ_PO,"w:"+QString::number(PwrCal(t.mid(2).toInt(),p890,7),'f',1));
        return;
    }
    if ((fam==2 || fam==3) && t.startsWith("SM0") && t.length() >= 4)
    {
        if (!pwr_tx) return;//S-meter while receiving
        double raw = t.mid(3).toInt();
        double frac;
        if (fam==2)
        {
            static const double p590[][2] = {{0,0},{3,5},{6,10},{8,15},{12,25},{17,50},{30,100}};
            frac = PwrCal(raw,p590,7)/100.0;
        }
        else frac = raw/30.0;
        emit EmitPwrInfo((int)READ_PO,"f:"+QString::number(qMin(frac,1.0),'f',4));
        return;
    }
    if (fam==5 && t.startsWith("TM") && t.length() >= 14)//K4 TMaaabbbcccddd: ALC, CMP, FWD W, SWR x10
    {
        double alc = t.mid(2,3).toInt(), fwd = t.mid(8,3).toInt(), swr = t.mid(11,3).toInt()/10.0;
        emit EmitPwrInfo((int)READ_ALC,"f:"+QString::number(alc/10.0,'f',4));
        emit EmitPwrInfo((int)READ_PO,"w:"+QString::number(fwd,'f',1));
        emit EmitPwrInfo((int)READ_SWR,"f:"+QString::number(swr < 1.0 ? 1.0 : swr,'f',3));
        return;
    }
    if (fam==4)
    {
        if (t.startsWith("TM") && t.length() >= 3) pwr_k3_tm = (t.at(2)=='1') ? 1 : 0;
        else if (t.startsWith("BG") && t.length() >= 5 && t.at(4)=='T')
        {
            double nn = t.mid(2,2).toInt();
            if (pwr_k3_tm==1) emit EmitPwrInfo((int)READ_ALC,"f:"+QString::number(nn/7.0,'f',4));
            else emit EmitPwrInfo((int)READ_PO,"f:"+QString::number(qMin(nn/10.0,1.0),'f',4));
        }
        else if (t.startsWith("SW") && t.length() >= 3)
        {
            double swr = t.mid(2).toInt()/10.0;
            if (swr < 1.0) swr = 1.0;
            emit EmitPwrInfo((int)READ_SWR,"f:"+QString::number(swr,'f',3));
        }
    }
}
void HvRigCat::StartStopPollingTimer(bool popen)/*, bool read_data_rts_on*/
{
    s_port_poen = popen;
    //s_cb_read_data_rts_on = read_data_rts_on;
    //qDebug()<<"s_port_poen="<<s_port_poen<<s_port_type;

    //if (s_port_type == RIG_PORT_SERIAL && s_port_poen)
    if ((s_port_type == RIG_PORT_SERIAL || s_port_type == RIG_PORT_NETWORK) && s_port_poen)
    {
        //qDebug()<<"START TIMER StartStopPollingTimer="<<s_port_type<<popen;
        //if(f_rig_activ)
        emit EmitOnOffCatCommand(true,s_active_model_id,s_active_fact_id);
        polling_timer->start(sb_pollint->value());
    }
    else
    {
        //qDebug()<<"STOP TIMER StartStopPollingTimer="<<s_port_type<<popen;
        polling_timer->stop();
        emit EmitOnOffCatCommand(false,s_active_model_id,s_active_fact_id);
    }
    emit EmitPwrCapable(PwrCapable());//IC8TEM Power & SWR

    bool fl = false;
    if (!popen) fl = true;  //qDebug()<<"UPDATE="<<fl;
    emit EmitCatAactiveAndRead(false,fl);//2.53 false no to TxWidget
    c_rig_cat_active_and_read = POLLRST;//reset 200
}
void HvRigCat::get_freq()
{
    emit SetCmd(GET_FREQ,RIG_PTT_OFF,"");//RIG_PTT_OFF fictive
}
void HvRigCat::get_mode()
{
    emit SetCmd(GET_MODE,RIG_PTT_OFF,"");//RIG_PTT_OFF fictive
} //int t = 1;
void HvRigCat::PollingTimerReadRig()
{
    c_poll_comm++;
    if (c_poll_comm==0)
    {
        get_freq();
        if (c_rig_cat_active_and_read != POLLRST)
        {
            //qDebug()<<f_rig_active_never_stop<<c_rig_cat_active_and_read;//qDebug()<<"TIMER="<<t;  t++;
            if (c_rig_cat_active_and_read > max4min)//2.76.1 2000*95=190000/60000=3,16min, 2000x130=260000/60000=4.3min
            {
                emit EmitCatAactiveAndRead(false,true);//2.53 true to TxWidget
                c_rig_cat_active_and_read = POLLRST;
            }
            else c_rig_cat_active_and_read++;
            if (f_rig_active_never_stop) c_rig_cat_active_and_read = 13;//2.76.1 Flex Slice
        }
    }
    else if (c_poll_comm==1)
    {
        get_mode();
        c_poll_comm = -1;//reset polling commands
    } //qDebug()<<"TIMER WORKKKKKKKKKKKKKKKKKKK=";
}
////////////////////////////////////////////////////////end new read com

