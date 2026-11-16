/* MSHV-IC8TEM  thin wrapper around the Hamlib library (libhamlib-4.dll, loaded at run time)
 *
 * Kept apart from rigdef.h: MSHV and Hamlib use the same names (ptt_t, RIG_PTT_ON, ...).
 * Built with HAVE_HAMLIB (Windows packages); otherwise every call reports "not available".
 *
 * Hamlib (C) the Hamlib Group, GNU LGPL v2.1 or later - https://hamlib.github.io
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef HAMLIB_WRAP_H
#define HAMLIB_WRAP_H

#include <QString>

namespace HlWrap
{
enum Level { L_RFPOWER = 0, L_RFPOWER_METER, L_SWR, L_ALC };
bool Load(QString &err);                         // load the library once
QString Version();
void *Open(int model,const QString &port,int baud,QString &err);
void Close(void *rig);
int GetFreq(void *rig,double *hz);
int SetFreq(void *rig,double hz);
int GetMode(void *rig,QString &mshv_mode);
int SetMode(void *rig,const QString &mshv_mode);
int SetPtt(void *rig,bool on);
int GetLevel(void *rig,int level,float *v);
int SetRfPower(void *rig,float v);              // 0..1
QString Error(int code);
}
#endif
