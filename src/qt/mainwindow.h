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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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
