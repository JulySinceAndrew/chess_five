#include "white.h"

White::White(int size,QWidget *parent) : Chess(size,parent) //设置白棋的图片
{
    QLabel::setPixmap(QPixmap(":/image/imges/白棋(2).png"));
}

void White::print()  //输出白棋位置，便于调试
{
    qDebug()<<"White"<<pos()<<endl;
}
