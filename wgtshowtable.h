#ifndef WGTSHOWTABLE_H
#define WGTSHOWTABLE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QModelIndexList>
#include <QModelIndex>

namespace Ui {
class WgtShowTable;
}

class WgtShowTable : public QWidget
{
    Q_OBJECT

public:
    explicit WgtShowTable(/*const*/ QSqlDatabase& m_db, QWidget *parent = 0);
    ~WgtShowTable();
    QAction* getMenuAction() const { return act; }
signals:
    void sgnExit();

private:
    Ui::WgtShowTable *ui;
//    QSqlDatabase    m_db;
    QAction* act;
    QSqlTableModel* mdl;
    QModelIndexList lstIdx;
    QModelIndex curIdx;
    int curColFind;

    void createConnect();
    void refresh();
    void resizeView();
private slots:
    void sltExit();
    void sltRefresh();
    void sltChangedText(QString txt);
    void sltCleareFind();
    void sltFindNext();
    void sltDel();

};

#endif // WGTSHOWTABLE_H
