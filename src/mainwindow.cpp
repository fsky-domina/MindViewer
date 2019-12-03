#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initComWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initComWidget()
{
#ifdef Q_OS_LINUX
    ComList<< "ttyUSB0"<<"ttyUSB1"<<"ttyUSB2"<<"ttyUSB3"<<"ttyS0"<<
               "ttyS1"<<"ttyS2"<<"ttyS3"<<"ttyS4"<<"ttyS5"<<"ttyS6";
#elif defined (Q_OS_WIN32)
    ComList<<"COM0"<<"COM1"<<"COM2"<<"COM3"<<"COM4"<<"COM5"<<
                "COM6"<<"COM7"<<"COM8";
#endif
    ui->boxPort->addItems(ComList);
    braudList<<"50"<<"75"<<"100"<<"150"<<"300"<<"600"<<"1200"<<"2400"
                <<"4800"<<"9600"<<"19200"<<"38400"<<"43000"<<"56000"
               <<"57600"<<"115200"<<"128000";
    ui->boxBraud->addItems(braudList);
    ui->boxBraud->setCurrentText("57600");
    ui->boxDataBit->setCurrentText("8");
    ui->boxStopBit->setCurrentText("1");
    ui->boxCheckBit->setCurrentText("NONE");
    ui->lineStauts->setText(tr("Close"));
    ui->lineStauts->setReadOnly(true);
    QFile file(":/resources/themes/light.qss");
    file.open(QIODevice::Text|QIODevice::ReadOnly);
    this->setStyleSheet(file.readAll());
    file.close();
    QString filename_log = QDateTime::currentDateTime().toString("yyyyMMdd") + ".txt";
    file.setFileName(filename_log);
    //如果打开失败则给出提示并退出函数
    if(!file.open(QFile::WriteOnly | QIODevice::Text|QIODevice::Append))
        QMessageBox::information(this,tr("提示"),tr("无法打开日志文件"),QMessageBox::Ok);
    file.close();
    write2file(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss dddd") + '\n');
    QString filename = QDir::currentPath() + QDir::separator() + "data.txt";
    if(filename.isEmpty()){
        return;
    }
}

void MainWindow::write2file(QString str)
{
    QString filename = QDateTime::currentDateTime().toString("yyyyMMdd") + ".txt";
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text |QIODevice::Append)){
        QMessageBox::information(this,tr("警告"),tr("无法打开文件，请检查"),QMessageBox::Ok);
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out<<str;
    file.close();
}

void MainWindow::readMyCom()
{

}

void MainWindow::internalTime()
{
    QDateTime intertime = QDateTime::currentDateTime();
    QString time = intertime.toString("yyyy-MM-dd hh:mm:ss dddd");
    write2file(time + '\n');
}

void MainWindow::on_actionOpen_triggered()
{

}

void MainWindow::on_actionClose_triggered()
{

}

void MainWindow::on_actionClear_triggered()
{

}

void MainWindow::on_actionPrint_triggered()
{

}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit(0);
}

void MainWindow::on_actionData_triggered()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->actionData->setEnabled(false);
    ui->actionGraph->setEnabled(true);
}

void MainWindow::on_actionGraph_triggered()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->actionData->setEnabled(true);
    ui->actionGraph->setEnabled(false);
}

void MainWindow::on_actionAbout_triggered()
{

}

void MainWindow::on_actionQt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::on_actionIndex_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/JackeyLea/MindViewer"));
}

void MainWindow::on_buttonOpen_clicked()
{
    QString portName = ui->boxPort->currentText();//获取串口名
#ifdef Q_OS_LINUX
    myCom = new QSerialPort("/dev/" + portName);
#elif defined (Q_OS_WIN)
    myCom = new QextSerialPort(portName);
#endif
    QObject::connect(myCom,SIGNAL(readyRead()),this,SLOT(readMyCom()));
    //设置波特率
    myCom->setBaudRate(ui->boxBraud->currentText().toInt());
    //设置数据位
    switch(ui->boxDataBit->currentText().toInt()){
    case 5:
        myCom->setDataBits(QSerialPort::Data5);
        break;
    case 6:
        myCom->setDataBits(QSerialPort::Data6);
        break;
    case 7:
        myCom->setDataBits(QSerialPort::Data7);
        break;
    case 8:
        myCom->setDataBits(QSerialPort::Data8);
        break;
    }

    //设置校验
    switch(ui->boxCheckBit->currentIndex())
    {
        case 0:
            myCom->setParity(QSerialPort::NoParity);
            break;
        case 1:
            myCom->setParity(QSerialPort::OddParity);
            break;
        case 2:
            myCom->setParity(QSerialPort::EvenParity);
            break;
        default:
            myCom->setParity(QSerialPort::NoParity);
            break;
    }
    //设置停止位
    switch(ui->boxStopBit->currentIndex())
    {
         case 0:
             myCom->setStopBits(QSerialPort::OneStop);
             break;
         case 1:
             #ifdef Q_OS_WIN
             myCom->setStopBits();//1.5
             #endif
             break;
         case 2:
             myCom->setStopBits(QSerialPort::TwoStop);
             break;
         default:
             myCom->setStopBits(QSerialPort::OneStop);
             qDebug("set to default : STOP_1");
             break;
    }
    //设置数据流控制
    myCom->setFlowControl(QSerialPort::NoFlowControl);
    if(myCom->open(QIODevice::ReadWrite)){
        QMessageBox::information(this, tr("打开成功"), tr("已成功打开串口") + portName, QMessageBox::Ok);
        //界面控制
        }else{
        QMessageBox::critical(this, tr("打开失败"), tr("未能打开串口 ") + portName + tr("\n该串口设备不存在或已被占用"), QMessageBox::Ok);
        return;
    }
}

void MainWindow::on_buttonClose_clicked()
{
    myCom->close();
        myCom=nullptr;
        ui->lineStauts->setText(tr("Close"));
        QMessageBox::information(this,tr("Tips"),tr("COM has been closed successfully"),QMessageBox::Ok);
}
