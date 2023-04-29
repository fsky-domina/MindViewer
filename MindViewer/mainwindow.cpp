#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->widgetAttention->setLabel("Attention");
    ui->widgetMeditation->setLabel("Meditation");

    ui->widgetAttention->setValue(50);
    ui->widgetMeditation->setValue(60);
}

MainWindow::~MainWindow()
{
    delete ui;
}

