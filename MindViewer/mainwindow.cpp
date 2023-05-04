#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initUi();//初始化界面
    initConnections();//初始化信号槽
}

MainWindow::~MainWindow()
{
    delete ui;
}
//使用测试模拟数据
void MainWindow::sltActionTest()
{
//    if(ui->actionTest->isChecked()){
//        ui->actionSerialPort->setChecked(false);
//        on_actionSerialPort_triggered();
//        gen=new Generator();
//        connect(gen,&Generator::sendData,this,&MainWindow::sltReceiveData);//模拟数据
//    }else{
//        if(gen){
//            disconnect(gen,&Generator::sendData,this,&MainWindow::sltReceiveData);
//            delete gen;
//        }
    //    }
}

void MainWindow::sltActionCOM()
{
//    if(ui->actionSerialPort->isChecked()){
//        ui->actionTest->setChecked(false);
//        on_actionTest_triggered();
//        ui->graphCommon->CurveClear();
//        ui->graphEEG->CurveClear();
//        retriverWgt->showWgt();
//    }else{
//        retriverWgt->stopCOM();
    //    }
}

void MainWindow::sltActionExit()
{
    exit(0);
}

void MainWindow::sltActionHex(bool checked)
{
//    if(checked){//文本模式
//        ui->stackedWidget->setCurrentIndex(0);
//        ui->actionGraph->setChecked(false);
//        ui->textHex->clear();
//    }else{
//        ui->stackedWidget->setCurrentIndex(1);
//        ui->actionGraph->setChecked(true);
//        ui->textHex->clear();
    //    }
}

void MainWindow::sltActionGraph(bool checked)
{
//    if(checked){
//        ui->stackedWidget->setCurrentIndex(1);
//        ui->actionHex->setChecked(false);
//        ui->graphCommon->CurveClear();
//        ui->graphEEG->CurveClear();
//    }else{
//        ui->stackedWidget->setCurrentIndex(0);
//        ui->actionHex->setChecked(true);
    //    }
}

void MainWindow::sltActionAbout()
{
    QString msg = QString("<h1>MindViewer</h1>"
                          "<h2>TGAM module tools</h2>"
                          "<h3>Author: JackeyLea</h3>"
                          "<h3>E-mail: 1768478912@qq.com</h3>"
                          "<h3>GitHub: https://github.com/JackeyLea/MindViewer</h3>"
                          "<h3>Gitee: https://gitee.com/JackeyLea/MindViewer</h3>"
                          "<h4>Do what you want but please obey the LGPL3 rules</h4>"
                          "<h4>And keep those message within application</h4>");
    QMessageBox::information(this, tr("About"), msg, QMessageBox::Ok);
}

void MainWindow::sltActionAboutQt()
{
    qApp->aboutQt();
}

void MainWindow::sltActionGithub()
{
    QDesktopServices::openUrl(QUrl("https://github.com/JackeyLea/MindViewer"));
}

void MainWindow::initUi()
{
    ui->widgetAttention->setLabel("Attention");
    ui->widgetMeditation->setLabel("Meditation");
}

void MainWindow::initConnections()
{
    //模拟数据
    connect(ui->actionTest,&QAction::triggered,this,&MainWindow::sltActionTest);
    //串口连接
    connect(ui->actionCOM,&QAction::triggered,this,&MainWindow::sltActionCOM);
    //退出
    connect(ui->actionExit,&QAction::triggered,this,&MainWindow::sltActionExit);
    //16进制数据模式
    connect(ui->actionHex,&QAction::triggered,this,&MainWindow::sltActionHex);
    //图形模式
    connect(ui->actionGrap,&QAction::triggered,this,&MainWindow::sltActionGraph);
    //关于
    connect(ui->actionAbout,&QAction::triggered,this,&MainWindow::sltActionAbout);
    //关于qt
    connect(ui->actionAbout_Qt,&QAction::triggered,this,&MainWindow::sltActionAboutQt);
    //github
    connect(ui->actionGithub,&QAction::triggered,this,&MainWindow::sltActionGithub);
}

