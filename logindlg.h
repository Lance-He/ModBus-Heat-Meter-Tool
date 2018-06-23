#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QtWidgets/QMainWindow>
#include <QDialog>

namespace Ui {
class LoginDlg;
}

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDlg(QWidget *parent = 0);
    ~LoginDlg();

protected:
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    bool enterBtn(QPoint pp, QPushButton *btn);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::LoginDlg *ui;
};

#endif // LOGINDLG_H
