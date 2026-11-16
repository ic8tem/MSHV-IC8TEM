/* MSHV-IC8TEM  thin wrapper around the Hamlib library. See hamlib_wrap.h.
 * May be used under the terms of the GNU General Public License (GPL)
 */
#include "hamlib_wrap.h"

#if defined HAVE_HAMLIB
#include <QLibrary>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <hamlib/rig.h>

namespace
{
QLibrary *lib = 0;
bool loaded = false;
decltype(&rig_init) p_init = 0;
decltype(&rig_open) p_open = 0;
decltype(&rig_close) p_close = 0;
decltype(&rig_cleanup) p_cleanup = 0;
decltype(&rig_token_lookup) p_token = 0;
decltype(&rig_set_conf) p_set_conf = 0;
decltype(&rig_get_freq) p_get_freq = 0;
decltype(&rig_set_freq) p_set_freq = 0;
decltype(&rig_get_mode) p_get_mode = 0;
decltype(&rig_set_mode) p_set_mode = 0;
decltype(&rig_set_ptt) p_set_ptt = 0;
decltype(&rig_get_level) p_get_level = 0;
decltype(&rig_set_level) p_set_level = 0;
decltype(&rigerror) p_error = 0;
decltype(&rig_set_debug) p_debug = 0;
const char *(*p_version)() = 0;
}

bool HlWrap::Load(QString &err)
{
    if (loaded) return true;
    QString dir = QCoreApplication::applicationDirPath();
    QStringList cand;
    cand << dir+"/hamlib/libhamlib-4.dll" << dir+"/libhamlib-4.dll" << "libhamlib-4.dll";
    QString tried;
    for (int i = 0; i < cand.count(); ++i)
    {
        if (i < 2 && !QFile::exists(cand.at(i))) continue;
        lib = new QLibrary(QDir::toNativeSeparators(cand.at(i)));
        if (lib->load()) break;
        tried += lib->errorString()+" ";
        delete lib;
        lib = 0;
    }
    if (!lib)
    {
        err = "Hamlib: cannot load libhamlib-4.dll "+tried;
        return false;
    }
#define HL_RES(var,name) var = (decltype(var))lib->resolve(name); if (!var) { err = QString("Hamlib: missing function ")+name; return false; }
    HL_RES(p_init,"rig_init");
    HL_RES(p_open,"rig_open");
    HL_RES(p_close,"rig_close");
    HL_RES(p_cleanup,"rig_cleanup");
    HL_RES(p_token,"rig_token_lookup");
    HL_RES(p_set_conf,"rig_set_conf");
    HL_RES(p_get_freq,"rig_get_freq");
    HL_RES(p_set_freq,"rig_set_freq");
    HL_RES(p_get_mode,"rig_get_mode");
    HL_RES(p_set_mode,"rig_set_mode");
    HL_RES(p_set_ptt,"rig_set_ptt");
    HL_RES(p_get_level,"rig_get_level");
    HL_RES(p_set_level,"rig_set_level");
    HL_RES(p_error,"rigerror");
    HL_RES(p_debug,"rig_set_debug");
#undef HL_RES
    p_version = (const char *(*)())lib->resolve("rig_version");
    p_debug(RIG_DEBUG_NONE);
    loaded = true;
    return true;
}
QString HlWrap::Version()
{
    if (!loaded) return QString();
    if (p_version) return QString::fromLatin1(p_version());
    return "Hamlib";
}
void *HlWrap::Open(int model,const QString &port,int baud,QString &err)
{
    if (!Load(err)) return 0;
    RIG *r = p_init((rig_model_t)model);
    if (!r)
    {
        err = "Hamlib: rig_init failed";
        return 0;
    }
    QByteArray pn = port.trimmed().toLatin1();
    QByteArray sp = QByteArray::number(baud);
    p_set_conf(r,p_token(r,"rig_pathname"),pn.constData());
    if (baud > 0) p_set_conf(r,p_token(r,"serial_speed"),sp.constData());
    int e = p_open(r);
    if (e != RIG_OK)
    {
        err = "Hamlib: "+port+" "+QString::fromLatin1(p_error(e));
        p_cleanup(r);
        return 0;
    }
    return r;
}
void HlWrap::Close(void *rig)
{
    if (!rig || !loaded) return;
    p_close((RIG*)rig);
    p_cleanup((RIG*)rig);
}
int HlWrap::GetFreq(void *rig,double *hz)
{
    freq_t f = 0;
    int e = p_get_freq((RIG*)rig,RIG_VFO_CURR,&f);
    *hz = f;
    return e;
}
int HlWrap::SetFreq(void *rig,double hz)
{
    return p_set_freq((RIG*)rig,RIG_VFO_CURR,(freq_t)hz);
}
int HlWrap::GetMode(void *rig,QString &m)
{
    rmode_t mode = 0;
    pbwidth_t w = 0;
    int e = p_get_mode((RIG*)rig,RIG_VFO_CURR,&mode,&w);
    if (e != RIG_OK) return e;
    if (mode == RIG_MODE_USB) m = "USB";
    else if (mode == RIG_MODE_LSB) m = "LSB";
    else if (mode == RIG_MODE_PKTUSB) m = "DIGU";
    else if (mode == RIG_MODE_PKTLSB) m = "DIGL";
    else if (mode == RIG_MODE_FM) m = "FM";
    else if (mode == RIG_MODE_PKTFM) m = "FM-D";
    else if (mode == RIG_MODE_AM) m = "AM";
    else if (mode == RIG_MODE_PKTAM) m = "AM-D";
    else if (mode == RIG_MODE_CW) m = "CWL";
    else if (mode == RIG_MODE_CWR) m = "CWU";
    else if (mode == RIG_MODE_RTTY) m = "R-L";
    else if (mode == RIG_MODE_RTTYR) m = "R-U";
    else m = "WRONG_MODE";
    return e;
}
int HlWrap::SetMode(void *rig,const QString &m)
{
    rmode_t mode;
    if (m == "USB") mode = RIG_MODE_USB;
    else if (m == "LSB") mode = RIG_MODE_LSB;
    else if (m == "DIGU") mode = RIG_MODE_PKTUSB;
    else if (m == "DIGL") mode = RIG_MODE_PKTLSB;
    else if (m == "FM") mode = RIG_MODE_FM;
    else if (m == "FM-D" || m == "DIGFM") mode = RIG_MODE_PKTFM;
    else if (m == "AM") mode = RIG_MODE_AM;
    else return RIG_OK;
    return p_set_mode((RIG*)rig,RIG_VFO_CURR,mode,RIG_PASSBAND_NOCHANGE);
}
int HlWrap::SetPtt(void *rig,bool on)
{
    return p_set_ptt((RIG*)rig,RIG_VFO_CURR,on ? RIG_PTT_ON : RIG_PTT_OFF);
}
int HlWrap::GetLevel(void *rig,int level,float *v)
{
    setting_t s = RIG_LEVEL_RFPOWER;
    if (level == L_RFPOWER_METER) s = RIG_LEVEL_RFPOWER_METER;
    else if (level == L_SWR) s = RIG_LEVEL_SWR;
    else if (level == L_ALC) s = RIG_LEVEL_ALC;
    value_t val;
    val.f = 0.0f;
    int e = p_get_level((RIG*)rig,RIG_VFO_CURR,s,&val);
    *v = val.f;
    return e;
}
int HlWrap::SetRfPower(void *rig,float v)
{
    value_t val;
    val.f = v;
    return p_set_level((RIG*)rig,RIG_VFO_CURR,RIG_LEVEL_RFPOWER,val);
}
QString HlWrap::Error(int code)
{
    if (!loaded) return QString::number(code);
    return QString::fromLatin1(p_error(code));
}

#else  // ---------------------------------------------------------------- no Hamlib in this build

bool HlWrap::Load(QString &err)
{
    err = "Hamlib is not included in this version of MSHV-IC8TEM";
    return false;
}
QString HlWrap::Version() { return QString(); }
void *HlWrap::Open(int,const QString &,int,QString &err) { Load(err); return 0; }
void HlWrap::Close(void *) {}
int HlWrap::GetFreq(void *,double *) { return -1; }
int HlWrap::SetFreq(void *,double) { return -1; }
int HlWrap::GetMode(void *,QString &) { return -1; }
int HlWrap::SetMode(void *,const QString &) { return -1; }
int HlWrap::SetPtt(void *,bool) { return -1; }
int HlWrap::GetLevel(void *,int,float *) { return -1; }
int HlWrap::SetRfPower(void *,float) { return -1; }
QString HlWrap::Error(int code) { return QString::number(code); }
#endif
