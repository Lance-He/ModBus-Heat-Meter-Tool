#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "win_qextserialport.h"
#include <QMainWindow>
#include <QMessageBox>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Win_QextSerialPort *myCom;
    QByteArray m_Com_Monitor;

private slots:
    void readMyCom();
    void on_openMyComBtn_clicked();
    void on_closeMyComBtn_clicked();
    void on_sendMsgBtn_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_revise_pushButton_3_clicked();
};

#endif // MAINWINDOW_H
