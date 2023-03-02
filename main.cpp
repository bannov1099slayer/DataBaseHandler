#include "mainwindow.h"


#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("DbHandler");
    w.setFixedSize(1084,625);
    w.show();
    return a.exec();
}
