#include "wgtgennum.h"
#include "ui_wgtgennum.h"
#include <QTime>
#include <QPrintDialog>
#include <QPainter>
#include <QPrinter>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QTextStream>
#include <QMessageBox>
#include <QModelIndexList>
#include <QItemSelectionModel>
#include <QRegExp>
#include <QRegExpValidator>
#include <QValidator>
#include <QSqlError>
#include <QSqlQuery>
#include <QProgressDialog>

//#include <QDebug>

WgtGenNum::WgtGenNum(QSqlDatabase &db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgtGenNum)
    ,m_db(db)
{
    ui->setupUi(this);
    //setWindowIcon(QIcon(":/img/barcode.png"));
    setAttribute(Qt::WA_DeleteOnClose);

    ui->pushButton_Prn->setVisible(false);
    ui->horizontalSlider->setMaximum(MAXCOUNT);

    QRegExp rx("[1-9][0-9]{0,3}");
    QValidator *v = new QRegExpValidator(rx, this);
    ui->lineEdit_bNum->setValidator(v);
    ui->lineEdit_eNum->setValidator(v);

    QString sTtl;
    sTtl = tr("%1").arg(this->windowTitle().trimmed());

    act = new QAction(sTtl,this);
    act->setCheckable(true);

    dbFont.addApplicationFont(":/Fonts/code128.ttf");

    mdl = new HkeTableModel(this);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tableView->setModel(mdl);
    resizeView();

    readSettings();
    //ui->horizontalSlider->setValue(2);
    //ui->Cnt->setText(QString::number(ui->horizontalSlider->value()));
    ui->lineEdit_Cnt->setText(QString::number(ui->horizontalSlider->value()));
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    //sltModeSelectRows(false);
    if(ui->radioButton_All->isChecked()){
        ui->label_bNum->setVisible(false);
        ui->lineEdit_bNum->setVisible(false);
        ui->label_eNum->setVisible(false);
        ui->lineEdit_eNum->setVisible(false);
        ui->pushButton_Select->setVisible(false);
        ui->pushButton_UnSelect->setVisible(false);
    }

    ui->buttonGroup->setId(ui->radioButton_File,0);
    ui->buttonGroup->setId(ui->radioButton_Db,1);
    ui->radioButton_File->setChecked(true);

    createConnect();

    //ui->radioButton_All->setChecked(true);


}

WgtGenNum::~WgtGenNum()
{
    saveSettings();
    delete ui;
}

void WgtGenNum::createConnect()
{
    connect(act,SIGNAL(triggered()),this,SLOT(showMaximized()));
    connect(ui->pushButton_Exit,SIGNAL(clicked()),this,SLOT(sltExit()));
    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(sltChangedCnt(int)));
    //connect(ui->pushButton_GenNum,SIGNAL(clicked()),this,SLOT(sltGenNum()));
    connect(ui->lineEdit_Cnt,SIGNAL(textChanged(QString)),this,SLOT(sltChangedLineCnt(QString)));

    connect(ui->pushButton_CreateBarcode,SIGNAL(clicked()),this,SLOT(sltCreateBarcode()));
    connect(ui->pushButton_LoadNum,SIGNAL(clicked()),this,SLOT(sltLoadNum()));

    connect(ui->pushButton_Prn,SIGNAL(clicked()),this,SLOT(sltPrnBarcode()));
    connect(ui->pushButton_PrnAll,SIGNAL(clicked()),this,SLOT(sltPrnBarcodeMdl()));
//    connect(ui->pushButton_SaveFile,SIGNAL(clicked()),this,SLOT(sltSaveFile()));
    connect(ui->pushButton_SaveFile,SIGNAL(clicked()),this,SLOT(sltSaveFilefromMdl()));

    connect(ui->radioButton_Select,SIGNAL(clicked(bool)),this,SLOT(sltModeSelectRows(bool)));
    connect(ui->radioButton_All,SIGNAL(clicked(bool)),this,SLOT(sltModeSelectRows(bool)));

    connect(ui->pushButton_Select,SIGNAL(clicked()),this,SLOT(sltSelectRows()));
    connect(ui->pushButton_UnSelect,SIGNAL(clicked()),this,SLOT(sltUnSelectRows()));

//    connect(mdl,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(sltChangedPrn(QModelIndex,QModelIndex)));

}

void WgtGenNum::readSettings()
{
    QSettings st;
    QString key,keyCnt;

    key = tr("WgtGenNum");
    keyCnt = key+tr("/count");
    if(st.value(keyCnt).isNull()){
        ui->horizontalSlider->setValue(2);
    }
    else {
        QVariant v;
//        isHiddenToolBar = st.value(keyH,v).toBool();
        ui->horizontalSlider->setValue(st.value(keyCnt,v).toInt());
    }

}

void WgtGenNum::saveSettings()
{
    QSettings st;
    QString key,keyCnt;

    key = tr("WgtGenNum");
    keyCnt = key+tr("/count");
    st.setValue(keyCnt,ui->horizontalSlider->value()/*this->geometry()*/);

}

int WgtGenNum::rnd(const int &b, const int &e)
{
    return qrand()%(e-b+1) + b;
}

void WgtGenNum::strTrio(QString &s, const int &m)
{
    if(s.trimmed().length()<3){
        switch (m) {
        case 0:
            s = s.leftJustified(3,'0');
            break;
        case 1:
            s = s.rightJustified(3,'0');
            break;
        default:
            break;
        }
    }
}

void WgtGenNum::strTwo(QString &s, const int &m)
{
    if(s.trimmed().length()<2){
        switch (m) {
        case 0:
            s = s.leftJustified(2,'0');
            break;
        case 1:
            s = s.rightJustified(2,'0');
            break;
        default:
            break;
        }
    }

}

void WgtGenNum::configPage(QPrinter *prn)
{
//   prn->setColorMode(QPrinter::GrayScale);
//   prn->setPageSizeMM(QSizeF(80,40));
//   prn->setPaperSize(QSizeF(80,40), QPrinter::Millimeter);
//   prn->setResolution(203);
//   prn->setPageMargins(0,0,0,0, QPrinter::Millimeter);
//   prn->setOrientation(QPrinter::Portrait);
//   prn->setOrientation(QPrinter::Landscape);
   prn->setColorMode(QPrinter::GrayScale);
   prn->setPageSizeMM(QSizeF(105,75));
   prn->setPaperSize(QSizeF(105,75), QPrinter::Millimeter);
   prn->setResolution(203);
   //prn->setResolution(QPrinter::HighResolution);
   prn->setPageMargins(0,0,0,0, QPrinter::Millimeter);
   prn->setOrientation(QPrinter::Portrait);

}

int WgtGenNum::calcCheckSumEAN13(const QString &barcodeText)
{
    int sum = 0;
    int digit = 0;

    // calculate checksum digit

    for(int i = barcodeText.length() - 1; i >= 0; i--)
    {
        digit = barcodeText.mid(i, 1).toInt();
        if(i % 2 == 0)
        {
            sum += digit;			// odd
        }
        else
        {
            sum += digit * 3;		// even
        }
//qDebug()<<i<<digit<<i%2<<sum;
    }
//qDebug()<<sum % 10<<(10 -(sum % 10))<<(10 -(sum % 10)) % 10;
    int checksum = (10 -(sum % 10)) % 10;

    return checksum;
}

int WgtGenNum::codeToChar(int code)
{
    return code + 105;
}

int WgtGenNum::charToCode(int ch)
{
     return ch - 32;
}

QString WgtGenNum::encodeBarCODE128B(QString code)
{
   QString encoded;
   encoded.prepend(QChar(codeToChar(CODE128_B_START)));    //Start set with B Code 104
   encoded.append(code);
   encoded.append(QChar(calcCheckSumCODE128B(code)));
   encoded.append(QChar(codeToChar(CODE128_STOP)));        //End set with Stop Code 106
   return encoded;

}

int WgtGenNum::calcCheckSumCODE128B(QString code)
{
    QByteArray encapBarcode; //(code.toUtf8()); //Convert code to utf8
    encapBarcode.append(code.toUtf8());
   //Calculate check character
   long long sum = CODE128_B_START;         //The sum starts with the B Code start character value
   int weight = 1;                          //Initial weight is 1

   foreach(char ch, encapBarcode) {
       int code_char = charToCode((int)ch); //Calculate character code
       sum += code_char*weight;             //add weighted code to sum
       weight++;                            //increment weight
   }

   int remain = sum%103;                    //The check character is the modulo 103 of the sum

   //Calculate the font integer from the code integer
   if(remain >= 95){
       remain += 105;
   }
   else{
       remain += 32;
   }

   return remain;

}

void WgtGenNum::resizeView()
{
    ui->tableView->resizeColumnsToContents();
    //ui->tableView->resizeRowsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

}

void WgtGenNum::prnAll()
{
    int cnt = mdl->rowCount();
    if(cnt<=0) return;

    QPrinter* prn = new QPrinter();
    QPrintDialog* pDlg = new QPrintDialog(prn,this);
    if(pDlg->exec()==QDialog::Accepted) {
        configPage(prn);

    }
    else{
        return;

        //--------Эти строки для отладки, эмитируют работу принтера
        prn->setOutputFileName("barcode.pdf");
        prn->setOutputFormat(QPrinter::PdfFormat);
        configPage(prn);
        //--------Эти строки для отладки, эмитируют работу принтера
    }

//    QFont fntBarCode(QFont("Code 128",24,QFont::Normal)); //16
//    QFont fntBarCode(QFont(":/Fonts/code 128",24,QFont::Normal)); //16

    QFont fntBarCode = dbFont.font("Code 128","font-style::normal",32);
    QFontMetrics fmBarCode(fntBarCode);
    int wBarCode=0;
    int hBarCode=0; //fmBarCode.height();

    QFont fntBarText("PT Sans", 8,QFont::Normal);
    QFontMetrics fmBarText(fntBarText);
    int wBarText=0;
    int hBarText=0; // fmBarText.height();
    int pgW = 0;
    int pgH = 0;

    QString barcode, encodebarcode,sNum;
    QModelIndex idxPrn;
    int isPrn = 1;
    QString strTmp;
    QPainter painter;//(prn);
//    QMapIterator<QString, QString> m(mapCode);
//    QMapIterator<QString, seasonTicket> m(mapSTctkt);
    painter.begin(prn);
    pgW = painter.viewport().width();
    pgH = painter.viewport().height();
    for(int n=0; n<cnt; n++){
        for(int j = 0; j<mdl->columnCount(); j++){
            strTmp = mdl->index(n,j).data().toString();
            switch (j) {
            case num:
                sNum = strTmp;
                break;
            case bc:
                barcode = strTmp;
                break;
            case ebc:
                encodebarcode = strTmp;
                break;
            case isprn:
                idxPrn = mdl->index(n,j);
                break;
            }
        }
//qDebug()<<sNum<<barcode<<encodebarcode;
        painter.setFont(fntBarCode);

        painter.setFont(fntBarCode);
        wBarCode = fmBarCode.width(encodebarcode)+20;
        hBarCode = fmBarCode.height()+fmBarCode.lineSpacing();

        painter.setFont(fntBarText);
        wBarText= fmBarText.width(sNum +" "+barcode);
        hBarText= fmBarText.height()+fmBarText.lineSpacing();

        QRect barcodeRect = QRect(pgW-(2*wBarCode), pgH-hBarCode-hBarText, 2*wBarCode, hBarCode);
        QRect barcodeTextRect = QRect(barcodeRect.x(), pgH-hBarText, barcodeRect.width(), hBarText);

        //qDebug()<<"barcodeRect "<<barcodeRect<<"barcodeTextRect "<<barcodeTextRect;

        painter.setFont(fntBarCode);
        painter.drawText(barcodeRect, Qt::AlignCenter, encodebarcode);

        painter.setFont(fntBarText);
        painter.drawText(barcodeTextRect,Qt::AlignCenter,sNum+"  "+barcode);

        if(mdl->setData(idxPrn,isPrn,Qt::EditRole)){
//           qDebug()<<"upd"<<idxPrn.data()<<isPrn;
           mdl->setIsDirty(true);
        }

        if(n <cnt-1) prn->newPage();

    }

/*    while (m.hasNext()) {
        m.next();
        sNum = m.value().num;
        barcode = m.value().barcode;
        encodebarcode =  m.value().encodebarcode;
        if(barcode.isEmpty()){
            break;
        }

//qDebug()<<prn->pageSize()<<painter.viewport().width()<<painter.viewport().height();
        painter.setFont(fntBarCode);


        painter.setFont(fntBarCode);
        wBarCode = fmBarCode.width(encodebarcode)+20;
        hBarCode = fmBarCode.height()+fmBarCode.lineSpacing();


        painter.setFont(fntBarText);
        wBarText= fmBarText.width(sNum +" "+barcode);
        hBarText= fmBarText.height()+fmBarText.lineSpacing();

        QRect barcodeRect = QRect(pgW-(2*wBarCode), pgH-hBarCode-hBarText, 2*wBarCode, hBarCode);
        QRect barcodeTextRect = QRect(barcodeRect.x(), pgH-hBarText, barcodeRect.width(), hBarText);

//qDebug()<<"barcodeRect "<<barcodeRect<<"barcodeTextRect "<<barcodeTextRect;

        painter.setFont(fntBarCode);
        painter.drawText(barcodeRect, Qt::AlignCenter, encodebarcode);

        painter.setFont(fntBarText);
        painter.drawText(barcodeTextRect,Qt::AlignCenter,sNum+"  "+barcode);


//    painter.save();
//        painter.translate(barcodeRect.x(),barcodeRect.y()+barcodeRect.width()/4);
//        painter.rotate(-90);
//        painter.drawText(QRect(0,0,barcodeRect.width(),barcodeRect.height()),encodebarcode);
//    painter.restore();

//    painter.save();
//    qDebug()<<"barcodeTextRect "<<barcodeTextRect<<painter.window();
//        painter.setFont(fntBarText);
//        painter.translate(barcodeTextRect.x(),barcodeTextRect.y());
//        painter.rotate(-90);
//    qDebug()<<"barcodeTextRect "<<barcodeTextRect;
//        painter.drawText(QRect(0,0,barcodeTextRect.width(),barcodeTextRect.height()),  "text 10");
//    painter.restore();

        if(m.hasNext()) prn->newPage();

    }*/

    painter.end();
//qDebug() << "Printing finished";
    delete pDlg;
    delete prn;

}

void WgtGenNum::prnSelect()
{
    //QMessageBox::information(NULL,"ВНИМАНИЕ","Модуль в разработке");
    QString barcode, encodebarcode,sNum;
    QString strTmp;

    QItemSelectionModel* sel = ui->tableView->selectionModel();
    QModelIndexList idxLst = sel->selectedRows();
    if(idxLst.count()<=0){
        QMessageBox::critical(NULL,"ВНИМАНИЕ","Отсутсвуют данные");
        return;
    }
    QPrinter* prn = new QPrinter();
    QPrintDialog* pDlg = new QPrintDialog(prn,this);
    if(pDlg->exec()==QDialog::Accepted) {
        configPage(prn);

    }
    else{
        return;

        //--------Эти строки для отладки, эмитируют работу принтера
        prn->setOutputFileName("barcode.pdf");
        prn->setOutputFormat(QPrinter::PdfFormat);
        configPage(prn);
        //--------Эти строки для отладки, эмитируют работу принтера
    }
//    QFont fntBarCode(QFont("Code 128",24,QFont::Normal)); //16
//    QFont fntBarCode(QFont(":/Fonts/code 128",24,QFont::Normal)); //16
    QFont fntBarCode = dbFont.font("Code 128","font-style::normal",32);
    QFontMetrics fmBarCode(fntBarCode);
    int wBarCode=0;
    int hBarCode=0; //fmBarCode.height();

    QFont fntBarText("PT Sans", 8,QFont::Normal);
    QFontMetrics fmBarText(fntBarText);
    int wBarText=0;
    int hBarText=0; // fmBarText.height();
    int pgW = 0;
    int pgH = 0;


    QPainter painter;
    painter.begin(prn);
    pgW = painter.viewport().width();
    pgH = painter.viewport().height();

    int n = 0;
    QModelIndex idxPrn;
    int isPrn = 1;
    foreach(QModelIndex idx, idxLst ){
        for(int j = 0; j<mdl->rowCount(); j++){
            strTmp = mdl->index(idx.row(),j).data().toString();
            switch(j){
            case num:
                sNum = strTmp;
                break;
            case bc:
                barcode = strTmp;
                break;
            case ebc:
                encodebarcode = strTmp;
                break;
            case isprn:
                idxPrn = mdl->index(idx.row(),j);
                break;
            }
        }
//qDebug()<<sNum<<barcode<<encodebarcode;
        painter.setFont(fntBarCode);

        painter.setFont(fntBarCode);
        wBarCode = fmBarCode.width(encodebarcode)+20;
        hBarCode = fmBarCode.height()+fmBarCode.lineSpacing();

        painter.setFont(fntBarText);
        wBarText= fmBarText.width(sNum +" "+barcode);
        hBarText= fmBarText.height()+fmBarText.lineSpacing();

        QRect barcodeRect = QRect(pgW-(2*wBarCode), pgH-hBarCode-hBarText, 2*wBarCode, hBarCode);
        QRect barcodeTextRect = QRect(barcodeRect.x(), pgH-hBarText, barcodeRect.width(), hBarText);

//qDebug()<<"barcodeRect "<<barcodeRect<<"barcodeTextRect "<<barcodeTextRect;

        painter.setFont(fntBarCode);
        painter.drawText(barcodeRect, Qt::AlignCenter, encodebarcode);

        painter.setFont(fntBarText);
        painter.drawText(barcodeTextRect,Qt::AlignCenter,sNum+"  "+barcode);

        if(mdl->setData(idxPrn,isPrn,Qt::EditRole)){
//           qDebug()<<"upd"<<idxPrn.data()<<isPrn;
           mdl->setIsDirty(true);
        }
//        else{
//            qDebug()<<" not upd"<<idxPrn.data()<<isPrn;
//        }
//qDebug()<<"afterUpd"<<mdl->index(idxPrn.row(),idxPrn.column()).data();
        if(n <idxLst.count()-1) prn->newPage();
        n++;
    }
    painter.end();
//qDebug() << "Printing finished";
    delete pDlg;
    delete prn;
}

void WgtGenNum::sltExit()
{
    if(mdl->IsDirty()){
        QMessageBox msgBox;
        msgBox.setText("В документе были сделаны отметки о печати");
        msgBox.setInformativeText("Сохранить отметки в файле");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        msgBox.setButtonText(QMessageBox::Save,"Сохранить");
        msgBox.setButtonText(QMessageBox::Discard,"Выйти без сохранения");
        msgBox.setButtonText(QMessageBox::Cancel,"Отменить");
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Save:
            mdl->save();
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            return;
        default:
            break;
        }
    }
    mdl->clearMdl();
    this->close();
    emit sgnExit();
}

void WgtGenNum::sltChangedCnt(const int &cnt)
{
//    ui->Cnt->setText(QString::number(cnt));
    ui->lineEdit_Cnt->setText(QString::number(cnt));
}

void WgtGenNum::sltChangedLineCnt(const QString &cnt)
{
    bool ok = false;
    int i = cnt.toInt(&ok);
    if(ok){
        ui->horizontalSlider->setValue(i);
    }
}

void WgtGenNum::sltGenNum()
{
    int cnt = ui->horizontalSlider->value();
    QString b,frs,scd,thr,fr,fv;
    QString strTmp;
    QString barCode;
    QString seria;
    QString sNum;
    QString sI;

    lstGenNum.clear();
    mapCode.clear();
    mapSTctkt.clear();
    seria = ui->comboBox->currentText();
    int i = 1;
    int chckSum = -1;
//qDebug()<<cnt;
    while( i<=cnt){
        b   = QString::number(rnd(1,9));
        frs = QString::number(rnd(1,999));
        scd = QString::number(rnd(1,999));
        thr = QString::number(rnd(1,999));
        fr  = QString::number(rnd(1,999));
        fv  = QString::number(rnd(1,99));

        strTrio(frs,0);
        strTrio(scd,1);
        strTrio(thr,0);
        strTrio(fr,1);
        strTwo(fv,0);
        //strTmp = b+frs+scd+thr+fr;
        //strTmp = frs+scd+thr+fr;
        //strTmp = seria+frs+scd+thr;
        //strTmp = "s"+frs+scd+thr+fv;
        strTmp = seria+frs+scd+thr+fv;
//qDebug()<<strTmp;
//        chckSum = calcCheckSumEAN13(strTmp);
//        barCode = strTmp+QString::number(chckSum);

        barCode =  encodeBarCODE128B(strTmp);
//qDebug()<<strTmp<<barCode;
//        if(lstGenNum.contains(strTmp)){
//            qDebug()<<"---------------------------------уже есть";
//           continue;
//        }
//        lstGenNum.append(strTmp);
        if(mapCode.contains(strTmp)){
            //qDebug()<<"---------------------------------уже есть";
            continue;
        }
        mapCode.insert(strTmp,barCode);

        sI = QString::number(i).trimmed();
        if(sI.length()<4){
            sI = sI.rightJustified(4,'0');
        }
        sNum = seria+sI; //QString::number(i);
//        sTckt.clear();
//        sTckt.setNum(sNum);
//        sTckt.setBarcode(strTmp);
//        sTckt.setEncodeBarcode(barcode);
//        sTckt.isPrn(0);
//        mapSTctkt.inser(strTmp,sTckt);
        sTckt.num = sNum;
        sTckt.barcode = strTmp;
        sTckt.encodebarcode = barCode;
        sTckt.isPrn = 0;
        mapSTctkt.insert(sI,sTckt); //strTmp

        i++;
        strTmp = "";
        sI = "";
        chckSum = -1;
    }
    //ui->textEdit_GenNum->clear();
//    foreach (QString s, lstGenNum) {
//        ui->textEdit_GenNum->append(s);

//    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
//    QMapIterator<QString, QString> m(mapCode);
//    while (m.hasNext()) {
//        m.next();
//        //cout << i.key() << ": " << i.value() << endl;
//        ui->textEdit_GenNum->append(m.key()+ " : "+m.value());
//    }
    QMapIterator<QString, seasonTicket> m(mapSTctkt);
    int k = 1;
    while(m.hasNext()){
        m.next();
        //ui->textEdit_GenNum->append(m.value().num+"  "+m.value().barcode+"  "+m.value().encodebarcode+"  "+QString::number(m.value().isPrn));
        k++;
    }
    QApplication::restoreOverrideCursor();
}

void WgtGenNum::sltCreateBarcode()
{
    QString file;
    QString strPath;
    strPath = QApplication::applicationDirPath()+tr("/Archives/");
    QDir dirPath;
    if(!dirPath.exists(strPath)){
     dirPath.mkpath(strPath);
    }
    QString strDate = QDate::currentDate().toString("yyyyMMdd");
    QString cntLst = QString::number(ui->horizontalSlider->value());
    file = ui->comboBox->currentText()+"_"+strDate+"_"+cntLst+".dat";
//    file = ui->comboBox->currentText()+"_"+strDate+"_"+cntLst+".csv";
//    file =  QFileDialog::getOpenFileName(this,tr("Открыть..."),strPath,tr("Файлы (*.csv)"));
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл"),
                               strPath+file,
                               tr("файлы (*.dat)"));
    if(fileName.isEmpty()) return;

    int cnt = ui->horizontalSlider->value();
    QString b,frs,scd,thr,fr,fv;
    QString strTmp;
    QString barCode;
    QString seria;
    QString sNum;
    QString sI;

    snTckt tmpTckt;
    QList<snTckt> crtLst;

    mdl->clearMdl();
//    lstGenNum.clear();
    mapCode.clear();
//    mapSTctkt.clear();
    seria = ui->comboBox->currentText();
    int i = 1;
    int chckSum = -1;
//qDebug()<<cnt;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    while( i<=cnt){
        b   = QString::number(rnd(1,9));
        frs = QString::number(rnd(1,999));
        scd = QString::number(rnd(1,999));
        thr = QString::number(rnd(1,999));
        fr  = QString::number(rnd(1,999));
        fv  = QString::number(rnd(1,99));

        strTrio(frs,0);
        strTrio(scd,1);
        strTrio(thr,0);
        strTrio(fr,1);
        strTwo(fv,0);
        //strTmp = b+frs+scd+thr+fr;
        //strTmp = frs+scd+thr+fr;
        //strTmp = seria+frs+scd+thr;
        //strTmp = "s"+frs+scd+thr+fv;
        strTmp = seria+frs+scd+thr+fv;
//qDebug()<<strTmp;
//        chckSum = calcCheckSumEAN13(strTmp);
//        barCode = strTmp+QString::number(chckSum);

        barCode =  encodeBarCODE128B(strTmp);
//qDebug()<<strTmp<<barCode;
//        if(lstGenNum.contains(strTmp)){
//            qDebug()<<"---------------------------------уже есть";
//           continue;
//        }
//        lstGenNum.append(strTmp);
        if(mapCode.contains(strTmp)){
            //qDebug()<<"---------------------------------уже есть";
            continue;
        }
        mapCode.insert(strTmp,barCode);

        sI = QString::number(i).trimmed();
        if(sI.length()<4){
            sI = sI.rightJustified(4,'0');
        }
        sNum = seria+sI; //QString::number(i);
/*        sTckt.clear();
        sTckt.setNum(sNum);
        sTckt.setBarcode(strTmp);
        sTckt.setEncodeBarcode(barcode);
        sTckt.isPrn(0);
        mapSTctkt.inser(strTmp,sTckt);
*/

/*        sTckt.num = sNum;
        sTckt.barcode = strTmp;
        sTckt.encodebarcode = barCode;
        sTckt.isPrn = 0;

        mapSTctkt.insert(sI,sTckt); //strTmp
*/
        tmpTckt.num = sNum;
        tmpTckt.barcode = strTmp;
        tmpTckt.encodedBarcode = barCode;
        tmpTckt.isPrn = 0;

        crtLst.append(tmpTckt);

        i++;
        strTmp = "";
        sI = "";
        chckSum = -1;
    }
    //ui->textEdit_GenNum->clear();
//    foreach (QString s, lstGenNum) {
//        ui->textEdit_GenNum->append(s);

//    }
/*    QMapIterator<QString, QString> m(mapCode);
    while (m.hasNext()) {
        m.next();
        //cout << i.key() << ": " << i.value() << endl;
        ui->textEdit_GenNum->append(m.key()+ " : "+m.value());
    }
*/
/*    QMapIterator<QString, seasonTicket> m(mapSTctkt);
    int k = 1;
    while(m.hasNext()){
        m.next();
        ui->textEdit_GenNum->append(m.value().num+"  "+m.value().barcode+"  "+m.value().encodebarcode+"  "+QString::number(m.value().isPrn));
        k++;
    }
*/

    mdl->setFileName(fileName);
    mdl->createLst(crtLst);
    resizeView();
    ui->tableView->setFocus();
    QApplication::restoreOverrideCursor();

}

void WgtGenNum::sltPrnBarcode()
{
    if(mapCode.isEmpty()) return;
    QPrinter* prn = new QPrinter();
    QPrintDialog* pDlg = new QPrintDialog(prn,this);
    if(pDlg->exec()==QDialog::Accepted) {
        configPage(prn);

    }
    else{
        prn->setOutputFileName("barcode.pdf");
        prn->setOutputFormat(QPrinter::PdfFormat);
        configPage(prn);
    }
//    QFont fntBarCode(QFont("Code 128",24,QFont::Normal)); //16

//    QFont fntBarCode(QFont(":/Fonts/code 128",24,QFont::Normal)); //16
    QFont fntBarCode = dbFont.font("Code 128","font-style::normal",32);
    QFontMetrics fmBarCode(fntBarCode);
    int wBarCode=0;
    int hBarCode=0; //fmBarCode.height();

    QFont fntBarText("PT Sans", 8,QFont::Normal);
    QFontMetrics fmBarText(fntBarText);
    int wBarText=0;
    int hBarText=0; // fmBarText.height();
    int pgW = 0;
    int pgH = 0;



    QString barcode, encodebarcode,sNum;
    QPainter painter;//(prn);
//    QMapIterator<QString, QString> m(mapCode);
    QMapIterator<QString, seasonTicket> m(mapSTctkt);
    painter.begin(prn);
    pgW = painter.viewport().width();
    pgH = painter.viewport().height();


    while (m.hasNext()) {
        m.next();
        sNum = m.value().num;
        barcode = m.value().barcode;
        encodebarcode =  m.value().encodebarcode;
        if(barcode.isEmpty()){
            break;
        }

//qDebug()<<prn->pageSize()<<painter.viewport().width()<<painter.viewport().height();
        painter.setFont(fntBarCode);

    /*    painter.setFont(QFont("Code 128",16,QFont::Normal));
        painter.setFont(QFont("barcode font",10,QFont::Normal));
        painter.setFont(QFont("Code128bWin",10,QFont::Normal));

        painter.setFont(QFont("Code EAN13",10,QFont::Normal));
        painter.setFont(QFont("EAN-13",10,QFont::Normal));
        painter.setFont(QFont("IDAutomationHC39M",10,QFont::Normal));
    */
//        double MmToDot = 8; //Printer DPI = 203 => 8 dots per mm

        painter.setFont(fntBarCode);
        wBarCode = fmBarCode.width(encodebarcode)+20;
        hBarCode = fmBarCode.height()+fmBarCode.lineSpacing();

/*        QRect barcodeRect = QRect(pgW-(2*wBarCode),pgH-hBarCode-hBarText-15,2*wBarCode,hBarCode);
        QRect barcodeRect = QRect(40,80,2*wBarCode,hBarCode);
        QRect barcodeRect = QRect(5*MmToDot,10*MmToDot,67.5*MmToDot,10*MmToDot);

        QRect barcodeTextRect = QRect(5*MmToDot,20.5*MmToDot,67.5*MmToDot,5*MmToDot);
        QRect barcodeTextRect = QRect(barcodeRect.x(),barcodeRect.y()+barcodeRect.height()+5, barcodeRect.width(),hBarText+2);
*/

        painter.setFont(fntBarText);
        wBarText= fmBarText.width(sNum +" "+barcode);
        hBarText= fmBarText.height()+fmBarText.lineSpacing();

        QRect barcodeRect = QRect(pgW-(2*wBarCode), pgH-hBarCode-hBarText, 2*wBarCode, hBarCode);
        QRect barcodeTextRect = QRect(barcodeRect.x(), pgH-hBarText, barcodeRect.width(), hBarText);

//qDebug()<<"barcodeRect "<<barcodeRect<<"barcodeTextRect "<<barcodeTextRect;

        painter.setFont(fntBarCode);
        painter.drawText(barcodeRect, Qt::AlignCenter, encodebarcode);

        painter.setFont(fntBarText);
        painter.drawText(barcodeTextRect,Qt::AlignCenter,sNum+"  "+barcode);


    //    painter.setFont(QFont("PT Sans", 10));
    //    painter.drawText(barcodeTextRect, /*Qt::AlignCenter,*/ barcode); //
    //=====================


    /*painter.save();
        painter.translate(barcodeRect.x(),barcodeRect.y()+barcodeRect.width()/4);
        painter.rotate(-90);
        painter.drawText(QRect(0,0,barcodeRect.width(),barcodeRect.height()),encodebarcode);
    painter.restore();

    painter.save();
    qDebug()<<"barcodeTextRect "<<barcodeTextRect<<painter.window();
    //        painter.drawText(barcodeTextRect,  "text 00");
        painter.setFont(fntBarText);
        //painter.setFont(QFont("PT Sans", 10))
        painter.translate(barcodeTextRect.x(),barcodeTextRect.y());
        painter.rotate(-90);
    qDebug()<<"barcodeTextRect "<<barcodeTextRect;
        painter.drawText(QRect(0,0,barcodeTextRect.width(),barcodeTextRect.height()),  "text 10");
    painter.restore();
    */
        if(m.hasNext()) prn->newPage();

    }

    painter.end();
//qDebug() << "Printing finished";
    delete pDlg;
    delete prn;
}

void WgtGenNum::sltPrnBarcodeMdl()
{
    if(ui->radioButton_All->isChecked()){
        prnAll();
    }
    else if(ui->radioButton_Select->isChecked()){
        prnSelect();
    }
}

void WgtGenNum::sltSaveFile()
{

    QString file;
    QString strPath;
    strPath = QApplication::applicationDirPath()+tr("/Archives/");
    QDir dirPath;
    if(!dirPath.exists(strPath)){
     dirPath.mkpath(strPath);
    }
    QString strDate = QDate::currentDate().toString("yyyyMMdd");
    QString cnt = QString::number(ui->horizontalSlider->value());
    file = ui->comboBox->currentText()+"_"+strDate+"_"+cnt+".csv";
 //   file =  QFileDialog::getOpenFileName(this,tr("Открыть..."),strPath,tr("Файлы (*.csv)"));
//    strPath = QFileDialog::getExistingDirectory(this,tr("Выбирете папку..."),strPath,QFileDialog::ShowDirsOnly);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл"),
                               strPath+file,
                               tr("CSV файлы (*.csv)"));
//qDebug()<<fileName;
    QFile fName(fileName);
    if (fName.open(QIODevice::WriteOnly) )
    {
        QTextStream stream(&fName);

        QString barcode;
        QString encodebarcode;

        QMapIterator<QString, QString> m(mapCode);

        while (m.hasNext()) {
            m.next();
            barcode = m.key();
            encodebarcode =  m.value();


            stream << barcode<<";"<<encodebarcode<<"\n";
        }
        if (stream.status() != QTextStream::Ok)
        {
//            qDebug() << "Ошибка записи файла";
        }
        fName.close();

    }
}

void WgtGenNum::sltLoadNum()
{
    QString strPath;
    strPath = QApplication::applicationDirPath()+tr("/Archives/");
    QDir dirPath;
    if(!dirPath.exists(strPath)){
     dirPath.mkpath(strPath);
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Выбрать файл"),
                                                    strPath,
                                                    tr("Файлы (*.dat)"));
//qDebug()<<"sltLoadNum()"<<fileName;
   if(fileName.isEmpty()) return;

   QApplication::changeOverrideCursor(Qt::WaitCursor);
   mdl->setIsDirty(false);
   mdl->load(fileName);
   ui->tableView->setModel(NULL);
   ui->tableView->setModel(mdl);
   resizeView();
   ui->tableView->setFocus();
   QApplication::restoreOverrideCursor();
}

void WgtGenNum::sltModeSelectRows(bool b)
{
    if(ui->radioButton_All->isChecked()){
        ui->label_bNum->setVisible(!b);
        ui->lineEdit_bNum->setVisible(!b);
        ui->label_eNum->setVisible(!b);
        ui->lineEdit_eNum->setVisible(!b);
        ui->pushButton_Select->setVisible(!b);
        ui->pushButton_UnSelect->setVisible(!b);

    }
    else{
        ui->label_bNum->setVisible(b);
        ui->lineEdit_bNum->setVisible(b);
        ui->label_eNum->setVisible(b);
        ui->lineEdit_eNum->setVisible(b);
        ui->pushButton_Select->setVisible(b);
        ui->pushButton_UnSelect->setVisible(b);


    }
}

void WgtGenNum::sltSelectRows()
{
    if(mdl->rowCount()<=0) {
        QMessageBox::critical(NULL,"ВНИМАНИЕ","Отсутвуют данные");
        return;
    }
    int iB;
    int iE;
    bool ok = false;
    QString sB;
    QString sE;



    iB = ui->lineEdit_bNum->text().toInt(&ok);
    if(ok && iB!=0){
        if( (sB = ui->lineEdit_bNum->text().trimmed()).length()<4){
            sB = sB.rightJustified(4,'0');
        }
    }
    iE = ui->lineEdit_eNum->text().toInt(&ok);
    if(ok && iE!=0){
        if ( (sE = ui->lineEdit_eNum->text().trimmed()).length()<4){
            sE = sE.rightJustified(4,'0');
        }
    }
    if(iB>mdl->rowCount() || iE>mdl->rowCount()){
        QMessageBox::critical(NULL,"ВНИМАНИЕ","Введенные значения не должны превышать: "+QString::number(mdl->rowCount()));
        return;
    }
    if(iB<=iE && iE<=MAXCOUNT){

//        qDebug()<<iB<<iE<<sB<<sE;
        int i = iB-1;
        int k = iE-1;
        //ui->tableView->selectRow(i);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        ui->tableView->setFocus();
        while(i<=k){
            for(int j = 0; j<mdl->rowCount(); j++){
                ui->tableView->selectionModel()->select(mdl->index(i,j), QItemSelectionModel::Select /*QItemSelectionRange::*/);
            }
            i++;
        }
        QApplication::restoreOverrideCursor();
    }
}

void WgtGenNum::sltUnSelectRows()
{
    ui->tableView->clearSelection();
}

void WgtGenNum::sltSaveFilefromMdl()
{
    int cnt = 0;
    QString barcode="";
    QString num="";
    QString seria="";

    cnt = mdl->rowCount();
    if(cnt<=0){
        QMessageBox::critical(NULL,"ВНИМАНИЕ","Отсутвуют данные");
        return;
    }

    int r = 0;
//    r = ui->radioButton_File->isChecked() ? 0:1;
    r = ui->buttonGroup->checkedId();
    switch (r) {
    case 0:
    {
        QString file;
        QString strPath;
        strPath = QApplication::applicationDirPath()+tr("/Archives/");
        QDir dirPath;
        if(!dirPath.exists(strPath)){
            dirPath.mkpath(strPath);
        }
        QString strDate = QDate::currentDate().toString("yyyyMMdd");
        num = mdl->index(0,bc).data().toString();
        seria = num.left(1);
        file = seria+"_"+strDate+"_"+QString::number(cnt)+".csv";
        QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить файл"),
                                                        strPath+file,
                                                        tr("CSV файлы (*.csv)"));
        if(fileName.isEmpty()){
            return;
        }
        //qDebug()<<fileName;
        QFile fName(fileName);
        if (fName.open(QIODevice::WriteOnly) )
        {
            QTextStream stream(&fName);

            QString barcode;
            //QString encodebarcode;

            for(int n=0; n<cnt; n++){
                barcode = mdl->index(n,bc).data().toString();
                stream<<barcode<<"\r\n";
            }
            if (stream.status() != QTextStream::Ok)
            {
                //            qDebug() << "Ошибка записи файла";
            }
            fName.close();
        }

    }

        break;
    case 1:
    {
        if(!m_db.isOpen()){
            //qDebug()<<db.lastError();
            //QMessageBox::critical(NULL,"ВНИМАНИЕ","Нет соединения !!!");
            if(!m_db.open()){
                QMessageBox::critical(NULL,"ВНИМАНИЕ","Нет соединения !!!");
                return;
            }

        }
//        else{
//qDebug()<<"ok open Db";
            QSqlQuery qry(m_db);
//            qry.prepare("insert into tickets (barcode) values(:bc);");
            qry.prepare("insert into owners (id_ticket_type, ticket, id_owner_type, name) values(:t_type, :ticket, :o_type, :name );");

            //QVariantList lstBarcode;
            //int cntExec= 0;
            QProgressDialog dlgBar("Импорт данных...","Отмена",0,cnt,this);
            dlgBar.setWindowModality(Qt::WindowModal);
            int t_type = 1;
            int o_type = 1;
            QString nm = "абонемент";

            for(int n=0; n<cnt; n++){
                barcode = mdl->index(n,bc).data().toString();
//                qry.bindValue(":bc",barcode);
                qry.bindValue(":t_type",t_type);
                qry.bindValue(":ticket",barcode);
                qry.bindValue(":o_type",o_type);
                qry.bindValue(":name", nm);

                if(dlgBar.wasCanceled()){
                    break;
                }
                if(!qry.exec()){
                    //qDebug()<<qry.lastError();
                    QString msg = qry.lastError().text();
                    QMessageBox::critical(NULL,"ВНИМАНИЕ",msg);
                    dlgBar.reject();
                    return;
                }
                dlgBar.setValue(n);
//                lstBarcode<<barcode;
//qDebug()<<barcode;
//qDebug() << qry.boundValues();
//qDebug() << qry.executedQuery();
            }
            dlgBar.setValue(cnt);
//BATCH
/*            qry.bindValue(":bc",lstBarcode);
            if(!qry.execBatch()){
                qDebug()<<qry.lastError();
            }*/
//BATCH END
       //}
        //db.close();
    }
    default:
        break;
    }

}
