#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    arduino_is_available = true;
    arduino_port_name = "";
    arduino = new QSerialPort;
    serialBuffer = "";
   // parsed_data = "";
    parsed_x_data = "";
    parsed_y_data = "";
    parsed_z_data = "";
    axis_x_value = 0.0;
    axis_y_value = 0.0;
    axis_z_value = 0.0;



    qDebug() << "Number of available ports: " << QSerialPortInfo::availablePorts().length();

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        qDebug() << "Has vendor ID: " << serialPortInfo.hasVendorIdentifier();
        if(serialPortInfo.hasVendorIdentifier()){
            qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
        }
        qDebug() << "Has product ID: " << serialPortInfo.hasProductIdentifier();
        if(serialPortInfo.hasProductIdentifier()){
            qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
        }

    }

//    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
//        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
//            if(serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id){
//                if(serialPortInfo.productIdentifier() == arduino_uno_product_id){
//                    arduino_port_name = serialPortInfo.portName();
//                    arduino_is_available = true;
//                }
//            }
//        }
//    }

    if(arduino_is_available){
        //open and conf the serial port
        arduino->setPortName("COM6");
        arduino->open(QSerialPort::ReadOnly);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
    }else{
        //give error msg if not available
        QMessageBox::warning(this, "Po7rt error", "Couldn't find the arduino");
    }

}

MainWindow::~MainWindow()
{
    if(arduino->isOpen()){
        arduino->close();
    }
    delete ui;
}

void MainWindow::readSerial(){
    QStringList buffer_split = serialBuffer.split(",");
    if(buffer_split.length() < 3){
        // no parsed value yet so continue accumulating bytes from serial in the buffer.
        serialData = arduino->readAll();
        serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
        //qDebug() << serialBuffer;
        serialData.clear();
    }else{
       serialBuffer = "";
       parsed_x_data = buffer_split[0];
       parsed_y_data = buffer_split[1];
       parsed_z_data = buffer_split[2];
       axis_x_value = parsed_x_data.toDouble();
       axis_y_value = parsed_y_data.toDouble();
       axis_z_value = parsed_z_data.toDouble();
//       qDebug() << axis_x_value;
//       qDebug() << axis_y_value;
//       qDebug() << axis_z_value;

       x.append(axis_x_value);
//       y.append(axis_y_value);
//       z.append(axis_z_value);
       xg.append(count);

       ui->xlcd->display(axis_x_value);
       ui->ylcd->display(axis_y_value);
       ui->zlcd->display(axis_z_value);
       count += 1;


       buffor(60); // można zmienić

    }
}

void MainWindow::plot(){
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->widget_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    ui->widget->addGraph();
    ui->widget->addGraph();
    ui->widget->addGraph();
    ui->widget_2->addGraph();
    ui->widget_2->addGraph();

    ui->widget->graph(0)->setData(xg,x);
    ui->widget->graph(1)->setData(xg,avg_filter);
    ui->widget_2->graph()->setData(xg,signal);
    ui->widget_2->graph(1)->setData(xg,swing);

    ui->widget->rescaleAxes();
    ui->widget_2->rescaleAxes();


    ui->widget->setInteraction(QCP::iRangeDrag,true);
    ui->widget->setInteraction(QCP::iRangeZoom,true);

    ui->widget->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->widget_2->xAxis->grid()->setZeroLinePen(Qt::NoPen);

    ui->widget->graph(0)->setPen(QPen(Qt::cyan));
    ui->widget->graph(1)->setPen(QPen(Qt::gray));
    ui->widget_2->graph()->setPen(QPen(Qt::darkGreen));
    ui->widget_2->graph(1)->setPen(QPen(Qt::red));

    ui->widget_2->graph()->setLineStyle((QCPGraph::LineStyle)QCPGraph::lsStepLeft);
}

void MainWindow::buffor(int prog){

    if(count >= prog){
       x.pop_front();
//       y.pop_front();
//       z.pop_front();

       xg.pop_front();
       n = x.size();
       adjustSize();
       peakDetect();

       plot();
       signal.pop_front();
       swing.pop_front();
       ui->widget->replot();
       ui->widget_2->replot();
    }


}

double MainWindow::std(QVector<double>::iterator start, QVector<double>::iterator end){
                double sum = std::accumulate(start,end, .0);
                double ilosc = std::distance(start,end);
                double mean = sum/ilosc;
                QVector<double> diff(ilosc);
                /*std::transform applies the given function to a range and stores the result in another range*/
                std::transform(start, end, diff.begin(), [mean](double x) { return x - mean; });
                /*suma kwadratów*/
                double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0); //
                double std_dev = std::sqrt(sq_sum / ilosc);
                return std_dev;
}

double MainWindow::mean( QVector<double>::iterator start, QVector<double>::iterator end){
    double sum = std::accumulate(start,end, .0);
    double ilosc = std::distance(start,end);
    double average = sum/ilosc;
    return average;
}

void MainWindow::adjustSize(){

    avg_filter.resize(n);
    std_filter.resize(n);
    signal.resize(n);
    swing.resize(n);
    //output_filter.resize(n);
    //dev.resize(n);
    //dev2.resize(n);

}

void MainWindow::peakDetect(){
        //inicjalizacja sredniej i odchylenia
        avg_filter[window_length-1] = mean(x.begin(), x.begin()+window_length);
        std_filter[window_length-1] = std(x.begin(), x.begin()+window_length);

        for (int i = window_length+1; i < n; i++) {
           if(avg_filter[i] > 30){ // pierwszy próg
                signal[i] = 1;  //wystapienie chodu - zielony wykres

                if(x[i] > 40){ // drugi próg
                    swing[i] = 1;
                    //wystapienie fazy swing (z artykulu: This phase starts when the toe
                    //of the supported limb leaves the ground and ends when the heel of
                    //the same limb makes contact with the ground again) - rozowy wykres

                }
                else{
                    swing[i] = 0;
                }
           }
           else signal[i] = 0; //nie ma chodu

           //aktualizacja sredniej i odchylenia
           avg_filter[i] = mean(x.begin() + (i-window_length), x.begin()+i);
           std_filter[i] = std(x.begin() + (i-window_length), x.begin()+i);
        }

}
/*void MainWindow::ZScore(){

    //inicialize average and std dev.
    avg_filter[window_length-1] = mean(x.begin(), x.begin()+window_length);
    std_filter[window_length-1] = std(x.begin(), x.begin()+window_length);
    //qDebug() << "mean " << avg_filter;

    for (int i = window_length+1; i < n; i++) {
       if(abs(input[i]-avg_filter[i-1]) > treshold*std_filter[i-1]){
           if(input[i] > avg_filter[i-1]){
                  signal[i] = 1;
            }
           else signal[i] = -1;
            output_filter[i] = influence * x[i] + (1 - influence) * output_filter[i-1];
        }else {
            signal[i] = 0;
            output_filter[i] = x[i];
        }
        avg_filter[i] = mean(output_filter.begin() + (i-window_length), output_filter.begin()+i);
        std_filter[i] = std(output_filter.begin() + (i-window_length), output_filter.begin()+i);
        dev[i] = avg_filter[i]+std_filter[i]*treshold;
        dev2[i] = avg_filter[i]-std_filter[i]*treshold;

    }
//    qDebug() << "output" << output_filter;
//    qDebug() << "mean" << avg_filter;
//    qDebug() << "std" << std_filter;
    qDebug() << "signal" << signal;


}*/
