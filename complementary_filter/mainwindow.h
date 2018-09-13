#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QString>
#include <QtWidgets>
#include <QMessageBox>
#include <armadillo>

#include <algorithm>
#include <numeric>

using namespace arma;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void readSerial();
    void buffor(int prog);
    void plot();
    double mean(QVector<double>::iterator, QVector<double>::iterator);
    double std(QVector<double>::iterator, QVector<double>::iterator);
    void peakDetect();
    void adjustSize();
private:
    Ui::MainWindow *ui;
    QSerialPort *arduino;
    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_uno_product_id = 67;
    QString arduino_port_name;
    bool arduino_is_available;
    QByteArray serialData;
    QString serialBuffer;
    QString parsed_x_data;
    QString parsed_y_data;
    QString parsed_z_data;
    double axis_x_value;
    double axis_y_value;
    double axis_z_value;
    //do wykresów
    QVector<double> x; // kąt tzw roll - oś x - ruch do przodu z obrotem wokół osi x zaznaczonej na czujniku
    QVector<double> y;
    QVector<double> z;
    QVector<double> xg;
    int count = 0;
    //można zmienić
    int window_length = 5;
    double treshold1 = 20;
    double treshold2= 50;
    double influence = 1;

//    QVector<double> input = {1.0, 1.0, 1.1, 1.0, 0.9, 1.0, 1.0, 1.1, 1.0, 0.9, 1.0, 1.1, 1.0, 1.0, 0.9, 1.0, 1.0, 1.1, 1.0,
//                            1.0, 1.0, 1.0, 1.1, 0.9, 1.0, 1.1, 1.0, 1.0, 0.9, 1.0, 1.1, 1.0, 1.0, 1.1, 1.0, 0.8, 0.9, 1.0,
//                            1.2, 0.9, 1.0, 1.0, 1.1, 1.2, 1.0, 1.5, 1.0, 3.0, 2.0, 5.0, 3.0, 2.0, 1.0, 1.0, 1.0, 0.9, 1.0,
//                            1.0, 3.0, 2.6, 4.0, 3.0, 3.2, 2.0, 1.0, 1.0, 0.8, 4.0, 4.0, 2.0, 2.5, 1.0, 1.0, 1.0};
    int n;
    QVector<double> avg_filter;
    QVector<double> std_filter;
    QVector<double> signal;
    QVector<double> swing;
//    QVector<double> output_filter;
//    QVector<double>::iterator start,end;
//    QVector<double> dev;
//    QVector<double> dev2;
//    bool moving = false;

};

#endif // MAINWINDOW_H
