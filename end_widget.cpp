#include "end_widget.h"
#include "ui_end_widget.h"
#include<QDebug>
#include"log_widget.h"

End_Widget::End_Widget(QWidget *parent,Qt::WindowFlags f) :
    QWidget(parent,f),
    ui(new Ui::End_Widget)      //将endwidget窗口构造成ui界面设置成的样子
{
    ui->setupUi(this);
}

End_Widget::~End_Widget()   //析构
{
    delete ui;
}

void End_Widget::setText(QString &s)    //设置结束窗口上显示的文字：白棋胜利！、黑棋胜利！、平局！
{
    ui->textlabel->setText(s);
}

void End_Widget::on_pushButton_3_clicked()  //关闭结束窗口
{
    close();
}

void End_Widget::on_pushButton_4_clicked()
{
    close();
    emit close_mainwidget();
}

void End_Widget::on_pushButton_2_clicked()  //再来一局 关闭结束窗口 并且清空主界面
{
    close();
    emit clear_mainwidget();
}

void End_Widget::on_pushButton_clicked()    //进入登录界面 关闭结束窗口并且发出关闭主界面的信号
{
    Log_Widget *log=new Log_Widget;
    close();
    emit close_mainwidget();
    log->show();
}

void End_Widget::on_pushButton_5_clicked()  //进入文件选择窗口
{
    QTime *t=new QTime(QTime::currentTime());
    QString* s=new QString(QFileDialog::getSaveFileName(this,"保存对战","D:","二进制文件(*bin)"));
    emit send_save_game(*s,*t);
}
