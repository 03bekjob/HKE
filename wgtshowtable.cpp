#include "wgtshowtable.h"
#include "ui_wgtshowtable.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QHeaderView>
#include <QModelIndexList>

//#include <QDebug>

WgtShowTable::WgtShowTable(QSqlDatabase &m_db, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WgtShowTable)
//    ,m_db(m_db)
{
    ui->setupUi(this);
    ui->radioButton_One->setChecked(true);

    setAttribute(Qt::WA_DeleteOnClose);
    if(!m_db.isOpen()){
        if(!m_db.open()){
            QMessageBox::critical(NULL,"ВНИМАНИЕ","Нет соединения !!!");
        }
    }

    QString sTtl;
    sTtl = tr("%1").arg(this->windowTitle().trimmed());

    curColFind = 2;  //по баркоду

    act = new QAction(sTtl,this);
    act->setCheckable(true);

    mdl = new QSqlTableModel(this,m_db);
    refresh();
    resizeView();

//qDebug()/*<<b*/<<db.isOpen()<<mdl->lastError()<<mdl->rowCount()<<mdl->columnCount();


    createConnect();
    ui->tableView->setModel(mdl);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);


}

WgtShowTable::~WgtShowTable()
{
    mdl->clear();
//    db.close();
    delete ui;
}

void WgtShowTable::createConnect()
{
    connect(act,SIGNAL(triggered()),this,SLOT(showMaximized()));
    connect(ui->pushButton_Exit,SIGNAL(clicked()),this,SLOT(sltExit()));

    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(sltChangedText(QString)));
    connect(ui->pushButton_FindNext,SIGNAL(clicked()),this,SLOT(sltFindNext()));
    connect(ui->pushButton_CleareFind,SIGNAL(clicked()),this,SLOT(sltCleareFind()));
    connect(ui->pushButton_Refesh,SIGNAL(clicked()),this,SLOT(sltRefresh()));

    connect(ui->pushButton_Del,SIGNAL(clicked()),this,SLOT(sltDel()));

}

void WgtShowTable::refresh()
{
   mdl->clear();
   mdl->setTable("");

   mdl->setTable("owners");//tickets
   if(mdl->select()){
       while(mdl->canFetchMore())
               mdl->fetchMore();
   }
   else{
       //qDebug()<<"FALSE select";
       QString msg=mdl->lastError().text();
       QMessageBox::critical(NULL,"ВНИМАНИЕ",msg);
   }


}

void WgtShowTable::resizeView()
{
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
}

void WgtShowTable::sltExit()
{
    //db.close();
    emit sgnExit();
}

void WgtShowTable::sltRefresh()
{
    refresh();
    resizeView();
}

void WgtShowTable::sltChangedText(QString txt)
{
   if(ui->lineEdit->text().trimmed().isEmpty()) {
       ui->label_MatchCnt->setText("0");
       ui->label_ord->setText("(0)");
       return;
   }
   if(ui->tableView->currentIndex().isValid()){
       curColFind = ui->tableView->currentIndex().column();
   }
   else{
       curColFind = 2;  // по barcode
   }
   lstIdx = mdl->match(mdl->index(0,curColFind),Qt::DisplayRole,QVariant::fromValue(txt),-1);
   ui->label_MatchCnt->setText(QString::number(lstIdx.count()));
   ui->label_ord->setText("(1)");
   if(!lstIdx.isEmpty()){
        ui->tableView->setCurrentIndex(lstIdx.at(0));
   }
   else{
       ui->label_MatchCnt->setText(QString::number(lstIdx.count()));
       ui->label_ord->setText("(0)");

   }
}

void WgtShowTable::sltCleareFind()
{
    ui->lineEdit->clear();
}

void WgtShowTable::sltFindNext()
{
    int i = lstIdx.indexOf(ui->tableView->currentIndex());
    int ord;
    i++;
    if(i<lstIdx.count()){
        ord = i+1;
        ui->label_ord->setText("("+QString::number(ord)+")");
        ui->tableView->setCurrentIndex(lstIdx.at(i));
    }
    else{
        ui->tableView->setCurrentIndex(lstIdx.at(0));
        ui->label_ord->setText("(1)");
    }
}

void WgtShowTable::sltDel()
{
    if(mdl->rowCount()<=0){
        QMessageBox::critical(NULL,"ВНИМАНИЕ","Отсутвуют данные !!!");
        return;
    }
    QModelIndex idx;
    idx = ui->tableView->currentIndex();
    int id;
    QSqlQuery qry;
    if(ui->radioButton_One->isChecked()){
        if(idx.isValid()){
           curIdx = idx;
//qDebug()<<"curIdx = "<<curIdx;
           id = mdl->index(idx.row(),0).data().toInt();
           qry.prepare("delete from owners where owners.id = :id");
           qry.bindValue(":id",id);

           if(qry.exec()){
               refresh();
               resizeView();
               if(curIdx.row() == 0){
                   ui->tableView->setCurrentIndex(mdl->index(0,0));
               }
               else{
                   ui->tableView->setCurrentIndex( mdl->index(curIdx.row()-1, 0) );
               }
               ui->tableView->setFocus();
           }
        }
        else{
            QMessageBox::critical(NULL,"ВНИМАНИЕ","Не выделена ни одна строка !!!");
        }
    }

    if(ui->radioButton_All->isChecked()){
        qry.prepare("delete from owners");
        if(QMessageBox::Ok == QMessageBox::information(NULL,"ВНИМАНИЕ","Вы действительно собираетесь очистить таблицу ?",QMessageBox::Ok | QMessageBox::No,QMessageBox::No)){ //, StandardButtons buttons = Ok, StandardButton defaultButton = NoButton)
            if(qry.exec()){
                refresh();
                resizeView();
            }
        }
    }
}

