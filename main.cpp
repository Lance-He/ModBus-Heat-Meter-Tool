#include "mainwindow.h"
#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <logindlg.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.setWindowTitle("瑞纳热量表-ModBus Beta v1.54"); //修改标题
    LoginDlg dlg;
    dlg.setWindowTitle("欢迎使用"); //修改标题
 //   dlg.exec(); //弹出提示框
    while(dlg.exec() != QDialog::Accepted); // 利用Accepted返回值判断按钮是否被按下 必须按下才可以
    return a.exec();
}

// 添加时间戳 1.13
// 修改输入表号提示 1.14
// 打开串口前不可使用自定义数据发送按钮 1.15
// BUG 修复自定义发送数据位数过多包含空格时数据接收错误 1.20
// 增加打开串口失败提示 1.21
// 增加修改瑞纳MODBUS协议表地址功能 1.30
// 增加修改表地址成功提示 1.31
// 增加300 600 4800波特率 1.32
// 修改数据接收与报文解析框 textBrowser 修改为 textEdit 1.33
// 去除多余输入框 1.34
// 修改布局界面 1.35
// BUG 修复当热量表地址大于128时校验和错误（大于128CRC校验和第一位为0十六进制自动省略第一位0则发送数据错误）BUG 1.4
// BUG 修复表地址只能修改在1-99之间为标准的MODBUS协议1-247 （247为特殊地址）1.5
// 增加了打开软件提示界面 1.51
// 去除了登陆框的关闭和最小化按钮 1.52
// 项目托管到Github上 1.53
// 此处不再添加更新内容，所有更新内容将移到Github上 地址：https://github.com/Lance-He/ModBus-Heat-Meter-Tool
