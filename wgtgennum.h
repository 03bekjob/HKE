#ifndef WGTGENNUM_H
#define WGTGENNUM_H

#include <QWidget>
#include <QPrinter>
#include <QMap>
#include <QFontDatabase>
#include "global.h"
#include "hketablemodel.h"
#include <QSqlDatabase>

namespace Ui {
class WgtGenNum;
}

#define CODE128_B_START 104
#define CODE128_STOP 106
//class SeasonTicket
//{
//public:
//    SeasonTicket():num(""),barcode(""),encodebarcode(""),isPrn(0) {}
//    QString num(){return num;}
//    QString barcode() {return barcode;}
//    QString encodeBarcode(){return encodebarcode;}
//    int isPrn(){return isPrn;}

//    void setNum(const QString& n){num = n;}
//    void setBarcode(const QString& bc){barcode = bc;}
//    void setEncodeBarcode(const QString& ebc){encodebarcode = bc;}
//    void setPrn(const int& p){isPrn = p;}
//    void clear(){num=barcode=encodebarcode=""; isPrn = 0;}
//private:
//    QString num;
//    QString barcode;
//    QString encodebarcode;
//    int isPrn;
//};

struct seasonTicket
{
    QString num;
    QString barcode;
    QString encodebarcode;
    int isPrn;

};

//struct snTcktI{
//    explicit snTckt(int num_=-1,
//            const QString &barcode_=QString(),
//            const QString &encodedBarcode_=QString(),
//            const int &isPrn_=0)
//        : num(num_), barcode(barcode_), isPrn(isPrn_) {}

//    bool operator<(const snTckt &other) const
//        { return num != other.num ? num < other.num : false; }

//    int num;
//    QString barcode;
//    QString encodedBarcode;
//    int isPrn;

//};

class WgtGenNum : public QWidget
{
    Q_OBJECT

public:
    explicit WgtGenNum(/*const*/ QSqlDatabase& db, QWidget *parent = 0);
    ~WgtGenNum();
    QAction* getMenuAction() const { return act; }
    HkeTableModel* getMdl(){return mdl;}

signals:
    void sgnExit();
private:
    Ui::WgtGenNum *ui;
    QSqlDatabase m_db;
    void createConnect();
    void readSettings();
    void saveSettings();
    int rnd(const int& b, const int& e);
    void strTrio(QString& s, const int& m);
    void strTwo(QString& s, const int& m);
    void configPage(QPrinter* prn);
    int calcCheckSumEAN13(const QString& barcodeText);

    int codeToChar(int code);
    int charToCode(int ch);
    QString encodeBarCODE128B(QString code);
    int calcCheckSumCODE128B(QString code);

    seasonTicket sTckt;

    QAction* act;

    QStringList lstGenNum;

    QMap<QString,QString> mapCode;
    QMap<QString,seasonTicket> mapSTctkt;

    QFontDatabase dbFont;
    HkeTableModel* mdl;
    void resizeView();
    void prnAll();
    void prnSelect();

private slots:
    void sltExit();
    void sltChangedCnt(const int& cnt);
    void sltChangedLineCnt(const QString& cnt);
    void sltGenNum();
    void sltCreateBarcode();
    void sltPrnBarcode();
    void sltPrnBarcodeMdl();
    void sltSaveFile();
    void sltSaveFilefromMdl();
    void sltLoadNum();

    void sltModeSelectRows(bool b);
    void sltSelectRows();
    void sltUnSelectRows();

//    void sltChangedPrn(QModelIndex,QModelIndex);
};

#endif // WGTGENNUM_H
