#include "black.h"

Black::Black(int size,QWidget *parent) : Chess(size,parent)  //构造黑棋
{
    QLabel::setPixmap(QPixmap(":/image/imges/黑棋.png"));
    QLabel::setScaledContents(true);
}

void Black::print() //虚函数，用于输出黑棋位置，便于调试
{
     qDebug()<<"Black"<<pos()<<endl;
}
