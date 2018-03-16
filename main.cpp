#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.loadCsvFile("../data/test001.csv");
    w.show();

    return a.exec();
}
