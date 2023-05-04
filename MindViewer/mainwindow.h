#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sltActionTest();
    void sltActionCOM();
    void sltActionExit();
    void sltActionHex(bool checked);
    void sltActionGraph(bool checked);
    void sltActionAbout();
    void sltActionAboutQt();
    void sltActionGithub();

private:
    void initUi();//初始化界面
    void initConnections();//初始化信号槽

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
