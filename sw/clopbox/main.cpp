#include "mainwindow.h"
#include <QApplication>
#include "hwcontrol.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HwControl hw;
    MainWindow w;
    w.show();

    return a.exec();
}
