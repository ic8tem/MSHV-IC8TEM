/* MSHV
 * By Hrisimir Hristov - LZ2HV
 * May be used under the terms of the GNU General Public License (GPL)
 */
#ifndef HVTXTCOLOR_H
#define HVTXTCOLOR_H

#include <QDialog>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>
#include <QGroupBox>
#include <QLabel>
#define C_COLORS 9 //IC8TEM: +7 worked on other bands (pink), +8 calls that heard me (PSKReporter, underline)
#define C_SHEM 2
#define C_CB 29
#define C_BT 9

#include "../HvTxW/hvinle.h"
//#include "../config.h"
//#include <QtGui>

class HvTxtColor : public QDialog
{
    Q_OBJECT
public:
    HvTxtColor(bool,QWidget *parent = 0);
    virtual ~HvTxtColor();
    QString GetTextMark();
    void SetTextMark(QString);
    void SetTextColor(QString);
    QString GetTextColor()
    {
        QString s;
        for (int j =0; j<C_SHEM; j++)
        {
            for (int i =0; i<C_COLORS; i++)
            {
                s.append(QString("%1").arg(collors[j][i].red()));
                s.append("#");
                s.append(QString("%1").arg(collors[j][i].green()));
                s.append("#");
                s.append(QString("%1").arg(collors[j][i].blue()));

                if (i<C_COLORS-1)
                    s.append("#");
            }
            if (j<C_SHEM-1) s.append("|");
        }
        return s;
    };
    void SetFont(QFont f);
    void Refresh() { TextMarkChanged(true); }//IC8TEM v32 send the current options

signals:
    void EmitTextMark(bool *f,QString);
    void EmitTextMarkColors(QColor*,int,bool,bool);
    void EmitWorkedOpts(bool,bool);//IC8TEM pink calls, new grids only
    void EmitHeardOpts(bool);//IC8TEM v32 underline the calls that heard me (PSKReporter)

public slots:
    void SetHeardStatus(QString);//IC8TEM v32

private slots:
    void TextMarkChanged(bool);
    void TxTextMarkChanged(bool);
    void OpenCDBox1();
    void OpenCDBox2();
    void OpenCDBox3();
    void OpenCDBoxT();
    void OpenCDBoxQ();
    void OpenCDBoxMC();
    void OpenCDBoxHC();
    void OpenCDBoxPK();//IC8TEM
    void OpenCDBoxHD();//IC8TEM v32
    void setDefColors();
    void ColorChanged(QColor);
    //void LeTextChanged(QString);

private:
    int dsty;
    QCheckBox *cb[C_CB];
    QCheckBox *cb_pink;//IC8TEM
    QCheckBox *cb_newgrid;//IC8TEM
    QCheckBox *cb_heard;//IC8TEM v32
    QLabel *l_heard;//IC8TEM v32 PSKReporter status
    HvLeWithSpace *le_cals_msg;    
    QColor collors[C_SHEM][C_COLORS];
    QColor collors0[C_SHEM][C_COLORS];
    int b_ident;
    QPushButton *b_[C_BT];
    QColorDialog *CD_box;
    void RefreshAll();
    QString CorrectSyntax(QString txt ,bool cordot);

protected:
    void closeEvent(QCloseEvent*);

};
#endif
