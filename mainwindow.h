#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_checkBox_toggled(bool checked);

    void on_actionEnable_Pan_toggled(bool arg1);

    void on_actionSave_SVG_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
