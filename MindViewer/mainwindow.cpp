#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widgetAttention->setLabel("Attention");
    ui->widgetMeditation->setLabel("Meditation");
}

MainWindow::~MainWindow()
{
    delete ui;
}

