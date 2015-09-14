#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->checkBox->toggle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    ui->graphicsView->critical(checked);
}
