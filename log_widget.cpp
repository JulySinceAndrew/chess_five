#include "log_widget.h"
#include "ui_log_widget.h"
#include "widget.h"
#include<QPushButton>
#include"setdialog.h"

Log_Widget::Log_Widget(QWidget *parent,Qt::WindowFlags f) :
    QWidget(parent,f),
    ui(new Ui::Log_Widget)  //按照在ui设计界面中的样式构造logwidget
{
    ui->setupUi(this);
}

Log_Widget::~Log_Widget()   //析构
{
    delete ui;
}

void Log_Widget::receive_is_rightfile(bool isrightfile) //接受代表打开的文件是否是正确的文件的信号，如果是就关闭登录界面，不是就弹出一个错误窗口
{
    if(isrightfile)
        close();
    else
    {
        QMessageBox::critical(this,"载入失败","您选择的文件不是历史对战文件，请选择正确的文件",QMessageBox::Ok);
    }
}

void Log_Widget::on_pushButton_clicked()    //发送设置modal为false的信号 进入单机模式
{
    close();
    Widget *w=new Widget;
    connect(this,SIGNAL(set_modal(bool)),w,SLOT(receive_set_modal(bool)));
    emit set_modal(false);
}

void Log_Widget::on_pushButton_2_clicked()  //发送设置modal为true的信号 进入双人模式
{
    close();
    Widget *w=new Widget;
    connect(this,SIGNAL(set_modal(bool)),w,SLOT(receive_set_modal(bool)));
    emit set_modal(true);
}

void Log_Widget::on_pushButton_4_clicked()  //关闭登录界面
{
    close();
}

void Log_Widget::on_pushButton_3_clicked()  //进入帮助窗口
{
    this->setWindowFlags(Qt::SplashScreen);
    QDialog *w=new QDialog;
    QLabel *l=new QLabel(w);
    QPushButton *p=new QPushButton(w);
    w->setWindowFlags(Qt::SplashScreen);
    l->setText("计算机程序设计基础(2)__夏季学期__大作业2\n选题:连珠五子棋\n齐涛__无64\n感谢您的使用！");
    l->resize(400,300);
    w->resize(400,300);
    l->move(QPoint(75,0));
    w->setWindowTitle("关于我们");
    p->setText("退出");
    p->move(300,250);
    connect(p,SIGNAL(clicked()),w,SLOT(close()));
    w->exec();
    this->setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint);
    this->show();
}

void Log_Widget::on_pushButton_5_clicked()  //进入设置的窗口
{
    SetDialog *sd=new SetDialog();
    this->setWindowFlags(Qt::SplashScreen);
    sd->exec();
    this->setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint);
    show();
}

void Log_Widget::on_pushButton_6_clicked()  //进入打开文件的窗口，并且把选择的文件名发送给新建的主界面窗口
{
    QString *s=new QString(QFileDialog::getOpenFileName(this,"载入对战","D:","二进制文件(*bin)"));
    if(*s==QString())
        return ;
    Widget* w=new Widget;
    connect(this,SIGNAL(send_open_file(QString&)),w,SLOT(receive_open_game(QString&)));
    connect(w,SIGNAL(send_is_rightfile(bool)),this,SLOT(receive_is_rightfile(bool)));
    emit send_open_file(*s);
}
