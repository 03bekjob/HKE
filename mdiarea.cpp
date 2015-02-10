#include "mdiarea.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMdiSubWindow>
#include "wgtgennum.h"
#include <QMessageBox>

//#include <QDebug>

MdiArea::MdiArea(QWidget *parent) :
    QMdiArea(parent)
{
}

void MdiArea::paintEvent(QPaintEvent *paintEvent)
{

    QPainter painter(viewport());
    painter.fillRect(rect(), QColor(158,205,233));
    int side = qMin(width(), height())-100;
    painter.setViewport((width() - side) / 2, (height() - side) / 2,  side, side);
    QImage img(side,side,QImage::Format_ARGB32_Premultiplied);
    img.load(":/img/logoE.png"); //:/img/logoE56.png
 //    QSize sz(rect().width(),rect().height());
 //    img.scaled(sz,Qt::KeepAspectRatioByExpanding);
    painter.drawImage(rect(),img);
}

void MdiArea::sltCloseAllSubWin()
{
//qDebug()<<"MdiArea::sltCloseAllSubWin()";
    QWidget* actWgt;
    QList<QMdiSubWindow *> lstSubWin;
    lstSubWin = this->subWindowList();
    foreach (QMdiSubWindow* subWin, lstSubWin) {
        actWgt = subWin->widget();
        if(actWgt->objectName() == "WgtGenNum"){
            WgtGenNum* wgt = qobject_cast<WgtGenNum*>(actWgt);
            if(wgt->getMdl()){
                if(wgt->getMdl()->IsDirty()){
                    QMessageBox msgBox;
                    msgBox.setText("В документе были сделаны отметки о печати");
                    msgBox.setInformativeText("Сохранить отметки в файле ?");
                    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                    msgBox.setDefaultButton(QMessageBox::Save);
                    msgBox.setButtonText(QMessageBox::Save,"Сохранить");
                    msgBox.setButtonText(QMessageBox::Discard,"Выйти без сохранения");
                    msgBox.setButtonText(QMessageBox::Cancel,"Отменить");
                    int ret = msgBox.exec();
                    switch (ret) {
                    case QMessageBox::Save:
                        wgt->getMdl()->save();
                        wgt->getMdl()->clearMdl();
                        break;
                    case QMessageBox::Discard:
                        wgt->getMdl()->clearMdl();
                        break;
                    case QMessageBox::Cancel:
                        return;
                    default:
                        break;
                    }
                }
            }
            wgt->getMdl()->clearMdl();
        }
    }
    this->closeAllSubWindows();
    emit sgnCloseSubWin();
}

void MdiArea::sltCascadeSubWin()
{
    this->cascadeSubWindows();
    emit sgnCloseSubWin();
}

void MdiArea::sltTileSubWin()
{
    this->tileSubWindows();
    emit sgnCloseSubWin();
}

void MdiArea::sltCloseSubWin()
{
    QWidget* actWgt = this->activeSubWindow()->widget();
    if(actWgt){
//qDebug()<<actWgt->objectName();
        if(actWgt->objectName() == "WgtGenNum"){
            WgtGenNum* wgt = qobject_cast<WgtGenNum*>(actWgt);
            if(wgt->getMdl()){
                if(wgt->getMdl()->IsDirty()){
                    QMessageBox msgBox;
                    msgBox.setText("В документе были сделаны отметки о печати");
                    msgBox.setInformativeText("Сохранить отметки в файле ?");
                    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                    msgBox.setDefaultButton(QMessageBox::Save);
                    msgBox.setButtonText(QMessageBox::Save,"Сохранить");
                    msgBox.setButtonText(QMessageBox::Discard,"Выйти без сохранения");
                    msgBox.setButtonText(QMessageBox::Cancel,"Отменить");
                    int ret = msgBox.exec();
                    switch (ret) {
                    case QMessageBox::Save:
                        wgt->getMdl()->save();
                        wgt->getMdl()->clearMdl();
                        break;
                    case QMessageBox::Discard:
                        wgt->getMdl()->clearMdl();
                        break;
                    case QMessageBox::Cancel:
                        return;
                    default:
                        break;
                    }
                }
            }
            wgt->getMdl()->clearMdl();
        }
    }
    this->closeActiveSubWindow();

    emit sgnCloseSubWin();
}


