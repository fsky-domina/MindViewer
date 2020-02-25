#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("Open"),QString());
    if(filePath.isEmpty()){
        QMessageBox::information(this,tr("Warning"),tr("Nothing selected"));
        return;
    }
    QFile file(filePath);
    if(!file.exists()){
        QMessageBox::information(this,tr("Warning"),tr("The file didn't exists"));
        return;
    }
    QString result;
    if(file.open(QIODevice::Text|QIODevice::ReadOnly)){
        QTextStream readIn(&file);
        result = readIn.readAll();
    }
    ui->textResult->setPlainText(result);

}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit(0);
}

void MainWindow::on_actionPause_triggered()
{}

void MainWindow::on_actionResume_triggered()
{}

void MainWindow::on_actionCapture_triggered()
{}

void MainWindow::on_actionClear_triggered()
{}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this,tr("About"),tr(""),QMessageBox::Ok);
}

void MainWindow::on_actionQt_triggered()
{
    qApp->aboutQt();
}

void MainWindow::on_actionGithub_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/JackeyLea/MindViewer"));
}
