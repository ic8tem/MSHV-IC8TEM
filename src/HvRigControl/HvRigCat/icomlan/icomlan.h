/* MSHV-IC8TEM  Icom LAN (RS-BA1 network protocol) rig control and audio
 *
 * Native network connection to Icom radios with a built-in LAN port
 * (IC-7300MK2, IC-705, IC-9700, IC-7610, IC-905): CAT (CI-V) and 48 kHz
 * audio in both directions over UDP, without RS-BA1, wfview or USB cable.
 *
 * Written for MSHV-IC8TEM by IC8TEM, 2026.  The wire format is not
 * documented by Icom; it is the one published by the open source projects
 * that reverse-engineered it, credited in README.md:
 *   wfview (Elliott Liggett W6EL, Phil Taylor M0VSE and contributors), GPLv3
 *   kappanhang (Norbert Varga HA2NON, nonoo), MIT
 *   AetherSDR contributors (IcomProtocol), GPLv3
 *   PiRO / IC7300MK2_Lan_Splitter (Simon S53ZO and contributors) - notes on
 *   the IC-7300MK2 behaviour (48 kHz LPCM only, big-endian inner sequence).
 * No code was copied from those projects; this is a new C++/Qt implementation.
 *
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef ICOMLAN_H
#define ICOMLAN_H

#include <QWidget>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <QUdpSocket>
#include <QHostAddress>
#include <QElapsedTimer>
#include <QByteArray>
#include <QMap>
#include <QString>
#include "../rigdef.h"

// ---- hooks for the rest of MSHV (defined in icomlan.cpp) ----------------
// Sound device "Icom LAN Output" chosen (Rawplayer::initialize).
extern void _IcomLanSetTxDevice_(bool on);
extern bool _IcomLanTxActive_();
// One TX block from Rawplayer: 4096 ints, stereo, 24-bit scale, at the rate
// given by _SetTxAudioRateTci_() (48000 or 44100).
extern bool _SetTxAudioIcomLan_(int *raw);
extern void _SetTxAudioRateTci_(int rate);
// MsCore: true = the decoder needs 44100 Hz input (FSK441, JTMS, JT6M, ISCAT...).
extern void _IcomLanSetRx44k_(bool on);

class IcomLanStream
{
public:
    IcomLanStream();
    QUdpSocket *sock;
    int role;              // 0 control, 1 CI-V, 2 audio
    quint16 remote_port;
    quint32 local_id;
    quint32 remote_id;
    quint16 tx_seq;
    quint16 ping_seq;
    bool started;
    bool ready;
    bool have_last;
    quint16 last_delivered;
    qint64 gap_since;      // -1 = no gap
    QMap<quint16,QByteArray> reorder;
    QMap<quint16,QByteArray> replay;
    QList<quint16> replay_order;
    qint64 last_rx;
    qint64 last_handshake;
    qint64 last_ping;
    qint64 last_idle;
};

class IcomLanCore : public QObject
{
    Q_OBJECT
public:
    IcomLanCore();
    virtual ~IcomLanCore();

signals:
    void CivReceived(QByteArray);
    void Status(QString,int);   // 0 disconnected, 1 connecting, 2 connected
    void RadioInfo(QString,int); // radio name, CI-V address announced by the radio

public slots:
    void Start(QString host,int port,QString user,QString pass,int civ_port,int audio_port);
    void Stop();
    void SendCiv(QByteArray);
    void SetTxKeyed(bool);

private slots:
    void ReadCtrl();
    void ReadCiv();
    void ReadAudio();
    void Tick();
    void TxTick();
    void Retry();

private:
    QString s_host;
    int s_port;
    int s_civ_port;
    int s_audio_port;
    QString s_user;
    QString s_pass;
    QHostAddress addr;
    bool want;
    bool running;
    bool connected;
    int attempts;
    IcomLanStream st[3];
    QTimer *tick;
    QTimer *txtick;
    QTimer *retry;
    QElapsedTimer clock;
    qint64 started_at;

    quint16 inner_seq;
    quint16 serial_seq;
    quint16 audio_seq;
    quint16 request_id;
    QByteArray auth_id;
    QByteArray radio_id;
    QByteArray radio_name;
    int radio_civ_addr;
    bool authenticated;
    bool requested;
    bool granted;
    bool civ_seen;
    qint64 last_auth;
    qint64 renew_due;
    qint64 last_renew_sent;
    QMap<quint16,qint64> pending_renew;
    qint64 last_serial_open;

    // TX
    bool tx_keyed;
    bool tx_primed;
    qint64 tx_next;
    double tx_frac;
    double tx_x[4];
    int tx_lead;
    // RX 48 -> 44.1 kHz
    double rx_frac;
    double rx_x[4];
    int rx_buf[16384];

    qint64 Now();
    void Begin();
    void CloseAll(bool polite);
    void Fail(QString why);
    bool BindStream(int i,quint16 rport);
    void StartStream(int i);
    void Handshake(int i);
    void Raw(int i,const QByteArray &p);
    void Tracked(int i,QByteArray p);
    void Receive(int i,const QByteArray &p);
    void Deliver(int i,const QByteArray &p);
    void Drain(int i);
    void SkipGap(int i);
    void ControlPayload(const QByteArray &p);
    void AudioPayload(const QByteArray &p);
    quint16 NextInner();
    quint16 SendAuth(int kind);
    void Renew();
    void RequestStreams();
    void OpenSerial();
    void MediaReady();
    void SendAudioFrame(const QByteArray &pcm);
};

class Icom;
class IcomLanRig : public QWidget
{
    Q_OBJECT
public:
    IcomLanRig(int ModelID,QWidget *parent = 0);
    virtual ~IcomLanRig();

signals:
    void EmitRigSet(RigSet);
    void EmitWriteCmd(char*data,int size);
    void EmitReadedInfo(CmdID,QString);
    void EmitNetConnInfo(QString,bool,bool);
    void EmitFullRigInfo(QString);
    // internal
    void IcomCmd(CmdID,ptt_t,QString);
    void IcomRead(QByteArray,int);
    void CoreStart(QString,int,QString,QString,int,int);
    void CoreStop();
    void CoreSendCiv(QByteArray);
    void CoreKey(bool);

public slots:
    void ConnectNet(QString);
    void SetOnOffCatCommand(bool,int,int);
    void SetTciSelect(int,int,bool);

private slots:
    void SetCmd(CmdID,ptt_t,QString);
    void CivFromIcom(char*,int);
    void CivFromCore(QByteArray);
    void CoreStatus(QString,int);
    void CoreRadioInfo(QString,int);
    void Quit();

private:
    int s_ModelID;
    int s_icom_model;
    void MakeIcom(int im);
    Icom *icom;
    IcomLanCore *core;
    QThread thr;
    bool running;
    bool quit_done;
    QString s_host;
    QString s_port;
};
#endif
