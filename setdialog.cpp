#include "setdialog.h"
#include "ui_setdialog.h"

extern bool _player_first;  //引用mian.cpp中的全局变量 便于设置先手

SetDialog::SetDialog(QWidget *parent,Qt::WindowFlags f) :
    QDialog(parent,f),
    ui(new Ui::SetDialog)   //按照ui设计中的样式构造setdialog
{
    ui->setupUi(this);
    if(_player_first)   //根据上次的设置，决定两个选项谁处于被选中状态
        ui->radioButton_2->setChecked(true);
    else
        ui->radioButton->setChecked(true);
}

SetDialog::~SetDialog() //析构
{
    delete ui;
}

void SetDialog::on_pushButton_clicked() //再点击确定后根据选中情况设置_player_first的值，同时也将这个值传给了mian.cpp
{
    _player_first=ui->radioButton_2->isChecked();
    close();
}
