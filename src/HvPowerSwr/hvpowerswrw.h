/* MSHV-IC8TEM  Power & SWR window
 *
 * Sets the RF output power of the radio and shows Po, SWR and ALC while
 * transmitting; power memory per band (optionally per mode), SWR protection
 * that stops the transmission, tune at reduced power.
 * Works with the Icom CI-V radios (USB) and Icom LAN:
 *   14 0A  RF power 0000-0255,  15 11 Po,  15 12 SWR,  15 13 ALC.
 *
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef HVPOWERSWRW_H
#define HVPOWERSWRW_H

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QTableWidget>
#include <QTabWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QMap>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QLineEdit>
#include "hvpwrlink.h"

class HvPwrMeter : public QWidget
{
    Q_OBJECT
public:
    HvPwrMeter(int kind,QWidget *parent = 0);   // 0 Po (0..1), 1 SWR, 2 ALC (0..1)
    void SetValue(double v,double peak = -1.0);
    void SetLimit(double l);                     // SWR protection mark
protected:
    void paintEvent(QPaintEvent *);
private:
    int s_kind;
    double s_val;
    double s_peak;
    double s_limit;
};

class HvPowerSwrW : public QWidget
{
    Q_OBJECT
public:
    HvPowerSwrW(QString app_path,QWidget *parent = 0);
    virtual ~HvPowerSwrW();
    bool WasVisible();
    void SaveSettings();

signals:
    void EmitRigCmd(int,QString);       // to HvRigControl::SendPwrCmd
    void EmitTune();                    // toggle the MSHV Tune
    void EmitStopTx();
    void EmitClosed();
    void EmitAskCapable();

public slots:
    void SetRigInfo(int,QString);
    void SetCapable(bool);
    void SetTx(bool);
    void SetFreqStr(QString);
    void SetModeName(QString);
    void SetStatus(QString);

protected:
    void closeEvent(QCloseEvent *);
    void showEvent(QShowEvent *);

private slots:
    void TabChanged(int);
    void FitNow();
    void SliderChanged(int);
    void SendPower();
    void Minus();
    void Plus();
    void Preset();
    void SaveBand();
    void ReadFromRadio();
    void TuneClicked();
    void Unlock();
    void PollMeters();
    void SettingsChanged();
    void TableChanged(QTableWidgetItem *);
    void ReadNow();
    void LogClear();
    void LogCopy();
    // own connection (independent from the MSHV CAT)
    void OnRfPct(int);
    void OnPo(double);
    void OnSwr(double);
    void OnAlc(double);
    void LinkStatus(bool,QString);
    void LinkLog(bool,QString);
    void LinkFreq(double);
    void ConnChanged();
    void ConnApply();
    void RefreshCom();
    void PollFreq();

private:
    QString s_set_path;
    bool f_block;
    bool f_capable;
    bool f_tx;
    bool f_blocked;
    bool f_tune;
    int s_saved_pct;
    QString s_band;
    QString s_mode;
    QElapsedTimer clock;
    qint64 tx_since;
    qint64 swr_high_since;
    double po_peak;
    qint64 po_peak_at;
    double last_po;
    double last_swr;
    double last_alc;
    QMap<QString,int> mem_pwr;     // band or band_mode -> %
    QMap<QString,int> mem_tune;    // band -> %
    bool f_was_visible;
    bool f_closing;

    QTabWidget *tabs;
    // the window takes the size of the open tab only: the Power and SWR tab stays small
    QList<QWidget*> tab_pages;
    QTimer *t_fit;
    bool f_fit_exact;
    QWidget *TabPage(QWidget *content);
    void FitLater(bool exact);
    void Fit(bool exact);
    QLabel *l_head;
    QLabel *l_tx;
    QLabel *l_pct;
    QLabel *l_watt;
    QSlider *sl_pwr;
    QPushButton *pb_minus;
    QPushButton *pb_plus;
    QList<QPushButton*> pb_presets;
    QLabel *l_mem;
    QPushButton *pb_save_band;
    QPushButton *pb_tune;
    QPushButton *pb_read;
    HvPwrMeter *m_po;
    HvPwrMeter *m_swr;
    HvPwrMeter *m_alc;
    QLabel *l_po;
    QLabel *l_swr;
    QLabel *l_alc;
    QLabel *l_event;
    QLabel *l_block;
    QPushButton *pb_unlock;
    QLabel *l_notcap;
    QCheckBox *cb_prot_main;
    // settings
    QSpinBox *sb_maxw;
    QSpinBox *sb_interval;
    QCheckBox *cb_only_tx;
    QTableWidget *tw_bands;
    QCheckBox *cb_apply_band;
    QCheckBox *cb_per_mode;
    QCheckBox *cb_prot;
    QDoubleSpinBox *sb_swr_lim;
    QDoubleSpinBox *sb_swr_sec;
    QCheckBox *cb_alc_warn;
    QCheckBox *cb_po_warn;

    QTimer *t_send;
    QTimer *t_poll;
    QTimer *t_cap;
    QLabel *l_status;
    QPlainTextEdit *te_log;
    QCheckBox *cb_log_pause;
    HvPwrLink *link;
    QComboBox *cb_conn;
    QLineEdit *le_host;
    QSpinBox *sb_port;
    QComboBox *cb_com;
    QComboBox *cb_baud;
    QLineEdit *le_civ;
    QPushButton *pb_conn;
    QLabel *l_conn;
    QTimer *t_freq;
    bool f_cat_capable;
    bool f_link_ok;
    QString s_cat_status;
    QString s_link_status;
    void RigCmd(int id,const QString &s);
    void UpdateCapable();
    void UpdateConnWidgets();
    void Log(bool tx,const QString &hex);
    QString Decode(const QString &hex);

    void ReadSettings();
    void FillTable();
    void UpdatePowerLabels();
    void SetPct(int pct,bool send);
    QString MemKey();
    void ApplyBandMemory();
    void UpdateEnabled();
    void Event(const QString &s,bool bad);
    static QString BandOf(double mhz);
    static double PoFraction(int raw);
    static double SwrValue(int raw);
};
#endif
