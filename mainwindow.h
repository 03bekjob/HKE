#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>

//class QMdiArea;
class MdiArea;

class QMdiSubWindow;
class QAction;
class QActionGroup;
class QMenu;
class QToolBar;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    QSqlDatabase    m_db;
    void createAction();
    void createMenu();
    void createToolBar();
    void createConnect();

    void addWgt(QWidget* w);      // добовляет в MDI новое дочернее окно,т.е. создает окно QMdiSubWindow, помещает
                                  // в него w
    QWidget*    activeWgt();      // возвращает, хранящейся в активном дочернем окне

//    QMdiArea* pMdiA;
    MdiArea* pMdiA;

    QMenu* p_mWork;
    QMenu* p_mService;
    QMenu* p_mWin;
    QToolBar* pTB;
    bool     isHiddenToolBar;

    QAction* p_aExit;
    QAction* p_aGenNum;
    QAction* p_aShowTab;

    QAction*      p_aAllClsW;    // Закрыть все окна
    QAction*      p_aClsW;       // Закрыть активное окно
    QAction*      p_aCsdWin;     // Сделать каскад
    QAction*      p_aTilWin;     // Сделать мозаику
    QAction*      p_aPopWin;     // Сделать активным окно
    QActionGroup* p_aWinGrp;
    QAction* p_aTB;

    void readSettings();
    void writeSetings();

private slots:
    void sltExit();
    void sltExitSubWin();
    void sltUpdActions();
    void sltTB();
    void sltPopWin();

    void sltGenNum();
    void sltShowTable();

    void sltCloseSubWin();


};

#endif // MAINWINDOW_H
