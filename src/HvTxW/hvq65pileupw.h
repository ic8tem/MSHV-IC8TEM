/* MSHV-IC8TEM  Q65 Pileup: list of the recent callers.
 * The window of WSJT-X 2.7 / 3.2 "Active Stations" in Q65 Pileup mode (K1JT and the WSJT
 * Development Group, GPL): callers in frequency order with locator, Moon elevation, hours since
 * the last decode and '#' when their last message said they copied our last transmission.
 * Click: answer with Tx3 (R + locator). Ctrl+click: remove the caller from the list.
 *
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef HVQ65PILEUPW_H
#define HVQ65PILEUPW_H

#include <QWidget>
#include <QStringList>
#include <QFont>

class QTreeWidget;
class QTreeWidgetItem;
class QLabel;
class QPushButton;

class HvQ65PileupW : public QWidget
{
    Q_OBJECT
public:
    explicit HvQ65PileupW(QWidget *parent = 0);
    virtual ~HvQ65PileupW();
    void SetFont(QFont);
    QString GetPosXYWH();
    void SetPosXYWH(QString);
    static QString InfoText(bool pileup);

public slots:
    void SetList(QStringList);
    void SetDxCall(QString);
    void SetState(bool q65,int activity_id);

signals:
    void StartQso(QString call,QString grid,QString period,int freq);
    void RemoveCaller(QString call);
    void EraseList();
    void RefreshList();

protected:
    void showEvent(QShowEvent *);

private slots:
    void ItemClicked(QTreeWidgetItem*,int);
    void EraseClicked();

private:
    QTreeWidget *tw;
    QLabel *l_count;
    QLabel *l_info;
    QPushButton *b_erase;
    QPushButton *b_refresh;
    QString s_dx;
    QStringList s_list;
    bool f_q65;
    int s_activity;
    bool f_pos_known;
    void Fill();
    void SetInfo();
};
#endif
