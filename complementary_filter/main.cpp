#include "mainwindow.h"
#include "vectorstats.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.setFixedSize(417,173);
    w.setWindowTitle("Complementary Filter");
    w.show();

    return a.exec();
}
