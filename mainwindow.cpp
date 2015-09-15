#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

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

void MainWindow::on_actionEnable_Pan_toggled(bool arg1)
{
    ui->graphicsView->enablePan(arg1);
}

void MainWindow::on_actionSave_SVG_triggered()
{

    QString file = QFileDialog::getSaveFileName(this, "Save to file...", "./", "*.svg");
    if(!file.isEmpty())
        ui->graphicsView->saveSVG(file);
}
