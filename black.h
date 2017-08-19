#ifndef BLACK_H
#define BLACK_H

#include "chess.h"

class Black : public Chess
{
    Q_OBJECT
public:
    explicit Black(int size,QWidget *parent = 0);
    virtual void print();

signals:

public slots:
};

#endif // BLACK_H
