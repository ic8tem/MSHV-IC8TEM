/* MSHV HvLotwUsersW  (IC8TEM mod)
 * LoTW users list from the ARRL file lotw-user-activity.csv
 * ("CALL,YYYY-MM-DD,HH:MM:SS" = last upload to LoTW).
 * Shown as page 3 of "Network Configuration".
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef HVLOTWUSERS_H
#define HVLOTWUSERS_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QDateEdit>
#include <QHash>
#include <QString>
#include <QProcess>
class QNetworkAccessManager;

class HvLotwUsersW : public QWidget
{
    Q_OBJECT
public:
    HvLotwUsersW(QString app_path,QWidget *parent = 0);
    virtual ~HvLotwUsersW();

    // true = the call (or its base call without prefix/suffix) uploaded to LoTW on/after the selected date
    static bool IsLotwUser(const QString &call);
    static bool IsShowActive();
    static bool HasList();                           // a list is loaded
    static bool IsLotwValid(const QString &call);    // like IsLotwUser() but ignores the "show dot" checkbox

private slots:
    void LoadCsvFile();
    void MinDateChanged(const QDate &);
    void ShowChanged(bool);
    void DownloadCsv();
    void DownloadProgress(qint64,qint64);
    void DownloadFinished();
    void CurlFinished(int,QProcess::ExitStatus);

private:
    QString s_app_path;
    QString s_csv_path;
    QString s_set_path;
    QLabel *l_status;
    QDateEdit *de_min_date;
    QCheckBox *cb_show;
    QPushButton *pb_download;
    QNetworkAccessManager *nam;
    bool f_block_save;
    int ParseCsv(QString path,QHash<QString,int> &out,QDate &last);
    void RefreshStatus();
    void ReadSettings();
    void SaveSettings();
    void RefreshDecodeLists();
    void UseDownloaded(QString tmp);
};
#endif
