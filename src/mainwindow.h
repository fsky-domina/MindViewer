#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <QIcon>
#include <QFile>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct pkgPacket{
    int signal=0;

    int delta=0;
    int theta=0;
    int lowAlpha=0;
    int highAlpha=0;
    int lowBeta=0;
    int highBeta=0;
    int lowGamma=0;
    int midGamma=0;
    int attention=0;//0-100
    int meditation=0;//0-100
    int rawdata=0;
    int blink=0;

    bool valid=false;//此包数据是否有效
};//一个数据包包含的所有值

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    int size;//数据字符数

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString toStandardText(QString source);

    QList<pkgPacket> parserData(QList<int> data);
    QList<pkgPacket> parserData(QByteArray data);
private slots:
    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionPause_triggered();

    void on_actionResume_triggered();

    void on_actionCapture_triggered();

    void on_actionClear_triggered();

    void on_actionAbout_triggered();

    void on_actionQt_triggered();

    void on_actionGithub_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
