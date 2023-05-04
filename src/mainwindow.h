#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QString>
#include <QPen>
#include <QResizeEvent>

#include "generator.h"
#include "retriver.h"
#include "common.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int parserData(QByteArray ba, bool &raw, short &rawValue, bool &common,bool &eeg, struct _eegPkt &pkt);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void sltReceiveData(QByteArray ba);
private:
    Ui::MainWindow *ui;

    Generator *gen=nullptr;
    Retriver *retriverWgt;

    bool isCommonResized=false;
    bool isEEGResized = false;

    QByteArray mBuff;//串口缓冲区

    int filterCnt=0;//用于滤波
};
#endif // MAINWINDOW_H
