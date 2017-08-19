#ifndef CHESS_H
#define CHESS_H

#include <QLabel>
#include<QPixmap>
#include<QDebug>
#include"stack.h"

class Chess : public QLabel
{
    Q_OBJECT
public:
    explicit Chess(int size,QWidget *parent = 0);
    virtual void print()=0;

signals:

public slots:
};

#endif // CHESS_H
