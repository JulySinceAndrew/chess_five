#ifndef WIDGET_H
#define WIDGET_H

#define boundary_percentage 0.041667
#define interval_percentage 0.065476

#include <QWidget>
#include<QMouseEvent>
#include"chess.h"
#include"white.h"
#include"black.h"
#include<QPoint>
#include"stack.h"
#include"end_widget.h"
#include<QTime>
#include<QTimer>
#include<sstream>
#include"log_widget.h"
#include<fstream>
#include<QFileDialog>
#include<QDebug>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    void timer_start();
    ~Widget();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void update_total_time();

    void update_signal_time();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

public slots:
    void receive_close_mainwidget();
    void receive_clear_mainwidget();
    void receive_set_modal(bool _modal);
    void receive_save_game(QString& s,QTime&t);
    void receive_open_game(QString& s);

signals:
    void send_is_rightfile(bool isrightfile);

private:
    void mousePressEvent(QMouseEvent* event);
    bool judge_Result();
    void clear_mainwidget();
    void AI_decide_chess();
    QString second_to_MMSS(int n);
    QString second_to_HHMMSS(int n);
    void save_game(QString& s,QTime& now);
    void open_game(QString &s);
    bool judge_draw();
    Ui::Widget *ui;
    Stack<White*> whitechess;
    Stack<Black*> blackchess;
    Stack<QPoint> whitepoint;
    Stack<QPoint> blackpoint;
    End_Widget *end_widget;
    int chessboard[15][15];
    QTime *total_starttime;
    QTime *signal_starttime;
    QTime *now_time;
    QTimer *onesecond_timer;
    int count;
    bool break_flag;
    bool double_winner;
    bool modal;   //双人模式为true 单人模式为false
    bool player_first;

private:
    QPoint *AI_Point;
    QPoint *AI_Point_Attack;
    bool havefind;
    int detail_case(int r,int c,int nearx,int neary,bool Now);
    QPoint find();

private:
    class node
    {
     public: QPoint* p;
            int level;
            node* next;
            void add(int x,int y,int l,node* &_now)
            {
                node*temp=new node;
                temp->p=new QPoint(x,y);
                temp->level=l;
                temp->next=NULL;
                _now->next=temp;
                _now=temp;
            }
            ~node(){delete p;}
    };

};

#endif // WIDGET_H
