#ifndef WHITE_H
#define WHITE_H

#include "chess.h"

class White : public Chess
{
    Q_OBJECT
public:
    explicit White(int size,QWidget *parent = 0);
    virtual void print();
signals:

public slots:
};

#endif // WHITE_H
