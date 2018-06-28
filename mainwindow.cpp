#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "crcmodbus.h"
#include <stdio.h>
#include  <QDebug>
#include <stdlib.h>
#include <QDateTime>
#include <string>
#include <windows.h>
#include <QButtonGroup>
char jx_data[1024]; //串口数据接收到的数据解析后存放位置
unsigned short Get_Crc(char *p, int len);
void jx_shuju(char *data,int pos,int length,double places_data);
double return_data_biao(char *data,int pos,int length,double places_data); //带入存有截取的字符串 需要截取长度的首地址 截取的长度 需要转换的单位
char* substring(char* ch,int pos,int length);
void StringToHex(QString str, QByteArray &senddata); //字符串转换为十六进制数据0-F
char ConvertHexChar(char ch);
int return0num(int hex);
bool radioButton=true; //定义Modbus读取数据起始地址全局变量
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);    // 禁止最大化按钮

    ui->setupUi(this);
    ui->closeMyComBtn->setEnabled(false); //开始“关闭串口”按钮不可用

    ui->sendMsgBtn->setEnabled(false); //开始“发送数据”按钮不可用

    ui->revise_pushButton_3->setEnabled(false); // 修改表地址按钮不可以
    ui->oldaddr_lineEdit->setEnabled(false); // 原地址不可输入
    ui->newaddr_lineEdit_2->setEnabled(false); // 新地址不可输入
}
MainWindow::~MainWindow()
{
    delete ui;
}

//1111
void MainWindow::readMyCom() //读串口函数
{
    QByteArray temp = myCom->readAll();//读取串口缓冲区的所有数据给临时变量temp


    if(temp!="") //检测返回数据是否为空
    {
        for(int i=0;i<75000;i++) //循环读数据
        {
            temp+=myCom->readAll();
        }

        qDebug()<< "Rec:" + temp.toHex(); //控制台显示串口接收到的十六进制数据
        char buf[1024];

        memset( buf , 0 , sizeof(buf) );
        memcpy( buf , temp.toHex().data() , strlen(temp.toHex().data()));
        //qDebug()<<"数据中校验和为："<<hex<<return_data_biao(buf,54,4);
       //qDebug()<<"计算校验和为："<<hex<<CRC16(temp.data(), 27);
        ui->textBrowser->append("Rec:"+temp.toHex().toUpper());       //数据接收区域显示数据  append 输入数据添加新行  insertPlainText() 输入什么显示什么
        ui->textBrowser->append(""); //数据发送后换行

        ui->textBrowser->moveCursor(QTextCursor::End); //禁止移动光标
        if((int)return_data_biao(buf,54,4,1)==CRC16(temp.data(), 27)) //串口发送过来的校验和是否与数据校验和吻合
        {
           // qDebug()<<"CRC校验和:"<< hex << CRC16(temp.data(), temp.length()-2);//减去两位 为校验和前数据的 .length FF为 1位

            jx_shuju(buf,6,8,0.0001); //瞬时流量
            ui->textBrowser_2->insertPlainText("瞬时流量:"+QString(QLatin1String(jx_data))+"m³/h");
            ui->textBrowser_2->append(""); //数据发送后换行

            jx_shuju(buf,14,8,1); //累计流量
            ui->textBrowser_2->insertPlainText("累计流量:"+QString(QLatin1String(jx_data))+"m³");
            ui->textBrowser_2->append(""); //数据发送后换行

            jx_shuju(buf,22,8,0.01); //瞬时热量
            ui->textBrowser_2->insertPlainText("瞬时热量:"+QString(QLatin1String(jx_data))+"KW");
            ui->textBrowser_2->append(""); //数据发送后换行

            jx_shuju(buf,30,8,0.01); //累计热量
            ui->textBrowser_2->insertPlainText("累计热量:"+QString(QLatin1String(jx_data))+"MWh");
            ui->textBrowser_2->append(""); //数据发送后换行

            jx_shuju(buf,38,8,0.01); //累计冷量
            ui->textBrowser_2->insertPlainText("累计冷量:"+QString(QLatin1String(jx_data))+"MWh");
            ui->textBrowser_2->append(""); //数据发送后换行

            jx_shuju(buf,46,4,0.01); //进水温度
            ui->textBrowser_2->insertPlainText("进水温度:"+QString(QLatin1String(jx_data))+"℃");
            ui->textBrowser_2->append(""); //数据发送后换行

            jx_shuju(buf,50,4,0.01); //回水温度
            ui->textBrowser_2->insertPlainText("回水温度:"+QString(QLatin1String(jx_data))+"℃");
            ui->textBrowser_2->append(""); //数据发送后换行

           ui->textBrowser_2->moveCursor(QTextCursor::End);//禁止移动光标
          }
        else  if((int)return_data_biao(buf,12,4,1)==CRC16(temp.data(), 6)) //串口发送过来的校验和是否与数据校验和吻合
        {
                ui->textBrowser_2->insertPlainText("地址成功修改为:"+ui->newaddr_lineEdit_2->text()+"\r\nOK!");
                ui->textBrowser_2->append(""); //数据发送后换行

        }
        else
            ui->textBrowser_2->insertPlainText("数据异常,无法解析！");
        ui->textBrowser_2->append(""); //数据发送后换行
    }
}


void MainWindow::on_openMyComBtn_clicked()
{
    QButtonGroup *bg=new QButtonGroup(this);

    bg->addButton(ui->radioButton,0);//一个值为0
    bg->addButton(ui->radioButton_2,1);//一个值为1

    int sel=bg->checkedId();//取到你所选的radioButton的值

    QString portName = "\\\\.\\"+ui->portNameComboBox->currentText(); //获取串口名
    myCom = new Win_QextSerialPort(portName,QextSerialBase::EventDriven);
    //定义串口对象，并传递参数，在构造函数里对其进行初始化
     bool isOpenSucc;
     isOpenSucc=myCom ->open(QIODevice::ReadWrite); //打开串口
    if(isOpenSucc==true)
    {
        switch(sel)
        {
            case 0:
              qDebug()<<"0";
              radioButton = false;
              break;
            case 1:
               qDebug()<<"1";
               radioButton = true;
              break;
            default:
              qDebug()<<"1";
              radioButton = true;
            break;
        }
        if(ui->baudRateComboBox->currentText()==tr("9600")) //根据组合框内容对串口进行设置
        myCom->setBaudRate(BAUD9600);
        else if(ui->baudRateComboBox->currentText()==tr("115200"))
        myCom->setBaudRate(BAUD115200);
        else if(ui->baudRateComboBox->currentText()==tr("1200"))
        myCom->setBaudRate(BAUD1200);
        else if(ui->baudRateComboBox->currentText()==tr("2400"))
        myCom->setBaudRate(BAUD2400);
        else if(ui->baudRateComboBox->currentText()==tr("4800"))
        myCom->setBaudRate(BAUD4800);
        else if(ui->baudRateComboBox->currentText()==tr("300"))
        myCom->setBaudRate(BAUD300);
        else if(ui->baudRateComboBox->currentText()==tr("600"))
        myCom->setBaudRate(BAUD600);
        //设置波特率

        if(ui->dataBitsComboBox->currentText()==tr("8"))
        myCom->setDataBits(DATA_8);
        else if(ui->dataBitsComboBox->currentText()==tr("7"))
        myCom->setDataBits(DATA_7);
        else if(ui->dataBitsComboBox->currentText()==tr("6"))
        myCom->setDataBits(DATA_6);
        else if(ui->dataBitsComboBox->currentText()==tr("5"))
        myCom->setDataBits(DATA_5);
        //设置数据位

        if(ui->parityComboBox->currentText()==tr("无校验"))
        myCom->setParity(PAR_NONE);
        else if(ui->parityComboBox->currentText()==tr("奇校验"))
        myCom->setParity(PAR_ODD);
        else if(ui->parityComboBox->currentText()==tr("偶校验"))
        myCom->setParity(PAR_EVEN);
        //设置奇偶校验

        if(ui->stopBitsComboBox->currentText()==tr("1"))
        myCom->setStopBits(STOP_1);
        else if(ui->stopBitsComboBox->currentText()==tr("2"))
        myCom->setStopBits(STOP_2);
        //设置停止位

        myCom->setFlowControl(FLOW_OFF); //设置数据流控制，我们使用无数据流控制的默认设置
        myCom->setTimeout(500); //设置延时
        connect(myCom,SIGNAL(readyRead()),this,SLOT(readMyCom()));
        //信号和槽函数关联，当串口缓冲区有数据时，进行读串口操作

       // QMessageBox::information(this, tr("串口配置"), tr("打开串口成功！"));


        ui->openMyComBtn->setEnabled(false); //打开串口后“打开串口”按钮不可用
        ui->closeMyComBtn->setEnabled(true); //打开串口后“关闭串口”按钮可用
        ui->sendMsgBtn->setEnabled(true); //打开串口后“发送数据”按钮可用

        ui->baudRateComboBox->setEnabled(false); //设置各个组合框不可用
        ui->dataBitsComboBox->setEnabled(false);
        ui->parityComboBox->setEnabled(false);
        ui->stopBitsComboBox->setEnabled(false);
        ui->portNameComboBox->setEnabled(false);
        ui->radioButton->setEnabled(false);
        ui->radioButton_2->setEnabled(false);

        ui->revise_pushButton_3->setEnabled(true); // 修改表地址按钮不可以
        ui->oldaddr_lineEdit->setEnabled(true); // 原地址不可输入
        ui->newaddr_lineEdit_2->setEnabled(true); // 新地址不可输入



    }
    else {
         QMessageBox::warning(this, tr("Warning"), tr("打开串口失败！"));
    }
}

void MainWindow::on_closeMyComBtn_clicked()
{
    myCom->close();
    ui->openMyComBtn->setEnabled(true); //关闭串口后“打开串口”按钮可用
    ui->closeMyComBtn->setEnabled(false); //关闭串口后“关闭串口”按钮不可用
    ui->sendMsgBtn->setEnabled(false); //关闭串口后“发送数据”按钮不可用
    ui->baudRateComboBox->setEnabled(true); //设置各个组合框可用
    ui->dataBitsComboBox->setEnabled(true);
    ui->parityComboBox->setEnabled(true);
    ui->stopBitsComboBox->setEnabled(true);
    ui->portNameComboBox->setEnabled(true);

    ui->revise_pushButton_3->setEnabled(false); // 修改表地址按钮不可以
    ui->oldaddr_lineEdit->setEnabled(false); // 原地址不可输入
    ui->newaddr_lineEdit_2->setEnabled(false); // 新地址不可输入
    ui->radioButton->setEnabled(true);
    ui->radioButton_2->setEnabled(true);
}

void MainWindow::on_sendMsgBtn_clicked()//第一个发送按钮
{
    QString input= ui->sendMsgLineEdit->text(); //检测输入行是否为空
        if(input.isEmpty()) //检测到密码输入为空
            QMessageBox::warning(this, tr("警告"),tr("输入不能为空！"),QMessageBox::Yes);
        else if(input.toInt() >246)
        {
            if(input.toInt() == 250)
                QMessageBox::warning(this, tr("ZZ"),tr("(￣_,￣ ) 二百五不是抄表地址！"),QMessageBox::Yes);
            else
                QMessageBox::warning(this, tr("警告"),tr("ModBus协议规定地址246以后为特殊地址，非读表内数据地址！"),QMessageBox::Yes);
        }
        else if (strcmp(input.toLatin1(), "Lance") == 0)
            {
                                                                                                                                    QMessageBox message(QMessageBox::NoIcon, "(╯‵□′)╯ ┻━┻", "━┳━　━┳━ 瑞纳集团中最可怜、最弱小、工资最低的员工！");
                message.exec();
             }
        else
            {
                QString str1 = ui->sendMsgLineEdit->text();//从sendMsgLineEdit得到字符串地址
                int add_biao= str1.toInt(); //存储接收到的表地址（十进制）
                char add_data[1024];
                if(add_biao<16)  //将十进制存储地址转换为十六进制并加上读取
                {
                    if(radioButton == true)
                        sprintf(add_data,"0%x030001000C",add_biao);
                    else
                        sprintf(add_data,"0%x030000000C",add_biao);
                }
                else
                {
                    if(radioButton == true)
                        sprintf(add_data,"%x030001000C",add_biao);
                    else
                        sprintf(add_data,"%x030000000C",add_biao);
                }
                 qDebug()<<add_data; //输出指令不带CRC校验和

                QString str = QString(QLatin1String(add_data));
                int len = str.length();
                if(len%2 == 1)   //如果发送的数据个数为奇数的，则在前面最后落单的字符前添加一个字符0
                {
                   str = str.insert(len-1,'0'); //insert(int position, const QString & str)
                }

                QByteArray senddata; //创建发送数据
                QByteArray senddata1;

                StringToHex(str,senddata);//将str字符串转换为16进制的形式
                char send_CRC_data[1024]; //存储十六进制转换成字符串
                int temp_crc=CRC16(senddata.data(), senddata.length());
                 qDebug()<<hex<<temp_crc;
                 qDebug()<<return0num(temp_crc);
                 switch(return0num(temp_crc))
                 {
                     case 0: sprintf(send_CRC_data,"%x",temp_crc);break;
                     case 1: sprintf(send_CRC_data,"0%x",temp_crc);break;
                     case 2: sprintf(send_CRC_data,"00%x",temp_crc);break;
                     case 3: sprintf(send_CRC_data,"000%x",temp_crc);break;

                 }
                qDebug()<<send_CRC_data;
                StringToHex(QString(QLatin1String(send_CRC_data)),senddata1);//将转换成字符串的CRC校验码转换为16进制的形式
                senddata.append(senddata1); //将转换的十六进制添加到数据字符串后面
                myCom->write(senddata);//发送到串口
                ui->textBrowser->append("Send:"+senddata.toHex().toUpper()); //发送的内容显示
                //qDebug()<<senddata;
             }
    //以ASCII码形式将行编辑框中的数据写入串口
}

void MainWindow::on_pushButton_clicked() //清除数据按钮
{
    ui->textBrowser->setPlainText("");
}

void MainWindow::on_pushButton_2_clicked() //清除报文按钮
{
     ui->textBrowser_2->setPlainText("");
}

void MainWindow::on_revise_pushButton_3_clicked()
{
    QString M_oldAddr= ui->oldaddr_lineEdit->text();  //源地址框
    QString M_newAddr= ui->newaddr_lineEdit_2->text();  //新地址框

        if(M_oldAddr.isEmpty()) //检测源地址是否为空
            QMessageBox::warning(this, tr("警告"),tr("原地址不能为空！"),QMessageBox::Yes);
        else if(M_newAddr.isEmpty())
            QMessageBox::warning(this, tr("警告"),tr("新地址不能为空！"),QMessageBox::Yes);
        else
        {
            if(M_oldAddr.toInt()>=247 || M_newAddr.toInt()>=247)
                 QMessageBox::warning(this, tr("警告"),tr("ModBus协议规定247地址及其后面地址为特殊地址！请谨慎修改！"),QMessageBox::Yes);

                M_oldAddr=M_oldAddr.remove(QRegExp("\\s")); //清楚源地址字符串内所有空格
                M_newAddr=M_newAddr.remove(QRegExp("\\s")); //清楚新地址字符串内所有空格
                //qDebug()<<M_newAddr.toInt();
                int len = M_oldAddr.length(); //源地址为1位 时在前面加0
                if(len == 1) //如果表地址为1位时则前一位需要补0
                   M_oldAddr = M_oldAddr.insert(len-1,'0'); //insert(int position, const QString & str)
              //   qDebug()<<"old addr is "+ M_oldAddr;

                int len1 = M_newAddr.length();//新地址为1位时，在前面加0
                 if(len1 == 1) //如果表地址为1位时则前一位需要补0
                  M_newAddr = M_newAddr.insert(len1-1,'0'); //insert(int position, const QString & str)
                //qDebug()<<"new addr is "+ M_newAddr;

                int M_oldAddr_Hex= M_oldAddr.toInt();
                int M_newAddr_Hex= M_newAddr.toInt();
                char char_MAddr_Hex[1024];
                if(M_newAddr_Hex > 15)
                {
                    if(M_oldAddr_Hex > 15)
                        sprintf(char_MAddr_Hex,"%x06060700%x",M_oldAddr_Hex,M_newAddr_Hex);
                    else
                        sprintf(char_MAddr_Hex,"0%x06060700%x",M_oldAddr_Hex,M_newAddr_Hex);
                }
                else
                {
                    if(M_oldAddr_Hex > 15)
                        sprintf(char_MAddr_Hex,"%x060607000%x",M_oldAddr_Hex,M_newAddr_Hex);
                    else
                        sprintf(char_MAddr_Hex,"0%x060607000%x",M_oldAddr_Hex,M_newAddr_Hex);
                }

                 QString S_MAddr_Hex = QString(QLatin1String(char_MAddr_Hex)); //将char型的数据变成Qstring型

                 QByteArray senddata;
                 QByteArray senddata1;
                 StringToHex(S_MAddr_Hex,senddata);//将str字符串转换为16进制的形式


                 char send_CRC_data[1024]; //存储十六进制转换成字符串

                 sprintf(send_CRC_data,"%x",CRC16(senddata.data(), senddata.length())); //把十六进制数据转换为字符串数据
                 StringToHex(QString(QLatin1String(send_CRC_data)),senddata1);//将转换成字符串的CRC校验码转换为16进制的形式

                 senddata.append(senddata1); //将转换的十六进制添加到数据字符串后面

                 myCom->write(senddata);//发送到串口
                  ui->textBrowser->append("Change Address Send:"+senddata.toHex().toUpper()); //发送的内容显示
        }
}

void StringToHex(QString str, QByteArray &senddata) //字符串转换为十六进制数据0-F
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;

    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}
char ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return ch-ch;//不在0-f范围内的会发送成0
}



//#define CRC 0xa001
unsigned short Get_Crc(char *p, int len) //CRC校验函数
{
    unsigned i = 0;
    unsigned int tmp = 0xffff;
    while (len > 0)
    {
        tmp ^= *p++;
        while (i < 8)
        {
            if (tmp & 0x0001)
            {
                tmp >>= 1;
                tmp ^= 0xa001;
            }
            else
            {
                tmp >>= 1;
            }
            i++;
        }
        i = 0;
        len--;
    }
    return tmp;
}

//
char* substring(char* ch,int pos,int length) //带入 数组   开始截取  停止截取  返回截取完的数据
{
    char* pch=ch;
//定义一个字符指针，指向传递进来的ch地址。
    char* subch=(char*)calloc(sizeof(char),length+1);
//通过calloc来分配一个length长度的字符数组，返回的是字符指针。
    int i;
//只有在C99下for循环中才可以声明变量，这里写在外面，提高兼容性。
    pch=pch+pos;
//是pch指针指向pos位置。
    for(i=0;i<length;i++)
    {
        subch[i]=*(pch++);
//循环遍历赋值数组。
    }
    subch[length]='\0';//加上字符串结束符。
    return subch;       //返回分配的字符数组地址。
}
//带入存有数据的数组 截取内容的首字节，截取内容的长度
double return_data_biao(char *data,int pos,int length,double places_data) //带入数据 FF 为两个字节
{
    int nValude; //存放转换后的数值

    sscanf(substring(data,pos,length), "%x", &nValude); //将substring截图到的字符串转为十六进制
   // qDebug()<<hex<<nValude;
    return (double)nValude *places_data;
}
//将全局变量jx_data写入转换后的数据
void jx_shuju(char *data,int pos,int length,double places_data)
{

    sprintf(jx_data,"%.2f",return_data_biao(data,pos,length,places_data));

}
//带入一个16位的十六进制数值
//返回从第一位开始连续几个0
int return0num(int hex)
{
    int num=0;

    if((hex & 0xf000)==0)
        num++;
    else
        return num;

    if((hex & 0x0f00)==0)
        num++;
    else
        return num;

    if((hex & 0x00f0)==0)
        num++;
    else return num;

    return num;
}

