/* MSHV HvAstroDataW
 * Copyright 2015 Hrisimir Hristov, LZ2HV
 * May be used under the terms of the GNU General Public License (GPL)
 */

#ifndef HVASTRODATAW_H //hvastrodata
#define HVASTRODATAW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QDateTime>
#include <QTimer>
#include <QLineEdit>
#include <QRadioButton>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>

// Doppler tracking (IC8TEM mod): spin box that shows the kHz value with leading zeros, e.g. "090 kHz"
class HvKhzSpinBox : public QSpinBox
{
public:
    HvKhzSpinBox(QWidget *parent = 0) : QSpinBox(parent) {}
protected:
    QString textFromValue(int v) const
    {
        return QString("%1").arg(v,3,10,QChar('0'));
    }
};

class HvAstroDataW : public QWidget
{
    Q_OBJECT
public:
    HvAstroDataW(int,int,QWidget *parent = 0);
    virtual ~HvAstroDataW();

    void SetMyLocHisLocBand(QString mloc,QString hisloc,QString strfreq);
    void StartStopTimer(bool f);
    void SetRaDec(QString);
    QString GetRaDec();
    void SetFont(QFont);
    QString GetPosXY()
	{
		return QString("%1").arg(pos().x())+"#"+QString("%1").arg(pos().y());				
	}
	void SetPosXY(QString);
	void SetTx(bool f);
    // Doppler tracking (IC8TEM mod)
    void SetRigFreq(QString hz);     // dial frequency read back from the rig via CAT (Hz, app side)
    void SetAppSetFreq(QString hz);  // dial frequency commanded by MSHV (band / frequency list)
    QString GetDopplerSettings();
    void SetDopplerSettings(QString);

signals:
    void EmitAstroWIsClosed();
    void EmitAstroData(double my_za,double my_el,int his_dop,double dgrd);
    void EmitDopplerFreqs(bool active,double rx_hz,double tx_hz);// Doppler tracking (IC8TEM mod)

public slots: 

private slots:
    void astro0();
    void DopplerMethodChanged(bool);
    void SetSkedKhz();
    void DopplerParmsChanged();

private:
	QString azel_path;
	bool f_txrx;
    QTimer *timer_ref_;
    QString s_mygrid;
    QString s_hisgrid;
    int s_nfreq;

    QLabel *s_radeceg;
    QLabel *moon_az;
    QLabel *moon_el;
    QLabel *moondx_az;
    QLabel *moondx_el;
    QLabel *sun_az;
    QLabel *sun_el;
    QLabel *sou_az;
    QLabel *sou_el;
    QLabel *dop_dx;
    QLabel *dop_self;
    QLabel *dfdt_dx;
    QLabel *dfdt_self;
    QLabel *sp_dx;
    QLabel *sp_self;
    QLabel *w50_dx;
    QLabel *w50_self;
    QLabel *moon_ra;
    QLabel *moon_dec;
    QLabel *sou_ra;
    QLabel *sou_dec;
    QLabel *freq_0;
    QLabel *tasky_0;
    QLabel *mnr_0;
    QLabel *dgrd_0;
    QLabel *dpol_0;
    QLabel *sd_0;
    
    QLineEdit *le_auxra;
    QLineEdit *le_auxdec;

    double uth8z;
    double dopplerz;
    double doppler00z;
    double poloffset1;
    double poloffset2;

    // Doppler tracking (IC8TEM mod)
    enum {DOP_FULL=0,DOP_OWN_ECHO,DOP_CFOM,DOP_ON_DX_ECHO,DOP_CALL_DX,DOP_NONE,DOP_COUNT};
    QRadioButton *rb_dop[DOP_COUNT];
    HvKhzSpinBox *sb_sked_khz;
    QPushButton *pb_set_khz;
    QLabel *l_rx_frq;
    QLabel *l_tx_frq;
    QDoubleSpinBox *sb_tx_shift;
    QCheckBox *cb_tx_shift;
    QGroupBox *gb_echo;
    QSpinBox *sb_rit;
    QCheckBox *cb_dither;
    int dop_method;
    double sked_hz;          // sked (nominal dial) frequency, 0 = not yet known
    double rig_hz;           // last frequency read back from the rig
    double dop_full_raw;     // one-way Doppler Me->Moon->DX (Hz at dop_nfreq_raw MHz)
    double dop_self_raw;     // own echo Doppler (Hz at dop_nfreq_raw MHz)
    int dop_nfreq_raw;
    bool dop_valid;
    double dop_dxecho_frozen;// On DX Echo / Call DX: Tx term frozen at QSO start (Hz)
    bool dop_frozen_valid;
    double last_rx_cmd;
    double prev_rx_cmd;
    double last_tx_cmd;
    bool dop_emit_active;
    QDateTime t_last_cmd;
    void UpdateDoppler(bool force_emit);
    void ComputeDoppler(double &rx,double &tx);
    double DopScale();
    QString FormatHz(double hz);
    
    void tm2(double day,double xlat4,double xlon4,double &xl4,double &b4);
    double dot(double *x,double *y);
    void toxyz(double alpha,double delta,double r,double *vec);
    void fromxyz(double *vec,double &alpha,double &delta,double &r);
    void coord(double A0,double B0,double AP,double BP,double A1,double B1,double &A2,double &B2);
    void dcoord(double A0,double B0,double AP,double BP,double A1,double B1,double &A2,double &B2);
    void moon2(int y,int m,int Day,double UT,double lon,double lat,double &RA,double &Dec,
                        double &topRA,double &topDec,double &LST,double &HA,double &Az,double &El,double &dist);//Az0,El0,
    void geocentric(double alat,double elev,double &hlt,double &erad);
    void MoonDop(int nyear,int month,int nday,double uth4,double lon4,double lat4,double &RAMoon4,
                 double &DecMoon4,double &LST4,double &HA4,double &AzMoon4,double &ElMoon4,double &ldeg4,
                 double &bdeg4,double &vr4,double &dist4);
    void sun(int y,int m,int DD,double UT,double lon,double lat,double &LST,double &Az,double &El,double &day);
    void grid2deg(QString grid0,double &dlong,double &dlat);
    void astro(int nyear,int month,int nday,double uth,int nfreq,QString Mygrid,int NStation,int mode,
               int MoonDX,double &AzSun,double &ElSun,double &AzMoon0,double &ElMoon0,int &ntsky,double &doppler00,
               double &doppler,double &dbMoon,double &RAMoon,double &DecMoon,double &HA,double &Dgrd, double &sd,
               double &poloffset,double &xnr,double auxra,double auxdec,double &azaux,double &elaux,double &day,
               double &lon,double &lat,double &LST);
    //void astro0();

    void closeEvent(QCloseEvent*);
};
#endif
