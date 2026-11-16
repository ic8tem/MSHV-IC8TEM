/* MSHV
 *
 * By Hrisimir Hristov - LZ2HV
 * May be used under the terms of the GNU General Public License (GPL)
 */

#ifndef ICOM_H
#define ICOM_H

#include <QWidget>
#include "../rigdef.h"

class Icom : public QWidget
{
    Q_OBJECT
public:
    static int ModelFromCivAddr(int addr,const QString &hint);//IC8TEM Icom LAN: model index from the CI-V address
    Icom(int ModelID,QWidget *parent = 0);
    virtual ~Icom();

signals:
    void EmitRigSet(RigSet);
    void EmitWriteCmd(char*data,int size);
    void EmitReadedInfo(CmdID,QString);

public slots:

private slots:
    void SetCmd(CmdID,ptt_t,QString);
    void SetReadyRead(QByteArray,int);
    //void SetOnOffCatCommand(bool,int,int);

private:
    bool s_CmdID_log = false;//IC8TEM CAT log of Power & SWR commands
	bool newSRMod;
    int  s_ModelID;
    QString s_rig_name;
    void make_cmd_all(unsigned char *comand_subcomand,int count);
    void set_ptt(ptt_t);
    int s_CmdID;
    QByteArray s_read_array;
    unsigned char s_model_addr;
    void set_freq(unsigned long long);
    void get_freq();
    void set_mode(QString);
    void get_mode();
    QString GetModeStr(unsigned char,unsigned char);

protected:

};
#endif
