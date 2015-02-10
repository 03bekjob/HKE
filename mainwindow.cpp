#include "mainwindow.h"
#include <QApplication>
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>

//#include <QMdiArea>
#include "mdiarea.h"

#include <QToolBar>
#include <QMdiSubWindow>
#include <QSettings>
#include <QVariant>
//#include <QPalette>
//#include <QPixmap>
//#include <QBrush>
//#include <QRect>
//#include <QSize>
//#include <QDesktopWidget>


#include "wgtgennum.h"
#include "wgtshowtable.h"
#include <QSqlDatabase>
#include <QMessageBox>

//#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //
    this->setWindowTitle("Генерация номеров");
    //this->setWindowIcon(QIcon(":/img/.png"));
//    pMdiA = new QMdiArea(this);
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("/home/bek03/SeasonTickets.db");   // C:/SQLite3/db.sqlite C:/SQLite3/SeasonTickets.db /home/bek03/SeasonTickets.db
//    if(!m_db.open()){
//        QMessageBox::information(NULL,"ВНИМАНИЕ","Нет соединения !!!");
//    }

    pMdiA = new MdiArea(this);

    readSettings();

    createAction();
    createMenu();
    createToolBar();
    createConnect();


    setCentralWidget(pMdiA);
    pMdiA->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    pMdiA->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    sltUpdActions();

}

MainWindow::~MainWindow()
{
    writeSetings();
}

void MainWindow::createAction()
{
    p_aExit = new QAction(QIcon(":/img/exit.png"),tr("Выход"),this);
    p_aTB = new QAction(tr("Выкл\\Вкл ToolBar"),this);

    p_aGenNum = new QAction(QIcon(":/img/barcode.png"),tr("Генерация номеров"),this);
    p_aShowTab= new QAction(QIcon(":/img/table32.png"),tr("Посмотреть таблицу номеров"),this);

    p_aAllClsW = new QAction(QIcon(":/img/closeall.png"),tr("Закрыть все окна"),this); // Закрыть все окна
    p_aClsW    = new QAction(QIcon(":/img/closeone.png"),tr("Закрыть окно"),this);     // Закрыть активное окно
    p_aCsdWin  = new QAction(QIcon(":/img/cascade.png"),tr("Каскад"),this);            // Сделать каскад
    p_aTilWin  = new QAction(QIcon(":/img/tile.png"),tr("Мозаика"),this);              // Сделать мозаику
    p_aPopWin  = new QAction(QIcon(":/img/turn.png"),tr("Одно окно"),this);            // Сделать активным окно
    p_aWinGrp  = new QActionGroup(this);


}

void MainWindow::createMenu()
{
    p_mWork = menuBar()->addMenu(tr("Работа"));
    p_mWork->addAction(p_aGenNum);
    p_mWork->addAction(p_aShowTab);
    p_mWork->addSeparator();
    p_mWork->addAction(p_aExit);

    p_mWin = menuBar()->addMenu(tr("Окна"));
    p_mWin->addAction(p_aCsdWin);
    p_mWin->addAction(p_aTilWin);
    p_mWin->addAction(p_aPopWin);
    p_mWin->addSeparator();
    p_mWin->addAction(p_aClsW);
    p_mWin->addAction(p_aAllClsW);
    p_mWin->addSeparator();

    p_mService = menuBar()->addMenu(tr("Сервис"));
    p_mService->addAction(p_aTB);

}

void MainWindow::createToolBar()
{
    pTB = new QToolBar(this);
    pTB->setObjectName("toolBar");
    this->addToolBar(Qt::RightToolBarArea,pTB);
    pTB->addAction(p_aExit);

    pTB->addSeparator();
    pTB->addAction(p_aCsdWin);
    pTB->addAction(p_aTilWin);
    pTB->addAction(p_aPopWin);

    pTB->addSeparator();
    pTB->addAction(p_aClsW);
    pTB->addAction(p_aAllClsW);

    addToolBar(Qt::TopToolBarArea,pTB);
    if(isHiddenToolBar){
        pTB->hide();
    }
    else{
        pTB->show();
    }
   p_aTB->setChecked(isHiddenToolBar);

}

void MainWindow::createConnect()
{
    connect(p_aExit,SIGNAL(triggered()),this,SLOT(sltExit()));
    connect(p_aTB,SIGNAL(triggered()),this,SLOT(sltTB()));
    connect(pMdiA, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(sltUpdActions()));
    connect(pMdiA,SIGNAL(sgnCloseSubWin()),this,SLOT(sltCloseSubWin()));

    connect(p_aGenNum,SIGNAL(triggered()),this,SLOT(sltGenNum()));
    connect(p_aShowTab,SIGNAL(triggered()),this,SLOT(sltShowTable()));

    //Окна
    connect(p_aAllClsW,SIGNAL(triggered()),pMdiA,SLOT(sltCloseAllSubWin()));  //closeAllSubWindows()
    connect(p_aClsW,SIGNAL(triggered()),pMdiA,SLOT(sltCloseSubWin()));        //closeActiveSubWindow()
    connect(p_aCsdWin,SIGNAL(triggered()),pMdiA,SLOT(sltCascadeSubWin()));    //cascadeSubWindows()
    connect(p_aTilWin,SIGNAL(triggered()),pMdiA,SLOT(sltTileSubWin()));       //tileSubWindows()
    connect(p_aPopWin,SIGNAL(triggered()),this,SLOT(sltPopWin()));

}

void MainWindow::addWgt(QWidget *w)
{
    QMdiSubWindow* subW=NULL;
    WgtGenNum*    wGenNum;
    WgtShowTable* wShowTable;
//qDebug()<<"MainWindow::addWgt"<<w->objectName();
    if(w->objectName()=="WgtGenNum"){
        wGenNum = qobject_cast<WgtGenNum*> (w);
        subW = pMdiA->addSubWindow(wGenNum);
        p_mWin->addAction(wGenNum->getMenuAction());
        p_aWinGrp->addAction(wGenNum->getMenuAction());
        connect(wGenNum,SIGNAL(sgnExit()),pMdiA,SLOT(sltCloseSubWin())); //closeActiveSubWindow()

    }
    if(w->objectName()=="WgtShowTable"){
        wShowTable = qobject_cast<WgtShowTable*> (w);
        subW = pMdiA->addSubWindow(wShowTable);
        p_mWin->addAction(wShowTable->getMenuAction());
        p_aWinGrp->addAction(wShowTable->getMenuAction());
        connect(wShowTable,SIGNAL(sgnExit()),pMdiA,SLOT(sltCloseSubWin())); //closeActiveSubWindow()

    }

    subW->showMaximized();
}

QWidget *MainWindow::activeWgt()
{
    QMdiSubWindow* subW = pMdiA->activeSubWindow();
//qDebug()<<"*MainWindow::activeWgt()"<<subW->widget()->objectName();
    if(subW){
        if(subW->widget()->objectName()=="WgtGenNum"){
            return qobject_cast<WgtGenNum*>(subW->widget());
        }
        else if(subW->widget()->objectName()=="WgtShowTable"){
            return qobject_cast<WgtShowTable*>(subW->widget());
        }

    }
    return NULL;
}

void MainWindow::readSettings()
{
    QSettings st;
    QString key,keyH,keyG; //,keyTBGeometry;
    key = tr("MainWindow");

    keyH= key + tr("/ToolBar/isHidded");
    if(st.value(keyH).isNull()){
        isHiddenToolBar = false;
    }
    else {
        QVariant v;
        isHiddenToolBar = st.value(keyH,v).toBool();
    }


    keyG   = key+"/geometry";
    if(!st.contains(keyG) ){
            return;
    }
//    if(st.value(keyG).toRect().isEmpty()){
//        return;
//    }

    //this->setGeometry(st.value(keyG).toRect());
    this->restoreGeometry(st.value(keyG).toByteArray());
}

void MainWindow::writeSetings()
{
    QSettings st;
    QString key,keyH,keyG; //,keyTBGeometry;

    key = tr("MainWindow");

    keyG  = key+"/geometry";
    st.setValue(keyG,this->saveGeometry()/*this->geometry()*/);

    keyH= key+tr("/ToolBar/isHidded");
    st.setValue(keyH,isHiddenToolBar);



//qDebug()<<sKeyG<<sKeyR;


}

void MainWindow::sltExit()
{
    if(pMdiA){
        //pMdiA->closeAllSubWindows();
        pMdiA->sltCloseAllSubWin();
    }
    if(m_db.isOpen()){
        m_db.close();
    }
    qApp->exit();

}

void MainWindow::sltExitSubWin()
{
    pMdiA->closeActiveSubWindow();
}

void MainWindow::sltUpdActions()
{
    QWidget* actWgt = activeWgt();
//    bool b =  (activeWgt() != NULL);
    bool b =  (actWgt != NULL);
    p_aAllClsW->setEnabled(b);
    p_aClsW->setEnabled(b);
    p_aCsdWin->setEnabled(b);
    p_aPopWin->setEnabled(b);
    p_aTilWin->setEnabled(b);

    if(actWgt){
        if(actWgt->objectName() == "WgtGenNum"){
            WgtGenNum* wgt = qobject_cast<WgtGenNum*>(actWgt);
            wgt->getMenuAction()->setChecked(true);
        }
        if(actWgt->objectName() == "WgtShowTable"){
            WgtShowTable* wgt = qobject_cast<WgtShowTable*>(actWgt);
            wgt->getMenuAction()->setChecked(true);
        }
    }

}

void MainWindow::sltTB()
{
    isHiddenToolBar  = isHiddenToolBar ? false:true;
    if(isHiddenToolBar){
        pTB->hide();
    }
    else{
        pTB->show();
    }
    p_aTB->setChecked(isHiddenToolBar);
}

void MainWindow::sltPopWin()
{
    if(pMdiA){
        if(!pMdiA->subWindowList().isEmpty()){
            pMdiA->activeSubWindow()->showMaximized();
        }
    }

}

void MainWindow::sltGenNum()
{
    WgtGenNum* w = new WgtGenNum(m_db,this);
//qDebug()<<"MainWindow::sltGenNum()";
    addWgt(w);
}

void MainWindow::sltShowTable()
{
    WgtShowTable* w = new WgtShowTable(m_db,this);
    addWgt(w);
}

void MainWindow::sltCloseSubWin()
{
//qDebug()<<"MainWindow::sltCloseSubWin()";
    QRect rct = this->geometry();
    rct.setX(rct.x()+1);
    this->setGeometry(rct);
    rct.setX(rct.x());
    this->setGeometry(rct);

}
