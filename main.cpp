#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/img/logoE48.png"));
    a.setOrganizationName("HK_Enisey");
    a.setApplicationName("HKE");

    MainWindow w;
    w.show();

    return a.exec();
}
