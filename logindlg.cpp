#include "logindlg.h"
#include "ui_logindlg.h"
#include  <QDebug>
#include <QMouseEvent>

LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    setFixedSize(405,155);                     // 禁止拖动窗口大小
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMaximizeButtonHint); //禁用关闭按钮 禁用？按钮
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化按钮
    setMouseTracking(true);
    ui->pushButton->setMouseTracking(true);

    ui->pushButton_2->setMouseTracking(true);

}

LoginDlg::~LoginDlg()
{
    delete ui;
}
//鼠标移动事件
void LoginDlg::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();

        if(enterBtn(event->pos(),ui->pushButton))
        {
           ui->pushButton->setText("同意");
           ui->pushButton_2->setText("拒绝");
        }

        if(enterBtn(event->pos(),ui->pushButton_2))
        {
            ui->pushButton_2->setText("同意");
            ui->pushButton->setText("拒绝");
        }
}


bool LoginDlg::enterBtn(QPoint pp, QPushButton *btn)
{
   int height = btn->height();
   int width = btn->width();
   QPoint btnMinPos = btn->pos();
   QPoint btnMaxPos = btn->pos();
   btnMaxPos.setX(btn->pos().x()+width);
   btnMaxPos.setY(btn->pos().y()+height);
   if(pp.x() >= btnMinPos.x() && pp.y() >= btnMinPos.y()
       && pp.x() <= btnMaxPos.x() && pp.y() <= btnMaxPos.y())
       return true;
   else
       return false;
}

void LoginDlg::on_pushButton_clicked()
{

}

void LoginDlg::on_pushButton_2_clicked()
{

}
