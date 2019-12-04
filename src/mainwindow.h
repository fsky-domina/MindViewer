#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIcon>
#include <QDesktopServices>
#include <QUrl>
#include <QTextCodec>
#include <QMessageBox>
#include <QSerialPort>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QTextStream>
#include <QFileDialog>
#include <QDebug>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initComWidget();

    void write2file(QString str);//将所有数据写入文件待用

private slots:
    void readMyCom();//读取串口数据

    void internalTime();//间隔时间

    void on_actionOpen_triggered();

    void on_actionClose_triggered();

    void on_actionClear_triggered();

    void on_actionPrint_triggered();

    void on_actionExit_triggered();

    void on_actionData_triggered();

    void on_actionGraph_triggered();

    void on_actionAbout_triggered();

    void on_actionQt_triggered();

    void on_actionIndex_triggered();

    void on_buttonOpen_clicked();

    void on_buttonClose_clicked();

private:
    Ui::MainWindow *ui;

    QStringList ComList,braudList;
    QSerialPort *myCom;
    QTimer *internalTimer;

    QString filePath;
};
#endif // MAINWINDOW_H
