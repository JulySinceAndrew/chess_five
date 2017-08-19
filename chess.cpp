#include "chess.h"

Chess::Chess(int size,QWidget *parent) : QLabel(parent)  //棋类设置大小，共同的构造部分
{
    QLabel::resize(size,size);
    QLabel::setScaledContents(true);
}
