#include "widget.h"
#include "ui_widget.h"

//宏定义在某一位置上棋子的评分
#define must_lose 10
#define more_will_win 9
#define more_will_lose 8
#define will_win 7
#define will_lose 6
#define willwill_win 5
#define willwill_lose 4
#define may_win 3
#define may_lose 2
#define hard_win 1
#define hard_lose 0
#define no_win -1
#define no_lose -2

//宏定义密码和密码位数，在保存的文件中做标记以便检验
#define password "ygnshelakg"
#define passwordcount 10

extern bool _player_first;  //引用mian.cpp的全局变量 达到和setdialog共享数据
using std::fstream;
using std::ios_base;
fstream* fout=NULL; //设置全局的文件指针，便于之后作为使用vistit方法遍历栈的函数指针的"参数"

void _save_file(QPoint& p)  //作为保存栈中元素的函数，fout作为其"参数",遍历栈中元素，对栈中每隔元素执行这个函数，就可以把栈中的元素都储存到对应的文件之中
{
    int n=p.x();
    fout->write((char*)(&n),4);
    n=p.y();
    fout->write((char*)(&n),4);
}

void delete_whitechess(White* &t)   //删去白棋
{
    t->close();
}

void delete_blackchess(Black* &t)   //删去黑棋
{
    t->close();
}

Widget::Widget(QWidget *parent) :   //按照UI设计模式中的掩饰构造主界面
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->lcdNumber->display("00:00");    //初始化各个属性的值
    modal=true;
    count=0;
    player_first=_player_first;
    break_flag=false;
    double_winner=false;
    setFixedSize(759,481);
    ui->label->resize(this->height(),this->height());
    for(int i=0;i<15;i++)
        for(int j=0;j<15;j++)
            chessboard[i][j]=2;
    total_starttime=new QTime(QTime::currentTime());
    signal_starttime=new QTime(QTime::currentTime());
    onesecond_timer=new QTimer(this);
    onesecond_timer->setInterval(1000);
    connect(onesecond_timer,SIGNAL(timeout()),this,SLOT(update_total_time()));
    connect(onesecond_timer,SIGNAL(timeout()),this,SLOT(update_signal_time()));
}

void Widget::timer_start()  //重新开始一次计时 将两个事件和一个计时器删去并重新构建
{
    delete total_starttime;
    delete signal_starttime;
    delete onesecond_timer;
    total_starttime=new QTime(QTime::currentTime());
    signal_starttime=new QTime(QTime::currentTime());
    onesecond_timer=new QTimer(this);
    onesecond_timer->setInterval(1000);
    onesecond_timer->start();
    connect(onesecond_timer,SIGNAL(timeout()),this,SLOT(update_total_time()));
    connect(onesecond_timer,SIGNAL(timeout()),this,SLOT(update_signal_time()));
}

Widget::~Widget()   //析构函数
{
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *event)    //鼠标单击事件
{
    if(break_flag)  //游戏结束就返回
        return ;
    if(event->x()>height()||event->y()>height())    //单击在棋盘外就返回
        return ;
    bool draw_flag=false;   //设置平局标志false
    int n_y=(event->x()-height()*boundary_percentage)/(height()*interval_percentage)+0.5;
    int n_x=(event->y()-height()*boundary_percentage)/(height()*interval_percentage)+0.5;
    if(chessboard[n_x][n_y]!=2) //检验像素点对应的棋盘点是否为空 不为空就返回
        return ;
    ui->lcdNumber->display("00:00");    //重置落子计时
    if((count++)%2==0&&modal)   //双人模式下的白棋落子
    {   //设置下一个落子提示为黑棋 并且将白棋的落子构造完成
            ui->text_nowchess_label->setText("黑\n棋");
            ui->png_nowchess_label->setPixmap(QPixmap(":/image/imges/黑棋.png"));
            White *w=new White(int(height()*boundary_percentage*1.1),this);
            QPoint p(height()*boundary_percentage+n_y*interval_percentage*height()-11,height()*boundary_percentage+n_x*interval_percentage*height()-11);
            w->move(p);
            w->show();
            chessboard[n_x][n_y]=0;
            QPoint q(n_x,n_y);
            whitepoint.push(q);
            whitechess.push(w);
            break_flag=judge_Result();//检验是否结束
            if(!break_flag)//没结束检验是否平局
                draw_flag=judge_draw();
    }
    else if(modal)//双人模式的黑棋落子
    {//设置下一个落子提示为白棋 并且将黑棋的落子构造完成
        ui->text_nowchess_label->setText("白\n棋");
        ui->png_nowchess_label->setPixmap(QPixmap(":/image/imges/白棋(2).png"));
        Black *e=new Black(int(height()*boundary_percentage*1.1),this);
        QPoint p(height()*boundary_percentage+n_y*interval_percentage*height()-11,height()*boundary_percentage+n_x*interval_percentage*height()-11);
        e->move(p);
        e->show();
        chessboard[n_x][n_y]=1;
        QPoint q(n_x,n_y);
        blackpoint.push(q);
        blackchess.push(e);
        break_flag=judge_Result();//检验是否结束
        if(!break_flag)//没结束检验是否平局
            draw_flag=judge_draw();
    }
    else if(player_first)//单人模式的玩家先手执白
    {//构造完玩家的白棋落子
        White *w=new White(int(height()*boundary_percentage*1.1),this);
        QPoint p(height()*boundary_percentage+n_y*interval_percentage*height()-11,height()*boundary_percentage+n_x*interval_percentage*height()-11);
        w->move(p);
        w->show();
        chessboard[n_x][n_y]=0;
        QPoint q(n_x,n_y);
        whitepoint.push(q);
        whitechess.push(w);
        break_flag=judge_Result();//判断玩家落子后游戏是否结束
        if(!break_flag)//游戏没结束判断是否平局
            draw_flag=judge_draw();
        if((!break_flag)&&(!draw_flag))//在玩家落子既不胜利也不平局 进行AI落子
        {
            AI_decide_chess();//AI落子
            break_flag=judge_Result();//判断AI落子后是否胜利
            if(!break_flag)//AI落子后没有胜利 判断AI落子后是否平局
                draw_flag=judge_draw();
        }
    }
    else//单机模式下玩家后手执黑
    {
        Black *e=new Black(int(height()*boundary_percentage*1.1),this);
        QPoint p(height()*boundary_percentage+n_y*interval_percentage*height()-11,height()*boundary_percentage+n_x*interval_percentage*height()-11);
        e->move(p);
        e->show();
        chessboard[n_x][n_y]=1;
        QPoint q(n_x,n_y);
        blackpoint.push(q);
        blackchess.push(e);
        break_flag=judge_Result();
        if(!break_flag)
            draw_flag=judge_draw();
        if((!break_flag)&&(!draw_flag))
        {
            AI_decide_chess();
            break_flag=judge_Result();
            if(!break_flag)
                draw_flag=judge_draw();
        }
    }
    delete signal_starttime;//删去上次落子的开始计时并构建新的
    signal_starttime=new QTime(QTime::currentTime());
    if(break_flag)//如果有人胜利
    {
        End_Widget *end_widget=new End_Widget();//构建结束界面并进行初始化
        connect(end_widget,SIGNAL(close_mainwidget()),this,SLOT(receive_close_mainwidget()));
        connect(end_widget,SIGNAL(clear_mainwidget()),this,SLOT(receive_clear_mainwidget()));
        connect(end_widget,SIGNAL(send_save_game(QString&,QTime&)),this,SLOT(receive_save_game(QString&,QTime&)));
        if(double_winner)//double记录了胜利者 true为黑 false为白
        {
            QString s="黑棋胜利！";
            end_widget->setText(s);
        }
        end_widget->show();
    }
    if(draw_flag)   //如果平局
    {
        break_flag=true;    //设置游戏结束 并初始化结束界面 设置文字为平局
        End_Widget *end_widget=new End_Widget();
        connect(end_widget,SIGNAL(close_mainwidget()),this,SLOT(receive_close_mainwidget()));
        connect(end_widget,SIGNAL(clear_mainwidget()),this,SLOT(receive_clear_mainwidget()));
        connect(end_widget,SIGNAL(send_save_game(QString&,QTime&)),this,SLOT(receive_save_game(QString&,QTime&)));
        QString *temps=new QString("    平局！");
        end_widget->setText(*temps);
        end_widget->show();
    }
}

bool Widget::judge_Result() //判断是否有人胜利
{
    bool flag=false;
    int r=0,c=0,now=0;
    int count=0,judgecolour;
    for(r=0;r<15;r++)   //横判断
    {
        for(c=0;c<15;)
        {
            for(;c<15;c++)
                if(chessboard[r][c]!=2)
                    break;
            judgecolour=chessboard[r][c];
            count=1;
            for(c=c+1;c<15;c++)
            {
                if(chessboard[r][c]==judgecolour)
                {
                    count++;
                }
                else
                    break;
            }
            if(count>=5)    //5个及5个以上子连珠
            {
                flag=true;  //设置有人胜利的标记为true
                double_winner=judgecolour;  //设置胜利者是判断的这个颜色
                return flag;
            }
        }
    }
    for(c=0;c<15;c++)   //判断列
    {
        for(r=0;r<15;)
        {
            for(;r<15;r++)
                if(chessboard[r][c]!=2)
                    break;
            judgecolour=chessboard[r][c];
            count=1;
            for(r=r+1;r<15;r++)
            {
                if(chessboard[r][c]==judgecolour)
                {
                    count++;
                }
                else
                    break;
            }
            if(count>=5)
            {
                flag=true;
                double_winner=judgecolour;
                return flag;
            }
        }
    }
    for(r=0;r<11;r++)   //判断斜下 左下棋盘
    {
        now=r;
        for(c=0;c<15&&now<15;)
        {
            for(;c<15&&now<15;now++,c++)
                if(chessboard[now][c]!=2)
                    break;
            judgecolour=chessboard[now][c];
            count=1;
            for(now=now+1,c=c+1;now<15&&c<15;now++,c++)
            {
                if(chessboard[now][c]==judgecolour)
                {
                    count++;
                }
                else
                    break;
            }
            if(count>=5)
            {
                flag=true;
                double_winner=judgecolour;
                return flag;
            }
        }
    }
    for(c=0;c<11;c++)   //判断斜下 右下棋盘
    {
        now=c;
        for(r=0;r<15&&now<15;)
        {
            for(;r<15&&now<15;now++,r++)
                if(chessboard[r][now]!=2)
                    break;
            judgecolour=chessboard[r][now];
            count=1;
            for(now=now+1,r=r+1;now<15&&r<15;now++,r++)
            {
                if(chessboard[r][now]==judgecolour)
                {
                    count++;
                }
                else
                    break;
            }
            if(count>=5)
            {
                flag=true;
                double_winner=judgecolour;
                return flag;
            }
        }
    }
    for(r=5;r<15;r++)   //判断斜上 左上棋盘
    {
        now=r;
        for(c=0;c<15&&now>0;)
        {
            for(;c<15&&now<15;now--,c++)
                if(chessboard[now][c]!=2)
                    break;
            judgecolour=chessboard[now][c];
            count=1;
            for(now=now-1,c=c+1;now>0&&c<15;now--,c++)
            {
                if(chessboard[now][c]==judgecolour)
                {
                    count++;
                }
                else
                    break;
            }
            if(count>=5)
            {
                flag=true;
                double_winner=judgecolour;
                return flag;
            }
        }
    }
    for(c=0;c<11;c++)   //判断斜上 右下棋盘
    {
        now=c;
        for(r=15;r>0&&now<15;)
        {
            for(;r>0&&now<15;now++,r--)
                if(chessboard[r][now]!=2)
                    break;
            judgecolour=chessboard[r][now];
            count=1;
            for(now=now+1,r=r-1;now<15&&r<15;now++,r--)
            {
                if(chessboard[r][now]==judgecolour)
                {
                    count++;
                }
                else
                    break;
            }
            if(count>=5)
            {
                flag=true;
                double_winner=judgecolour;
                return flag;
            }
        }
    }
    return false;
}

void Widget::clear_mainwidget() //清空棋盘
{   //将属性清空至默认
    count=0;
    break_flag=double_winner=false;
    for(int i=0;i<15;i++)
        for(int j=0;j<15;j++)
        {
            chessboard[i][j]=2;
        }
    while(whitepoint.count())
        whitepoint.pop();
    while(blackpoint.count())
        blackpoint.pop();
    whitechess.visit(delete_whitechess);//不能直接让whitechess中元素出栈，否则出栈的仅是指针，导致内存泄漏
    blackchess.visit(delete_blackchess);
    while(whitechess.count())
        whitechess.pop();
    while(blackchess.count())
        blackchess.pop();
    delete total_starttime;
    delete signal_starttime;
    delete onesecond_timer;
    total_starttime=new QTime(QTime::currentTime());
    signal_starttime=new QTime(QTime::currentTime());
    onesecond_timer=new QTimer(this);
    onesecond_timer->setInterval(1000);
    onesecond_timer->start();
    ui->lcdNumber->display("00:00");
    ui->totaltime_label->setText("00:00:00");
    connect(onesecond_timer,SIGNAL(timeout()),this,SLOT(update_total_time()));
    connect(onesecond_timer,SIGNAL(timeout()),this,SLOT(update_signal_time()));
    if(modal||player_first)
    {
        ui->text_nowchess_label->setText("白\n棋");
        ui->png_nowchess_label->setPixmap(QPixmap(":/image/imges/白棋(2).png"));
    }
    else
    {
        ui->text_nowchess_label->setText("黑\n棋");
        ui->png_nowchess_label->setPixmap(QPixmap(":/image/imges/黑棋.png"));
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        int n_x=qrand()%7+4;
        int n_y=qrand()%7+4;
        White *e=new White(int(height()*boundary_percentage*1.1),this);
        QPoint p(height()*boundary_percentage+n_y*interval_percentage*height()-11,height()*boundary_percentage+n_x*interval_percentage*height()-11);
        e->move(p);
        e->show();
        chessboard[n_x][n_y]=1;
        QPoint q(n_x,n_y);
        whitepoint.push(q);
        whitechess.push(e);

    }
}

void Widget::AI_decide_chess()  //AI落子的函数
{
    QPoint q=find();    //由find函数返回最佳落子点
    if(_player_first)   //根据谁是先手，决定应该落子为什么颜色
    {
         Black *e=new Black(int(height()*boundary_percentage*1.1),this);
         e->move(height()*boundary_percentage+q.y()*interval_percentage*height()-11,height()*boundary_percentage+q.x()*interval_percentage*height()-11);
         e->show();
         chessboard[q.x()][q.y()]=1;
         blackpoint.push(q);
         blackchess.push(e);
    }
    else
    {
         White *e=new White(int(height()*boundary_percentage*1.1),this);
         e->move(height()*boundary_percentage+q.y()*interval_percentage*height()-11,height()*boundary_percentage+q.x()*interval_percentage*height()-11);
         e->show();
         chessboard[q.x()][q.y()]=0;
         whitepoint.push(q);
         whitechess.push(e);
    }
}

QString Widget::second_to_MMSS(int n)   //将整数转化为一个mm：ss格式的时间字符串
{
    int second=n%60;
    n/=60;
    int minute=n;
    QString save;
    std::ostringstream ostring;
    if(minute<10)
        ostring<<0;
    ostring<<minute;
    if(second%2==0)
        ostring<<" ";
    else
        ostring<<":";
    if(second<10)
        ostring<<0;
    ostring<<second;
    save=QString::fromStdString(ostring.str());
    return save;
}

QString Widget::second_to_HHMMSS(int n)  //将整数转化为一个hh：mm：ss格式的时间字符串
{
    int second=n%60;
    n/=60;
    int minute=n%60;
    n/=60;
    int hour=n;
    QString save;
    std::ostringstream ostring;
    if(hour<10)
        ostring<<0;
    ostring<<hour;
    if(second%2==0)
        ostring<<" ";
    else
        ostring<<":";
    if(minute<10)
        ostring<<0;
    ostring<<minute;
    if(second%2==0)
        ostring<<" ";
    else
        ostring<<":";
    if(second<10)
        ostring<<0;
    ostring<<second;
    save=QString::fromStdString(ostring.str());
    return save;
}

void Widget::save_game(QString &s,QTime& now)   //保存游戏
{
    fout=new fstream;
    fout->open(s.toStdString(),ios_base::out|ios_base::trunc);
    char ss[passwordcount+1]=password;
    fout->write((char*)(&ss),passwordcount);    //加入密码
    fout->write((char*)(&count),4); //储存各个属性
    fout->write((char*)(&break_flag),1);
    fout->write((char*)(&double_winner),1);
    fout->write((char*)(&modal),1);
    fout->write((char*)(&player_first),1);
    int n;
    n=total_starttime->secsTo(now);
    fout->write((char*)(&n),4);
    n=signal_starttime->secsTo(now);
    fout->write((char*)(&n),4);
    fout->write((char*)chessboard,225*4);
    n=whitepoint.count();
    fout->write((char*)(&n),4);
    whitepoint.visit(_save_file);
    n=blackpoint.count();
    fout->write((char*)(&n),4);
    blackpoint.visit(_save_file);
    fout->close();
    delete fout;
}

void Widget::open_game(QString &s)  //读取游戏
{
    fstream* fin=new fstream;
    fin->open(s.toStdString(),ios_base::in);
    char stemp[passwordcount+1];
    char pass[passwordcount+1]=password;
    fin->read(stemp,passwordcount);
    bool flag=true;
    for(int i=0;i<passwordcount;i++)
        if(stemp[i]!=pass[i])
        {
            flag=false;
            break;
        }
    if(!flag)   //检测密码是否正确，发送不是正确文件的信号并返回
    {
        emit send_is_rightfile(false);
        return ;
    }
    emit send_is_rightfile(true);  //检测密码正确，发送正确文件的信号
    int temp;   //按照写入格式读取属性
    fin->read((char*)(&count),4);
    fin->read((char*)(&break_flag),1);
    fin->read((char*)(&double_winner),1);
    fin->read((char*)(&modal),1);
    fin->read((char*)(&player_first),1);
    fin->read((char*)(&temp),4);
    ui->totaltime_label->setText(second_to_HHMMSS(temp));
    int all=QTime(0,0,0).secsTo(QTime::currentTime());
    int _now=all-temp;
    int sec=_now%60;
    _now/=60;
    int m=_now%60;
    _now/=60;
    int h=_now;
    total_starttime=new QTime(h,m,sec);
    fin->read((char*)(&temp),4);
    ui->lcdNumber->display(second_to_MMSS(temp));
    _now=all-temp;
    sec=_now%60;
    _now/=60;
    m=_now%60;
    _now/=60;
    h=_now;
    signal_starttime=new QTime(h,m,sec);
    onesecond_timer=new QTimer(this);
    connect(onesecond_timer,SIGNAL(timeout()),this,SLOT(update_total_time()));
    connect(onesecond_timer,SIGNAL(timeout()),this,SLOT(update_signal_time()));
    onesecond_timer->setInterval(1000);
    onesecond_timer->start();
    fin->read((char*)chessboard,225*4);
    fin->read((char*)(&temp),4);
    int x,y;
    QPoint *p;
    for(int i=0;i<temp;i++)
    {
        fin->read((char*)(&x),4);
        fin->read((char*)(&y),4);
        p=new QPoint(x,y);
        whitepoint.push(*p);
        White *w=new White(int(height()*boundary_percentage*1.1),this);
        QPoint p(height()*boundary_percentage+y*interval_percentage*height()-11,height()*boundary_percentage+x*interval_percentage*height()-11);
        w->move(p);
        whitechess.push(w);
    }
    fin->read((char*)(&temp),4);
    for(int i=0;i<temp;i++)
    {
        fin->read((char*)(&x),4);
        fin->read((char*)(&y),4);
        p=new QPoint(x,y);
        blackpoint.push(*p);
        Black *w=new Black(int(height()*boundary_percentage*1.1),this);
        QPoint p(height()*boundary_percentage+y*interval_percentage*height()-11,height()*boundary_percentage+x*interval_percentage*height()-11);
        w->move(p);
        blackchess.push(w);
    }
    show();
}

bool Widget::judge_draw()   //判断是否平局
{
    bool flag=true;
    for(int i=0;i<15;i++)
    {
        for(int j=0;j<15;j++)
            if(chessboard[i][j]==2)
            {
                flag=false;
                break;
            }
        if(!flag)
            break;
    }
    return flag;
}

int Widget::detail_case(int r, int c, int nearr, int nearc, bool Now)  //检测落子在不在他的判断方向上的其他几个方向 在落完该子后是否出现三子情况
{
    if(r-3>0&&chessboard[r-1][c]==Now&&chessboard[r-2][c]==Now&&chessboard[r-3][c]==Now&&(r-1!=nearr||c!=nearc)&&((r+1<15&&chessboard[r+1][c]==2)||(r-4>0&&chessboard[r-4][c]==2)))
       return 1;
    if(r+3<15&&chessboard[r+1][c]==Now&&chessboard[r+2][c]==Now&&chessboard[r+3][c]==Now&&(r+1!=nearr||c!=nearc)&&((r-1>0&&chessboard[r-1][c]==2)||(r+4<15&&chessboard[r+4][c]==2)))
       return 1;
    if(r-3>0&&r+2<15&&(r-1!=nearr||c!=nearc)&&(r+1!=nearr||c!=nearc)&&chessboard[r-3][c]==2&&chessboard[r+2][c]==2)
    {
        if(chessboard[r+1][c]==Now&&chessboard[r-1][c]==Now&&chessboard[r-2][c]==2)
                   return 1;
        if(chessboard[r-1][c]==Now&&chessboard[r-2][c]==Now&&chessboard[r+1][c]==2)
                   return 1;
    }
    if(r+3<15&&r-2>0&&(r+1!=nearr||c!=nearc)&&(r-1!=nearr||c!=nearc)&&chessboard[r+3][c]==2&&chessboard[r-2][c]==2)
    {
        if(chessboard[r+1][c]==Now&&chessboard[r-1][c]==Now&&chessboard[r-2][c]==2)
                   return 1;
        if(chessboard[r+1][c]==Now&&chessboard[r+2][c]==Now&&chessboard[r-1][c]==2)
                   return 1;
    }
    if(r-4>0&&r+1<15&&chessboard[r-1][c]==Now&&chessboard[r-2][c]==Now&&chessboard[r-3][c]==2&&chessboard[r-4][c]==2&&chessboard[r+1][c]==2&&(r-1!=nearr||c!=nearc))
               return 1;
    if(r+4<15&&r-1>0&&chessboard[r+1][c]==Now&&chessboard[r+2][c]==Now&&chessboard[r+3][c]==2&&chessboard[r+4][c]==2&&chessboard[r-1][c]==2&&(r+1!=nearr||c!=nearc))
               return 1;


    if(c-3>0&&chessboard[r][c-1]==Now&&chessboard[r][c-2]==Now&&chessboard[r][c-3]==Now&&(c-1!=nearc||r!=nearr)&&((c+1<15&&chessboard[r][c+1]==2)||(c-4>0&&chessboard[r][c-4]==2)))
              return 1;
    if(c+3<15&&chessboard[r][c+1]==Now&&chessboard[r][c+2]==Now&&chessboard[r][c+3]==Now&&(c+1!=nearc||r!=nearr)&&((c-1>0&&chessboard[r][c-1]==2)||(c+4<15&&chessboard[r][c+4]==2)))
              return 1;
    if(c-3>0&&c+2<15&&(c-1!=nearc||r!=nearr)&&(c+1!=nearc||r!=nearr)&&chessboard[r][c-3]==2&&chessboard[r][c+2]==2)
    {
        if(chessboard[r][c+1]==Now&&chessboard[r][c-1]==Now&&chessboard[r][c-2]==2)
                 return 1;
        if(chessboard[r][c-1]==Now&&chessboard[r][c-2]==Now&&chessboard[r][c+1]==2)
                 return 1;
    }
    if(c+3<15&&c-2>0&&(c+1!=nearc||r!=nearr)&&(c-1!=nearc||r!=nearr)&&chessboard[r][c+3]==2&&chessboard[r][c-2]==2)
    {
        if(chessboard[r][c+1]==Now&&chessboard[r][c-1]==Now&&chessboard[r][c-2]==2)
                  return 1;
        if(chessboard[r][c+1]==Now&&chessboard[r][c+2]==Now&&chessboard[r][c-1]==2)
                  return 1;
    }
    if(c-4>0&&c+1<15&&chessboard[r][c-1]==Now&&chessboard[r][c-2]==Now&&chessboard[r][c-3]==2&&chessboard[r][c-4]==2&&chessboard[r][c+1]==2&&(r!=nearr||c-1!=nearc))
                 return 1;
    if(c+4<15&&c-1>0&&chessboard[r][c+1]==Now&&chessboard[r][c+2]==Now&&chessboard[r][c+3]==2&&chessboard[r][c+4]==2&&chessboard[r][c-1]==2&&(r!=nearr||c+1!=nearc))
                 return 1;


    if(r-3>0&&c-3>0&&(r-1!=nearr||c-1!=nearc)&&chessboard[r-1][c-1]==Now&&chessboard[r-2][c-2]==Now&&chessboard[r-3][c-3]==Now&&((r-4>0&&c-4>0&&chessboard[r-4][c-4]==2)||(r+1<15&&c+1<15&&chessboard[r+1][c+1]==2)))
               return 1;
    if(r+3<15&&c+3<15&&(r+1!=nearr||c+1!=nearc)&&chessboard[r+1][c+1]==Now&&chessboard[r+2][c+2]==Now&&chessboard[r+3][c+3]==Now&&((r+4<15&&c+4<15&&chessboard[r+4][c+4]==2)||(r-1>0&&c-1>0&&chessboard[r-1][c-1]==2)))
               return 1;
    if(c-3>0&&r-3>0&&r+2<15&&c+2<15&&((c+1!=nearc||r+1!=nearr)&&(c-1!=nearc||r-1!=nearr))&&chessboard[r-3][c-3]==2&&chessboard[r+2][c+2]==2&&chessboard[r-1][c-1]==Now)
    {
        if(chessboard[r-2][c-2]==Now&&chessboard[r+1][c+1]==2)
                   return 1;
        if(chessboard[r+1][c+1]==Now&&chessboard[r-2][c-2]==2)
                   return 1;
    }
    if(c+3<15&&r+3<15&&r-2>0&&c-2>0&&((c+1!=nearc||r+1!=nearr)&&(c-1!=nearc||r-1!=nearr))&&chessboard[r+3][c+3]==2&&chessboard[r-2][c-2]==2&&chessboard[r+1][c+1]==Now)
    {
        if(chessboard[r+2][c+2]==Now&&chessboard[r-1][c-1]==2)
                   return 1;
        if(chessboard[r-1][c-1]==Now&&chessboard[r+2][c+2]==2)
                   return 1;
    }
    if(c-4>0&&r-4>0&&r+1<15&&c+1<15&&(c-1!=nearc||r-1!=nearr)&&chessboard[r-1][c-1]==Now&&chessboard[r-2][c-2]==Now&&chessboard[r-3][c-3]==Now&&chessboard[r-4][c-4]==2&&chessboard[r+1][c+1]==2)
               return 1;
    if(c+4<15&&r+4<15&&r-1>0&&c-1>0&&(c+1!=nearc||r+1!=nearr)&&chessboard[r+1][c+1]==Now&&chessboard[r+2][c+2]==Now&&chessboard[r+3][c+3]==Now&&chessboard[r+4][c+4]==2&&chessboard[r-1][c-1]==2)
               return 1;


    if(r-3>0&&c+3<15&&(r-1!=nearr||c+1!=nearc)&&chessboard[r-1][c+1]==Now&&chessboard[r-2][c+2]==Now&&chessboard[r-3][c+3]==Now&&((r-4>0&&c+4<15&&chessboard[r-4][c+4]==2)||(r+1<15&&c-1>0&&chessboard[r+1][c-1]==2)))
               return 1;
    if(r+3<15&&c-3>0&&(r+1!=nearr||c-1!=nearc)&&chessboard[r+1][c-1]==Now&&chessboard[r+2][c-2]==Now&&chessboard[r+3][c-3]==Now&&((r+4<15&&c-4>0&&chessboard[r+4][c-4]==2)||(r-1>0&&c+1<15&&chessboard[r-1][c+1]==2)))
              return 1;
    if(c-3>0&&r+3<15&&r-2>0&&c+2<15&&((c-1!=nearc||r+1!=nearr)&&(c+1!=nearc||r-1!=nearr))&&chessboard[r+3][c-3]==2&&chessboard[r-2][c+2]==2&&chessboard[r+1][c-1]==Now)
    {
        if(chessboard[r+2][c-2]==Now&&chessboard[r-1][c+1]==2)
                   return 1;
        if(chessboard[r-1][c+1]==Now&&chessboard[r+2][c-2]==2)
                   return 1;
    }
    if(c+3<15&&r-3>0&&r+2<15&&c-2>0&&((c-1!=nearc||r+1!=nearr)&&(c+1!=nearc||r-1!=nearr))&&chessboard[r-3][c+3]==2&&chessboard[r+2][c-2]==2&&chessboard[r-1][c+1]==Now)
    {
        if(chessboard[r-2][c+2]==Now&&chessboard[r+1][c-1]==2)
                   return 1;
        if(chessboard[r+1][c-1]==Now&&chessboard[r-2][c+2]==2)
                   return 1;
    }
    if(c+4<15&&r-4>0&&r+1<15&&c-1>0&&(c+1!=nearc||r-1!=nearr)&&chessboard[r-1][c+1]==Now&&chessboard[r-2][c+2]==Now&&chessboard[r-3][c+3]==Now&&chessboard[r-4][c+4]==2&&chessboard[r+1][c-1]==2)
               return 1;
    if(r+4<15&&c-4>0&&c+1<15&&r-1>0&&(r+1!=nearr||c-1!=nearc)&&chessboard[r+1][c-1]==Now&&chessboard[r+2][c-2]==Now&&chessboard[r+3][c-3]==Now&&chessboard[r+4][c-4]==2&&chessboard[r-1][c+1]==2)
              return 1;

    return 0;
}

QPoint Widget::find()   //寻找最佳落子
{
    int AI=player_first;
    int Player=!player_first;
    int Now,countchess;
    int c,r,cpr,cpc;
    int flag1,flag2;
    bool last_is_empty=false;
    bool next_is_empty=false;
    bool lastlast_is_empty=false;
    bool nextnext_is_empty=false;
    bool lastlastlast_is_empty=false;
    bool nextnextnext_is_empty=false;
    node* head=new node;
    node* now=head;
    head->next=NULL;
    head->level=-5;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));  //增加落子的随机性
    for(r=0;r<15;r++) //按照行来搜索
    {
        for(c=0;c<15;)
        {
            for(;c<15;c++)
                if(chessboard[r][c]!=2)
                    break;
            Now=chessboard[r][c];
            if(c-1>=0&&chessboard[r][c-1]==2)//记录上一个、上上个、上上上个位置是否为空
                last_is_empty=true;
            else
                last_is_empty=false;
            if(c-2>=0&&chessboard[r][c-2]==2&&last_is_empty)
                lastlast_is_empty=true;
            else
                lastlast_is_empty=false;
            if(c-3>=0&&chessboard[r][c-3]==2&&lastlast_is_empty)
                lastlastlast_is_empty=true;
            else
                lastlastlast_is_empty=false;
            countchess=1;
            for(c=c+1;c<15;c++)
            {
                if(chessboard[r][c]==Now)
                    countchess++;
                else
                    break;
            }
            if(c<15&&chessboard[r][c]==2)//记录下一个、下下个、下下下个位置是否为空
                next_is_empty=true;
            else
                next_is_empty=false;
            if(c+1<15&&chessboard[r][c+1]==2&&next_is_empty)
                nextnext_is_empty=true;
            else
                nextnext_is_empty=false;
            if(c+2<15&&chessboard[r][c+2]==2&&nextnext_is_empty)
                nextnextnext_is_empty=true;
            else
                nextnextnext_is_empty=false;
            if(countchess==4)	//判断四个连子时落子评分
            {
                if(Now==AI)
                {
                    if(next_is_empty)
                        return QPoint(r,c);
                    else if(last_is_empty)
                        return QPoint(r,c-5);
                }
                else if(Now==Player)
                {
                    if(next_is_empty)
                        now->add(r,c,must_lose,now);
                    else if(last_is_empty)
                        now->add(r,c-5,must_lose,now);
                }
            }
            if(countchess==3&&(last_is_empty||next_is_empty))	//判断三个连子时落子评分
            {
                flag1=0,flag2=0;
                if(next_is_empty&&c+1<15&&chessboard[r][c+1]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c);
                    else
                        now->add(r,c,must_lose,now);
                }
                if(last_is_empty&&c-5>=0&&chessboard[r][c-5]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c-4);
                    else
                        now->add(r,c-4,must_lose,now);
                }
                if(next_is_empty)
                    flag1=detail_case(r,c,r,c-1,Now);
                if(last_is_empty)
                    flag2=detail_case(r,c-4,r,c-3,Now);
                if(nextnext_is_empty&&flag1)
                {
                    if(Now==Player)
                        now->add(r,c,more_will_lose,now);
                    else
                        now->add(r,c,more_will_win,now);
                }
                if(lastlast_is_empty&&flag2)
                {
                    if(Now==Player)
                        now->add(r,c-4,more_will_lose,now);
                    else
                        now->add(r,c-4,more_will_win,now);
                }
                if(nextnext_is_empty&&last_is_empty)
                {
                    if(Now==Player)
                        now->add(r,c,will_lose,now);
                    else if(Now==AI)
                        now->add(r,c,will_win,now);
                }
                if(lastlast_is_empty&&next_is_empty)
                {
                    if(Now==Player)
                        now->add(r,c-4,will_lose,now);
                    else if(Now==AI)
                        now->add(r,c-4,will_win,now);
                }
                if(last_is_empty&&(lastlast_is_empty||next_is_empty))
                {
                    if(Now==Player)
                        now->add(r,c-4,willwill_lose,now);
                    else if(Now==AI)
                        now->add(r,c-4,more_will_win,now);
                }
                else if(next_is_empty&&(nextnext_is_empty||last_is_empty))
                {
                    if(Now==Player)
                        now->add(r,c,willwill_lose,now);
                    else if(Now==AI)
                        now->add(r,c,willwill_lose,now);
                }
                else
                {
                    if(next_is_empty)
                    {
                        if(Now==Player)
                            now->add(r,c,hard_lose,now);
                        else if(Now==AI)
                            now->add(r,c,hard_win,now);
                    }
                    else if(last_is_empty)
                    {
                        if(Now==Player)
                            now->add(r,c,hard_lose,now);
                        else if(Now==AI)
                            now->add(r,c,hard_win,now);
                    }
                }
            }
            if(countchess==2)	//判断两个连子时落子评分
            {
                flag1=flag2=0;
                if(next_is_empty)
                    flag1=detail_case(r,c,r,c-1,Now);
                if(last_is_empty)
                    flag2=detail_case(r,c-3,r,c-2,Now);
            }
            if(countchess==2&&next_is_empty&&c+1<15&&chessboard[r][c+1]==Now)
            {
                if(c+2<15&&chessboard[r][c+2]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c);
                    else
                        now->add(r,c,must_lose,now);
                }
                else if(((c+2<15&&chessboard[r][c+2]==2)+(flag1==1)+(last_is_empty==true))>=2)
                {
                    if(Now==AI)
                        now->add(r,c,more_will_win,now);
                    else
                        now->add(r,c,more_will_lose,now);
                }
                else if(((c+2<15&&chessboard[r][c+2]==2)+(flag1==1)+(last_is_empty==true))>=1)
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                else
                {
                    if(Now==AI)
                        now->add(r,c,hard_win,now);
                    else
                        now->add(r,c,hard_lose,now);
                }
            }
            if(countchess==2&&last_is_empty&&c-4>=0&&chessboard[r][c-4]==Now)
            {
                if(c-5>=0&&chessboard[r][c-5]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c-3);
                    else
                        now->add(r,c-3,must_lose,now);
                }
                else if(((c-5>=0&&chessboard[r][c-5]==2)+(flag2==1)+(next_is_empty))>=2)
                {
                    if(Now==AI)
                        now->add(r,c-3,more_will_win,now);
                    else
                        now->add(r,c-3,more_will_lose,now);
                }
                else if(((c-5>=0&&chessboard[r][c-5]==2)+(flag2==1)+(next_is_empty))>=1)
                {
                    if(Now==AI)
                        now->add(r,c-3,willwill_win,now);
                    else
                        now->add(r,c-3,willwill_lose,now);
                }
                else
                {
                    if(Now==AI)
                        now->add(r,c-3,hard_win,now);
                    else
                        now->add(r,c-3,hard_lose,now);
                }
            }
            if(countchess==2&&last_is_empty&&next_is_empty)
            {
                if(flag1)
                {
                    if((nextnext_is_empty&&lastlast_is_empty)||(nextnextnext_is_empty))
                    {
                        if(Now==AI)
                            now->add(r,c,more_will_win,now);
                        else
                            now->add(r,c,more_will_lose,now);
                    }
                    else if(lastlast_is_empty||nextnext_is_empty)
                    {
                        if(Now==AI)
                            now->add(r,c,willwill_win,now);
                        else
                            now->add(r,c,willwill_win,now);
                    }
                    else
                    {
                        if(Now==AI)
                            now->add(r,c,may_win,now);
                        else
                            now->add(r,c,may_lose,now);
                    }
                }
                if(flag2)
                {
                    if((nextnext_is_empty&&lastlast_is_empty)||(lastlastlast_is_empty))
                    {
                        if(Now==AI)
                            now->add(r,c-3,more_will_win,now);
                        else
                            now->add(r,c-3,more_will_lose,now);
                    }
                    else if(lastlast_is_empty||nextnext_is_empty)
                    {
                        if(Now==AI)
                            now->add(r,c-3,willwill_win,now);
                        else
                            now->add(r,c-3,willwill_win,now);
                    }
                    else
                    {
                        if(Now==AI)
                            now->add(r,c-3,may_win,now);
                        else
                            now->add(r,c-3,may_lose,now);
                    }
                }
                if(nextnextnext_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                if(nextnext_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c,may_win,now);
                    else
                        now->add(r,c,may_lose,now);
                }
                if(lastlastlast_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c-3,willwill_win,now);
                    else
                        now->add(r,c-3,willwill_lose,now);
                }
                if(lastlast_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c-3,may_win,now);
                    else
                        now->add(r,c-3,may_lose,now);
                }
            }
            if(countchess==1)	//判断仅有一个子时 落子评分
            {
                if(next_is_empty&&last_is_empty&&c+1<15&&chessboard[r][c+1]==Now&&(lastlast_is_empty||(c+2<15&&chessboard[r][c+2]==2)))
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c-3>=0&&chessboard[r][c-3]==Now&&(nextnext_is_empty||(c-4>=0&&chessboard[r][c-4]==2)))
                {
                    if(Now==AI)
                        now->add(r,c-2,willwill_win,now);
                    else
                        now->add(r,c-2,willwill_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c+1<15&&chessboard[r][c+1]==Now)
                {
                    if(Now==AI)
                        now->add(r,c,may_win,now);
                    else
                        now->add(r,c,may_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c-3>=0&&chessboard[r][c-3]==Now)
                {
                    if(Now==AI)
                        now->add(r,c-2,may_win,now);
                    else
                        now->add(r,c-2,may_lose,now);
                }
            }
        }
    }

    for(c=0;c<15;c++)	//按列来搜索
    {
        for(r=0;r<15;)
        {
            for(;r<15;r++)
                if(chessboard[r][c]!=2)
                    break;
            Now=chessboard[r][c];
            if(r-1>=0&&chessboard[r-1][c]==2)
                last_is_empty=true;
            else
                last_is_empty=false;
            if(r-2>=0&&chessboard[r-2][c]==2&&last_is_empty)
                lastlast_is_empty=true;
            else
                lastlast_is_empty=false;
            if(r-3>=0&&chessboard[r-3][c]==2&&lastlast_is_empty)
                lastlastlast_is_empty=true;
            else
                lastlastlast_is_empty=false;
            countchess=1;
            for(r=r+1;r<15;r++)
            {
                if(chessboard[r][c]==Now)
                    countchess++;
                else
                    break;
            }
            if(r<15&&chessboard[r][c]==2)
                next_is_empty=true;
            else
                next_is_empty=false;
            if(r+1<15&&chessboard[r+1][c]==2&&next_is_empty)
                nextnext_is_empty=true;
            else
                nextnext_is_empty=false;
            if(r+2<15&&chessboard[r+2][c]==2&&nextnext_is_empty)
                nextnextnext_is_empty=true;
            else
                nextnextnext_is_empty=false;
            if(countchess==4)
            {
                if(Now==AI)
                {
                    if(next_is_empty)
                        return QPoint(r,c);
                    else if(last_is_empty)
                        return QPoint(r-5,c);
                }
                else if(Now==Player)
                {
                    if(next_is_empty)
                        now->add(r,c,must_lose,now);
                    else if(last_is_empty)
                        now->add(r-5,c,must_lose,now);
                }
            }
            if(countchess==3&&(last_is_empty||next_is_empty))
            {
                flag1=0,flag2=0;
                if(next_is_empty&&r+1<15&&chessboard[r+1][c]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c);
                    else
                        now->add(r,c,must_lose,now);
                }
                if(last_is_empty&&r-5>=0&&chessboard[r-5][c]==Now)
                {
                    if(Now==AI)
                        return QPoint(r-4,c);
                    else
                        now->add(r-4,c,must_lose,now);
                }
                if(next_is_empty)
                    flag1=detail_case(r,c,r-1,c,Now);
                if(last_is_empty)
                    flag2=detail_case(r-4,c,r-3,c,Now);
                if(nextnext_is_empty&&flag1)
                {
                    if(Now==Player)
                        now->add(r,c,more_will_lose,now);
                    else
                        now->add(r,c,more_will_win,now);
                }
                if(lastlast_is_empty&&flag2)
                {
                    if(Now==Player)
                        now->add(r-4,c,more_will_lose,now);
                    else
                        now->add(r-4,c,more_will_win,now);
                }
                if(nextnext_is_empty&&last_is_empty)
                {
                    if(Now==Player)
                        now->add(r,c,will_lose,now);
                    else if(Now==AI)
                        now->add(r,c,will_win,now);
                }
                if(lastlast_is_empty&&next_is_empty)
                {
                    if(Now==Player)
                        now->add(r-4,c,will_lose,now);
                    else if(Now==AI)
                        now->add(r-4,c,will_win,now);
                }
                if(last_is_empty&&(lastlast_is_empty||next_is_empty))
                {
                    if(Now==Player)
                        now->add(r-4,c,willwill_lose,now);
                    else if(Now==AI)
                        now->add(r-4,c,more_will_win,now);
                }
                else if(next_is_empty&&(nextnext_is_empty||last_is_empty))
                {
                    if(Now==Player)
                        now->add(r,c,willwill_lose,now);
                    else if(Now==AI)
                        now->add(r,c,willwill_lose,now);
                }
                else
                {
                    if(next_is_empty)
                    {
                        if(Now==Player)
                            now->add(r,c,hard_lose,now);
                        else if(Now==AI)
                            now->add(r,c,hard_win,now);
                    }
                    else if(last_is_empty)
                    {
                        if(Now==Player)
                            now->add(r,c,hard_lose,now);
                        else if(Now==AI)
                            now->add(r,c,hard_win,now);
                    }
                }
            }
            if(countchess==2)
            {
                flag1=flag2=0;
                if(next_is_empty)
                    flag1=detail_case(r,c,r-1,c,Now);
                if(last_is_empty)
                    flag2=detail_case(r-3,c,r-2,c,Now);
            }
            if(countchess==2&&next_is_empty&&r+1<15&&chessboard[r+1][c]==Now)
            {
                if(r+2<15&&chessboard[r+2][c]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c);
                    else
                        now->add(r,c,must_lose,now);
                }
                else if(((r+2<15&&chessboard[r+2][c]==2)+(flag1==1)+(last_is_empty==true))>=2)
                {
                    if(Now==AI)
                        now->add(r,c,more_will_win,now);
                    else
                        now->add(r,c,more_will_lose,now);
                }
                else if(((r+2<15&&chessboard[r+2][c]==2)+(flag1==1)+(last_is_empty==true))>=1)
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                else
                {
                    if(Now==AI)
                        now->add(r,c,hard_win,now);
                    else
                        now->add(r,c,hard_lose,now);
                }
            }
            if(countchess==2&&last_is_empty&&r-4>=0&&chessboard[r-4][c]==Now)
            {
                if(r-5>=0&&chessboard[r-5][c]==Now)
                {
                    if(Now==AI)
                        return QPoint(r-3,c);
                    else
                        now->add(r-3,c,must_lose,now);
                }
                else if(((r-5>=0&&chessboard[r-5][c]==2)+(flag2==1)+(next_is_empty))>=2)
                {
                    if(Now==AI)
                        now->add(r-3,c,more_will_win,now);
                    else
                        now->add(r-3,c,more_will_lose,now);
                }
                else if(((r-5>=0&&chessboard[r-5][c]==2)+(flag2==1)+(next_is_empty))>=1)
                {
                    if(Now==AI)
                        now->add(r-3,c,willwill_win,now);
                    else
                        now->add(r-3,c,willwill_lose,now);
                }
                else
                {
                    if(Now==AI)
                        now->add(r-3,c,hard_win,now);
                    else
                        now->add(r-3,c,hard_lose,now);
                }
            }
            if(countchess==2&&last_is_empty&&next_is_empty)
            {
                if(flag1)
                {
                    if((nextnext_is_empty&&lastlast_is_empty)||(nextnextnext_is_empty))
                    {
                        if(Now==AI)
                            now->add(r,c,more_will_win,now);
                        else
                            now->add(r,c,more_will_lose,now);
                    }
                    else if(lastlast_is_empty||nextnext_is_empty)
                    {
                        if(Now==AI)
                            now->add(r,c,willwill_win,now);
                        else
                            now->add(r,c,willwill_win,now);
                    }
                    else
                    {
                        if(Now==AI)
                            now->add(r,c,may_win,now);
                        else
                            now->add(r,c,may_lose,now);
                    }
                }
                if(flag2)
                {
                    if((nextnext_is_empty&&lastlast_is_empty)||(lastlast_is_empty))
                    {
                        if(Now==AI)
                            now->add(r-3,c,more_will_win,now);
                        else
                            now->add(r-3,c,more_will_lose,now);
                    }
                    else if(lastlast_is_empty||nextnext_is_empty)
                    {
                        if(Now==AI)
                            now->add(r-3,c,willwill_win,now);
                        else
                            now->add(r-3,c,willwill_win,now);
                    }
                    else
                    {
                        if(Now==AI)
                            now->add(r-3,c,may_win,now);
                        else
                            now->add(r-3,c,may_lose,now);
                    }
                }
                if(nextnextnext_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                if(nextnext_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c,may_win,now);
                    else
                        now->add(r,c,may_lose,now);
                }
                if(lastlastlast_is_empty)
                {
                    if(Now==AI)
                        now->add(r-3,c,willwill_win,now);
                    else
                        now->add(r-3,c,willwill_lose,now);
                }
                if(lastlast_is_empty)
                {
                    if(Now==AI)
                        now->add(r-3,c,may_win,now);
                    else
                        now->add(r-3,c,may_lose,now);
                }
            }
            if(countchess==1)
            {
                if(next_is_empty&&last_is_empty&&r+1<15&&chessboard[r+1][c]==Now&&(lastlast_is_empty||(r+2<15&&chessboard[r+2][c]==2)))
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                if(next_is_empty&&last_is_empty&&r-3>=0&&chessboard[r-3][c]==Now&&(nextnext_is_empty||(r-4>=0&&chessboard[r-4][c]==2)))
                {
                    if(Now==AI)
                        now->add(r-2,c,willwill_win,now);
                    else
                        now->add(r-2,c,willwill_lose,now);
                }
                if(next_is_empty&&last_is_empty&&r+1<15&&chessboard[r+1][c]==Now)
                {
                    if(Now==AI)
                        now->add(r,c,may_win,now);
                    else
                        now->add(r,c,may_lose,now);
                }
                if(next_is_empty&&last_is_empty&&r-3>=0&&chessboard[r-3][c]==Now)
                {
                    if(Now==AI)
                        now->add(r-2,c,may_win,now);
                    else
                        now->add(r-2,c,may_lose,now);
                }
            }
        }
    }

    for(cpr=0;cpr<11;cpr++)	//按斜下搜索 左下棋盘
    {
        for(r=cpr,c=0;c<15;)
        {
            for(;c<15&&r<15;c++,r++)
                if(chessboard[r][c]!=2)
                    break;
            Now=chessboard[r][c];
            if(c-1>=0&&r-1>=0&&chessboard[r-1][c-1]==2)
                last_is_empty=true;
            else
                last_is_empty=false;
            if(c-2>=0&&r-2>=0&&chessboard[r-2][c-2]==2&&last_is_empty)
                lastlast_is_empty=true;
            else
                lastlast_is_empty=false;
            if(c-3>=0&&r-3>=0&&chessboard[r-3][c-3]==2&&lastlast_is_empty)
                lastlastlast_is_empty=true;
            else
                lastlastlast_is_empty=false;
            countchess=1;
            for(c=c+1,r=r+1;c<15&&r<15;c++,r++)
            {
                if(chessboard[r][c]==Now)
                    countchess++;
                else
                    break;
            }
            if(c<15&&r<15&&chessboard[r][c]==2)
                next_is_empty=true;
            else
                next_is_empty=false;
            if(c+1<15&&r+1<15&&chessboard[r+1][c+1]==2&&next_is_empty)
                nextnext_is_empty=true;
            else
                nextnext_is_empty=false;
            if(c+2<15&&r+2<15&&chessboard[r+2][c+2]==2&&nextnext_is_empty)
                nextnextnext_is_empty=true;
            else
                nextnextnext_is_empty=false;
            if(countchess==4)
            {
                if(Now==AI)
                {
                    if(next_is_empty)
                        return QPoint(r,c);
                    else if(last_is_empty)
                        return QPoint(r-5,c-5);
                }
                else if(Now==Player)
                {
                    if(next_is_empty)
                        now->add(r,c,must_lose,now);
                    else if(last_is_empty)
                        now->add(r-5,c-5,must_lose,now);
                }
            }
            if(countchess==3&&(last_is_empty||next_is_empty))
            {
                flag1=0,flag2=0;
                if(next_is_empty&&c+1<15&&r+1<15&&chessboard[r+1][c+1]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c);
                    else
                        now->add(r,c,must_lose,now);
                }
                if(last_is_empty&&c-5>=0&&r-5>=0&&chessboard[r-5][c-5]==Now)
                {
                    if(Now==AI)
                        return QPoint(r-4,c-4);
                    else
                        now->add(r-4,c-4,must_lose,now);
                }
                if(next_is_empty)
                    flag1=detail_case(r,c,r-1,c-1,Now);
                if(last_is_empty)
                    flag2=detail_case(r-4,c-4,r-3,c-3,Now);
                if(nextnext_is_empty&&flag1)
                {
                    if(Now==Player)
                        now->add(r,c,more_will_lose,now);
                    else
                        now->add(r,c,more_will_win,now);
                }
                if(lastlast_is_empty&&flag2)
                {
                    if(Now==Player)
                        now->add(r-4,c-4,more_will_lose,now);
                    else
                        now->add(r-4,c-4,more_will_win,now);
                }
                if(nextnext_is_empty&&last_is_empty)
                {
                    if(Now==Player)
                        now->add(r,c,will_lose,now);
                    else if(Now==AI)
                        now->add(r,c,will_win,now);
                }
                if(lastlast_is_empty&&next_is_empty)
                {
                    if(Now==Player)
                        now->add(r-4,c-4,will_lose,now);
                    else if(Now==AI)
                        now->add(r-4,c-4,will_win,now);
                }
                if(last_is_empty&&(lastlast_is_empty||next_is_empty))
                {
                    if(Now==Player)
                        now->add(r-4,c-4,willwill_lose,now);
                    else if(Now==AI)
                        now->add(r-4,c-4,more_will_win,now);
                }
                else if(next_is_empty&&(nextnext_is_empty||last_is_empty))
                {
                    if(Now==Player)
                        now->add(r,c,willwill_lose,now);
                    else if(Now==AI)
                        now->add(r,c,willwill_lose,now);
                }
                else
                {
                    if(next_is_empty)
                    {
                        if(Now==Player)
                            now->add(r,c,hard_lose,now);
                        else if(Now==AI)
                            now->add(r,c,hard_win,now);
                    }
                    else if(last_is_empty)
                    {
                        if(Now==Player)
                            now->add(r,c,hard_lose,now);
                        else if(Now==AI)
                            now->add(r,c,hard_win,now);
                    }
                }
            }
            if(countchess==2)
            {
                flag1=flag2=0;
                if(next_is_empty)
                    flag1=detail_case(r,c,r-1,c-1,Now);
                if(last_is_empty)
                    flag2=detail_case(r-3,c-3,r-2,c-2,Now);
            }
            if(countchess==2&&next_is_empty&&r+1<15&&c+1<15&&chessboard[r+1][c+1]==Now)
            {
                if(r+2<15&&c+2<15&&chessboard[r+2][c+2]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c);
                    else
                        now->add(r,c,must_lose,now);
                }
                else if(((r+2<15&&c+2<15&&chessboard[r+2][c+2]==2)+(flag1==1)+(last_is_empty==true))>=2)
                {
                    if(Now==AI)
                        now->add(r,c,more_will_win,now);
                    else
                        now->add(r,c,more_will_lose,now);
                }
                else if(((r+2<15&&c+2<15&&chessboard[r+2][c+2]==2)+(flag1==1)+(last_is_empty==true))>=1)
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                else
                {
                    if(Now==AI)
                        now->add(r,c,hard_win,now);
                    else
                        now->add(r,c,hard_lose,now);
                }
            }
            if(countchess==2&&last_is_empty&&r-4>=0&&c-4>=0&&chessboard[r-4][c-4]==Now)
            {
                if(r-5>=0&&c-5>=0&&chessboard[r-5][c-5]==Now)
                {
                    if(Now==AI)
                        return QPoint(r-3,c-3);
                    else
                        now->add(r-3,c-3,must_lose,now);
                }
                else if(((r-5>=0&&c-5>=0&&chessboard[r-5][c-5]==2)+(flag2==1)+(next_is_empty==true))>=2)
                {
                    if(Now==AI)
                        now->add(r-3,c-3,more_will_win,now);
                    else
                        now->add(r-3,c-3,more_will_lose,now);
                }
                else if(((r-5>=0&&c-5>=0&&chessboard[r-5][c-5]==2)+(flag2==1)+(next_is_empty==true))>=1)
                {
                    if(Now==AI)
                        now->add(r-3,c-3,willwill_win,now);
                    else
                        now->add(r-3,c-3,willwill_lose,now);
                }
                else
                {
                    if(Now==AI)
                        now->add(r-3,c-3,hard_win,now);
                    else
                        now->add(r-3,c-3,hard_lose,now);
                }
            }
            if(countchess==2&&last_is_empty&&next_is_empty)
            {
                if(flag1)
                {
                    if((nextnext_is_empty&&lastlast_is_empty)||(nextnextnext_is_empty))
                    {
                        if(Now==AI)
                            now->add(r,c,more_will_win,now);
                        else
                            now->add(r,c,more_will_lose,now);
                    }
                    else if(lastlast_is_empty||nextnext_is_empty)
                    {
                        if(Now==AI)
                            now->add(r,c,willwill_win,now);
                        else
                            now->add(r,c,willwill_win,now);
                    }
                    else
                    {
                        if(Now==AI)
                            now->add(r,c,may_win,now);
                        else
                            now->add(r,c,may_lose,now);
                    }
                }
                if(flag2)
                {
                    if((nextnext_is_empty&&lastlast_is_empty)||(lastlastlast_is_empty))
                    {
                        if(Now==AI)
                            now->add(r-3,c-3,more_will_win,now);
                        else
                            now->add(r-3,c-3,more_will_lose,now);
                    }
                    else if(lastlast_is_empty||nextnext_is_empty)
                    {
                        if(Now==AI)
                            now->add(r-3,c-3,willwill_win,now);
                        else
                            now->add(r-3,c-3,willwill_win,now);
                    }
                    else
                    {
                        if(Now==AI)
                            now->add(r-3,c-3,may_win,now);
                        else
                            now->add(r-3,c-3,may_lose,now);
                    }
                }
                if(nextnextnext_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                if(nextnext_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c,may_win,now);
                    else
                        now->add(r,c,may_lose,now);
                }
                if(lastlastlast_is_empty)
                {
                    if(Now==AI)
                        now->add(r-3,c-3,willwill_win,now);
                    else
                        now->add(r-3,c-3,willwill_lose,now);
                }
                if(lastlast_is_empty)
                {
                    if(Now==AI)
                        now->add(r-3,c-3,may_win,now);
                    else
                        now->add(r-3,c-3,may_lose,now);
                }
            }
            if(countchess==1)
            {
                if(next_is_empty&&last_is_empty&&c+1<15&&r+1<15&&chessboard[r+1][c+1]==Now&&(lastlast_is_empty||(c+2<15&&r+2<15&&chessboard[r+2][c+2]==2)))
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c-3>=0&&r-3>=0&&chessboard[r-3][c-3]==Now&&(nextnext_is_empty||(c-4>=0&&r-4>=0&&chessboard[r-4][c-4]==2)))
                {
                    if(Now==AI)
                        now->add(r-2,c-2,willwill_win,now);
                    else
                        now->add(r-2,c-2,willwill_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c+1<15&&r+1<15&&chessboard[r+1][c+1]==Now)
                {
                    if(Now==AI)
                        now->add(r,c,may_win,now);
                    else
                        now->add(r,c,may_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c-3>=0&&r-3>=0&&chessboard[r-3][c-3]==Now)
                {
                    if(Now==AI)
                        now->add(r-2,c-2,may_win,now);
                    else
                        now->add(r-2,c-2,may_lose,now);
                }
            }

        }
    }

    for(cpc=0;cpc<11;cpc++)	//按斜下搜索 右上棋盘
    {
        for(c=cpc,r=0;r<15;)
        {
            for(;c<15&&r<15;c++,r++)
                if(chessboard[r][c]!=2)
                    break;
            Now=chessboard[r][c];
            if(c-1>=0&&r-1>=0&&chessboard[r-1][c-1]==2)
                last_is_empty=true;
            else
                last_is_empty=false;
            if(c-2>=0&&r-2>=0&&chessboard[r-2][c-2]==2&&last_is_empty)
                lastlast_is_empty=true;
            else
                lastlast_is_empty=false;
            if(c-3>=0&&r-3>=0&&chessboard[r-3][c-3]==2&&lastlast_is_empty)
                lastlastlast_is_empty=true;
            else
                lastlastlast_is_empty=false;
            countchess=1;
            for(c=c+1,r=r+1;c<15&&r<15;c++,r++)
            {
                if(chessboard[r][c]==Now)
                    countchess++;
                else
                    break;
            }
            if(c<15&&r<15&&chessboard[r][c]==2)
                next_is_empty=true;
            else
                next_is_empty=false;
            if(c+1<15&&r+1<15&&chessboard[r+1][c+1]==2&&next_is_empty)
                nextnext_is_empty=true;
            else
                nextnext_is_empty=false;
            if(c+2<15&&r+2<15&&chessboard[r+2][c+2]==2&&nextnext_is_empty)
                nextnextnext_is_empty=true;
            else
                nextnextnext_is_empty=false;
            if(countchess==4)
            {
                if(Now==AI)
                {
                    if(next_is_empty)
                        return QPoint(r,c);
                    else if(last_is_empty)
                        return QPoint(r-5,c-5);
                }
                else if(Now==Player)
                {
                    if(next_is_empty)
                        now->add(r,c,must_lose,now);
                    else if(last_is_empty)
                        now->add(r-5,c-5,must_lose,now);
                }
            }
            if(countchess==3&&(last_is_empty||next_is_empty))
            {
                flag1=0,flag2=0;
                if(next_is_empty&&c+1<15&&r+1<15&&chessboard[r+1][c+1]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c);
                    else
                        now->add(r,c,must_lose,now);
                }
                if(last_is_empty&&c-5>=0&&r-5>=0&&chessboard[r-5][c-5]==Now)
                {
                    if(Now==AI)
                        return QPoint(r-4,c-4);
                    else
                        now->add(r-4,c-4,must_lose,now);
                }
                if(next_is_empty)
                    flag1=detail_case(r,c,r-1,c-1,Now);
                if(last_is_empty)
                    flag2=detail_case(r-4,c-4,r-3,c-3,Now);
                if(nextnext_is_empty&&flag1)
                {
                    if(Now==Player)
                        now->add(r,c,more_will_lose,now);
                    else
                        now->add(r,c,more_will_win,now);
                }
                if(lastlast_is_empty&&flag2)
                {
                    if(Now==Player)
                        now->add(r-4,c-4,more_will_lose,now);
                    else
                        now->add(r-4,c-4,more_will_win,now);
                }
                if(nextnext_is_empty&&last_is_empty)
                {
                    if(Now==Player)
                        now->add(r,c,will_lose,now);
                    else if(Now==AI)
                        now->add(r,c,will_win,now);
                }
                if(lastlast_is_empty&&next_is_empty)
                {
                    if(Now==Player)
                        now->add(r-4,c-4,will_lose,now);
                    else if(Now==AI)
                        now->add(r-4,c-4,will_win,now);
                }
                if(last_is_empty&&(lastlast_is_empty||next_is_empty))
                {
                    if(Now==Player)
                        now->add(r-4,c-4,willwill_lose,now);
                    else if(Now==AI)
                        now->add(r-4,c-4,more_will_win,now);
                }
                else if(next_is_empty&&(nextnext_is_empty||last_is_empty))
                {
                    if(Now==Player)
                        now->add(r,c,willwill_lose,now);
                    else if(Now==AI)
                        now->add(r,c,willwill_lose,now);
                }
                else
                {
                    if(next_is_empty)
                    {
                        if(Now==Player)
                            now->add(r,c,hard_lose,now);
                        else if(Now==AI)
                            now->add(r,c,hard_win,now);
                    }
                    else if(last_is_empty)
                    {
                        if(Now==Player)
                            now->add(r,c,hard_lose,now);
                        else if(Now==AI)
                            now->add(r,c,hard_win,now);
                    }
                }
            }
            if(countchess==2)
            {
                flag1=flag2=0;
                if(next_is_empty)
                    flag1=detail_case(r,c,r-1,c-1,Now);
                if(last_is_empty)
                    flag2=detail_case(r-3,c-3,r-2,c-2,Now);
            }
            if(countchess==2&&next_is_empty&&r+1<15&&c+1<15&&chessboard[r+1][c+1]==Now)
            {
                if(r+2<15&&c+2<15&&chessboard[r+2][c+2]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c);
                    else
                        now->add(r,c,must_lose,now);
                }
                else if(((r+2<15&&c+2<15&&chessboard[r+2][c+2]==2)+(flag1==1)+(last_is_empty==true))>=2)
                {
                    if(Now==AI)
                        now->add(r,c,more_will_win,now);
                    else
                        now->add(r,c,more_will_lose,now);
                }
                else if(((r+2<15&&c+2<15&&chessboard[r+2][c+2]==2)+(flag1==1)+(last_is_empty==true))>=1)
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                else
                {
                    if(Now==AI)
                        now->add(r,c,hard_win,now);
                    else
                        now->add(r,c,hard_lose,now);
                }
            }
            if(countchess==2&&last_is_empty&&r-4>=0&&c-4>=0&&chessboard[r-4][c-4]==Now)
            {
                if(r-5>=0&&c-5>=0&&chessboard[r-5][c-5]==Now)
                {
                    if(Now==AI)
                        return QPoint(r-3,c-3);
                    else
                        now->add(r-3,c-3,must_lose,now);
                }
                else if(((r-5>=0&&c-5>=0&&chessboard[r-5][c-5]==2)+(flag2==1)+(next_is_empty==true))>=2)
                {
                    if(Now==AI)
                        now->add(r-3,c-3,more_will_win,now);
                    else
                        now->add(r-3,c-3,more_will_lose,now);
                }
                else if(((r-5>=0&&c-5>=0&&chessboard[r-5][c-5]==2)+(flag2==1)+(next_is_empty==true))>=1)
                {
                    if(Now==AI)
                        now->add(r-3,c-3,willwill_win,now);
                    else
                        now->add(r-3,c-3,willwill_lose,now);
                }
                else
                {
                    if(Now==AI)
                        now->add(r-3,c-3,hard_win,now);
                    else
                        now->add(r-3,c-3,hard_lose,now);
                }
            }
            if(countchess==2&&last_is_empty&&next_is_empty)
            {
                if(flag1)
                {
                    if((nextnext_is_empty&&lastlast_is_empty)||(nextnextnext_is_empty))
                    {
                        if(Now==AI)
                            now->add(r,c,more_will_win,now);
                        else
                            now->add(r,c,more_will_lose,now);
                    }
                    else if(lastlast_is_empty||nextnext_is_empty)
                    {
                        if(Now==AI)
                            now->add(r,c,willwill_win,now);
                        else
                            now->add(r,c,willwill_win,now);
                    }
                    else
                    {
                        if(Now==AI)
                            now->add(r,c,may_win,now);
                        else
                            now->add(r,c,may_lose,now);
                    }
                }
                if(flag2)
                {
                    if((nextnext_is_empty&&lastlast_is_empty)||(lastlastlast_is_empty))
                    {
                        if(Now==AI)
                            now->add(r-3,c-3,more_will_win,now);
                        else
                            now->add(r-3,c-3,more_will_lose,now);
                    }
                    else if(lastlast_is_empty||nextnext_is_empty)
                    {
                        if(Now==AI)
                            now->add(r-3,c-3,willwill_win,now);
                        else
                            now->add(r-3,c-3,willwill_win,now);
                    }
                    else
                    {
                        if(Now==AI)
                            now->add(r-3,c-3,may_win,now);
                        else
                            now->add(r-3,c-3,may_lose,now);
                    }
                }
                if(nextnextnext_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                if(nextnext_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c,may_win,now);
                    else
                        now->add(r,c,may_lose,now);
                }
                if(lastlastlast_is_empty)
                {
                    if(Now==AI)
                        now->add(r-3,c-3,willwill_win,now);
                    else
                        now->add(r-3,c-3,willwill_lose,now);
                }
                if(lastlast_is_empty)
                {
                    if(Now==AI)
                        now->add(r-3,c-3,may_win,now);
                    else
                        now->add(r-3,c-3,may_lose,now);
                }
            }
            if(countchess==1)
            {
                if(next_is_empty&&last_is_empty&&c+1<15&&r+1<15&&chessboard[r+1][c+1]==Now&&(lastlast_is_empty||(c+2<15&&r+2<15&&chessboard[r+2][c+2]==2)))
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c-3>=0&&r-3>=0&&chessboard[r-3][c-3]==Now&&(nextnext_is_empty||(c-4>=0&&r-4>=0&&chessboard[r-4][c-4]==2)))
                {
                    if(Now==AI)
                        now->add(r-2,c-2,willwill_win,now);
                    else
                        now->add(r-2,c-2,willwill_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c+1<15&&r+1<15&&chessboard[r+1][c+1]==Now)
                {
                    if(Now==AI)
                        now->add(r,c,may_win,now);
                    else
                        now->add(r,c,may_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c-3>=0&&r-3>=0&&chessboard[r-3][c-3]==Now)
                {
                    if(Now==AI)
                        now->add(r-2,c-2,may_win,now);
                    else
                        now->add(r-2,c-2,may_lose,now);
                }
            }
        }
    }

    for(cpr=14;cpr>=4;cpr--) //按斜上搜索 左上棋盘
    {
        for(r=cpr,c=0;c<15;)
        {
            for(;c<15&&r>=0;c++,r--)
                if(chessboard[r][c]!=2)
                    break;
            Now=chessboard[r][c];
            if(c-1>=0&&r+1<15&&chessboard[r+1][c-1]==2)
                last_is_empty=true;
            else
                last_is_empty=false;
            if(c-2>=0&&r+2>=0&&chessboard[r+2][c-2]==2&&last_is_empty)
                lastlast_is_empty=true;
            else
                lastlast_is_empty=false;
            if(c-3>=0&&r+3>=0&&chessboard[r+3][c-3]==2&&lastlast_is_empty)
                lastlastlast_is_empty=true;
            else
                lastlastlast_is_empty=false;
            countchess=1;
            for(c=c+1,r=r-1;c<15&&r>=0;c++,r--)
            {
                if(chessboard[r][c]==Now)
                    countchess++;
                else
                    break;
            }
            if(c<15&&r>=0&&chessboard[r][c]==2)
                next_is_empty=true;
            else
                next_is_empty=false;
            if(c+1<15&&r-1>=0&&chessboard[r-1][c+1]==2&&next_is_empty)
                nextnext_is_empty=true;
            else
                nextnext_is_empty=false;
            if(c+2<15&&r-2>=0&&chessboard[r-2][c+2]==2&&nextnext_is_empty)
                nextnextnext_is_empty=true;
            else
                nextnextnext_is_empty=false;
            if(countchess==4)
            {
                if(Now==AI)
                {
                    if(next_is_empty)
                        return QPoint(r,c);
                    else if(last_is_empty)
                        return QPoint(r+5,c-5);
                }
                else if(Now==Player)
                {
                    if(next_is_empty)
                        now->add(r,c,must_lose,now);
                    else if(last_is_empty)
                        now->add(r+5,c-5,must_lose,now);
                }
            }
            if(countchess==3&&(last_is_empty||next_is_empty))
            {
                flag1=0,flag2=0;
                if(next_is_empty&&c+1<15&&r-1>=0&&chessboard[r-1][c+1]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c);
                    else
                        now->add(r,c,must_lose,now);
                }
                if(last_is_empty&&c-5>=0&&r+5<15&&chessboard[r+5][c-5]==Now)
                {
                    if(Now==AI)
                        return QPoint(r+4,c-4);
                    else
                        now->add(r+4,c-4,must_lose,now);
                }
                if(next_is_empty)
                    flag1=detail_case(r,c,r+1,c-1,Now);
                if(last_is_empty)
                    flag2=detail_case(r+4,c-4,r+3,c-3,Now);
                if(nextnext_is_empty&&flag1)
                {
                    if(Now==Player)
                        now->add(r,c,more_will_lose,now);
                    else
                        now->add(r,c,more_will_win,now);
                }
                if(lastlast_is_empty&&flag2)
                {
                    if(Now==Player)
                        now->add(r+4,c-4,more_will_lose,now);
                    else
                        now->add(r+4,c-4,more_will_win,now);
                }
                if(nextnext_is_empty&&last_is_empty)
                {
                    if(Now==Player)
                        now->add(r,c,will_lose,now);
                    else if(Now==AI)
                        now->add(r,c,will_win,now);
                }
                if(lastlast_is_empty&&next_is_empty)
                {
                    if(Now==Player)
                        now->add(r+4,c-4,will_lose,now);
                    else if(Now==AI)
                        now->add(r+4,c-4,will_win,now);
                }
                if(last_is_empty&&(lastlast_is_empty||next_is_empty))
                {
                    if(Now==Player)
                        now->add(r+4,c-4,willwill_lose,now);
                    else if(Now==AI)
                        now->add(r+4,c-4,more_will_win,now);
                }
                else if(next_is_empty&&(nextnext_is_empty||last_is_empty))
                {
                    if(Now==Player)
                        now->add(r,c,willwill_lose,now);
                    else if(Now==AI)
                        now->add(r,c,willwill_lose,now);
                }
                else
                {
                    if(next_is_empty)
                    {
                        if(Now==Player)
                            now->add(r,c,hard_lose,now);
                        else if(Now==AI)
                            now->add(r,c,hard_win,now);
                    }
                    else if(last_is_empty)
                    {
                        if(Now==Player)
                            now->add(r,c,hard_lose,now);
                        else if(Now==AI)
                            now->add(r,c,hard_win,now);
                    }
                }
            }
            if(countchess==2)
            {
                flag1=flag2=0;
                if(next_is_empty)
                    flag1=detail_case(r,c,r+1,c-1,Now);
                if(last_is_empty)
                    flag2=detail_case(r+3,c-3,r+2,c-2,Now);
            }
            if(countchess==2&&next_is_empty&&r-1>=0&&c+1<15&&chessboard[r-1][c+1]==Now)
            {
                if(r-2>=0&&c+2<15&&chessboard[r-2][c+2]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c);
                    else
                        now->add(r,c,must_lose,now);
                }
                else if(((r-2>=0&&c+2<15&&chessboard[r-2][c+2]==2)+(flag1==1)+(last_is_empty==true))>=2)
                {
                    if(Now==AI)
                        now->add(r,c,more_will_win,now);
                    else
                        now->add(r,c,more_will_lose,now);
                }
                else if(((r-2>=0&&c+2<15&&chessboard[r-2][c+2]==2)+(flag1==1)+(last_is_empty==true))>=1)
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                else
                {
                    if(Now==AI)
                        now->add(r,c,hard_win,now);
                    else
                        now->add(r,c,hard_lose,now);
                }

            }
            if(countchess==2&&last_is_empty&&r+4<15&&c-4>=0&&chessboard[r+4][c-4]==Now)
            {
                if(r+5<15&&c-5>=0&&chessboard[r+5][c-5]==Now)
                {
                    if(Now==AI)
                        return QPoint(r+3,c-3);
                    else
                        now->add(r+3,c-3,must_lose,now);
                }
                else if(((r+5<15&&c-5>=0&&chessboard[r+5][c-5]==2)+(flag2==1)+(next_is_empty==true))>=2)
                {
                    if(Now==AI)
                        now->add(r+3,c-3,more_will_win,now);
                    else
                        now->add(r+3,c-3,more_will_lose,now);
                }
                else if(((r+5<15&&c-5>=0&&chessboard[r+5][c-5]==2)+(flag2==1)+(next_is_empty==true))>=1)
                {
                    if(Now==AI)
                        now->add(r+3,c-3,willwill_win,now);
                    else
                        now->add(r+3,c-3,willwill_lose,now);
                }
                else
                {
                    if(Now==AI)
                        now->add(r+3,c-3,hard_win,now);
                    else
                        now->add(r+3,c-3,hard_lose,now);
                }
            }
            if(countchess==2&&last_is_empty&&next_is_empty)
            {
                if(flag1)
                {
                    if((nextnext_is_empty&&lastlast_is_empty)||(nextnextnext_is_empty))
                    {
                        if(Now==AI)
                            now->add(r,c,more_will_win,now);
                        else
                            now->add(r,c,more_will_lose,now);
                    }
                    else if(lastlast_is_empty||nextnext_is_empty)
                    {
                        if(Now==AI)
                            now->add(r,c,willwill_win,now);
                        else
                            now->add(r,c,willwill_win,now);
                    }
                    else
                    {
                        if(Now==AI)
                            now->add(r,c,may_win,now);
                        else
                            now->add(r,c,may_lose,now);
                    }
                }
                if(flag2)
                {
                    if((nextnext_is_empty&&lastlast_is_empty)||(lastlastlast_is_empty))
                    {
                        if(Now==AI)
                            now->add(r+3,c-3,more_will_win,now);
                        else
                            now->add(r+3,c-3,more_will_lose,now);
                    }
                    else if(lastlast_is_empty||nextnext_is_empty)
                    {
                        if(Now==AI)
                            now->add(r+3,c-3,willwill_win,now);
                        else
                            now->add(r+3,c-3,willwill_win,now);
                    }
                    else
                    {
                        if(Now==AI)
                            now->add(r+3,c-3,may_win,now);
                        else
                            now->add(r+3,c-3,may_lose,now);
                    }
                }
                if(nextnextnext_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                if(nextnext_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c,may_win,now);
                    else
                        now->add(r,c,may_lose,now);
                }
                if(lastlastlast_is_empty)
                {
                    if(Now==AI)
                        now->add(r+3,c-3,willwill_win,now);
                    else
                        now->add(r+3,c-3,willwill_lose,now);
                }
                if(lastlast_is_empty)
                {
                    if(Now==AI)
                        now->add(r+3,c-3,may_win,now);
                    else
                        now->add(r+3,c-3,may_lose,now);
                }
            }
            if(countchess==1)
            {
                if(next_is_empty&&last_is_empty&&c+1<15&&r-1>=0&&chessboard[r-1][c+1]==Now&&(lastlast_is_empty||(c+2<15&&r-2>=0&&chessboard[r-2][c+2]==2)))
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c-3>=0&&r+3<15&&chessboard[r+3][c-3]==Now&&(nextnext_is_empty||(c-4>=0&&r+4<15&&chessboard[r+4][c-4]==2)))
                {
                    if(Now==AI)
                        now->add(r+2,c-2,willwill_win,now);
                    else
                        now->add(r+2,c-2,willwill_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c+1<15&&r+1<15&&chessboard[r+1][c+1]==Now)
                {
                    if(Now==AI)
                        now->add(r,c,may_win,now);
                    else
                        now->add(r,c,may_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c-3>=0&&r+3<15&&chessboard[r+3][c-3]==Now)
                {
                    if(Now==AI)
                        now->add(r+2,c-2,may_win,now);
                    else
                        now->add(r+2,c-2,may_lose,now);
                }
            }
        }
    }

    for(cpc=0;cpc<11;cpc++)	//按斜上搜索 右下棋盘
    {
        for(c=cpc,r=14;r>=0;)
        {
            for(;c<15&&r>=0;c++,r--)
                if(chessboard[r][c]!=2)
                    break;
            Now=chessboard[r][c];
            if(c-1>=0&&r+1<15&&chessboard[r+1][c-1]==2)
                last_is_empty=true;
            else
                last_is_empty=false;
            if(c-2>=0&&r+2>=0&&chessboard[r+2][c-2]==2&&last_is_empty)
                lastlast_is_empty=true;
            else
                lastlast_is_empty=false;
            if(c-3>=0&&r+3>=0&&chessboard[r+3][c-3]==2&&lastlast_is_empty)
                lastlastlast_is_empty=true;
            else
                lastlastlast_is_empty=false;
            countchess=1;
            for(c=c+1,r=r-1;c<15&&r>=0;c++,r--)
            {
                if(chessboard[r][c]==Now)
                    countchess++;
                else
                    break;
            }
            if(c<15&&r>=0&&chessboard[r][c]==2)
                next_is_empty=true;
            else
                next_is_empty=false;
            if(c+1<15&&r-1>=0&&chessboard[r-1][c+1]==2&&next_is_empty)
                nextnext_is_empty=true;
            else
                nextnext_is_empty=false;
            if(c+2<15&&r-2>=0&&chessboard[r-2][c+2]==2&&nextnext_is_empty)
                nextnextnext_is_empty=true;
            else
                nextnextnext_is_empty=false;
            if(countchess==4)
            {
                if(Now==AI)
                {
                    if(next_is_empty)
                        return QPoint(r,c);
                    else if(last_is_empty)
                        return QPoint(r+5,c-5);
                }
                else if(Now==Player)
                {
                    if(next_is_empty)
                        now->add(r,c,must_lose,now);
                    else if(last_is_empty)
                        now->add(r+5,c-5,must_lose,now);
                }
            }
            if(countchess==3&&(last_is_empty||next_is_empty))
            {
                flag1=0,flag2=0;
                if(next_is_empty&&c+1<15&&r-1>=0&&chessboard[r-1][c+1]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c);
                    else
                        now->add(r,c,must_lose,now);
                }
                if(last_is_empty&&c-5>=0&&r+5<15&&chessboard[r+5][c-5]==Now)
                {
                    if(Now==AI)
                        return QPoint(r+4,c-4);
                    else
                        now->add(r+4,c-4,must_lose,now);
                }
                if(next_is_empty)
                    flag1=detail_case(r,c,r+1,c-1,Now);
                if(last_is_empty)
                    flag2=detail_case(r+4,c-4,r+3,c-3,Now);
                if(nextnext_is_empty&&flag1)
                {
                    if(Now==Player)
                        now->add(r,c,more_will_lose,now);
                    else
                        now->add(r,c,more_will_win,now);
                }
                if(lastlast_is_empty&&flag2)
                {
                    if(Now==Player)
                        now->add(r+4,c-4,more_will_lose,now);
                    else
                        now->add(r+4,c-4,more_will_win,now);
                }
                if(nextnext_is_empty&&last_is_empty)
                {
                    if(Now==Player)
                        now->add(r,c,will_lose,now);
                    else if(Now==AI)
                        now->add(r,c,will_win,now);
                }
                if(lastlast_is_empty&&next_is_empty)
                {
                    if(Now==Player)
                        now->add(r+4,c-4,will_lose,now);
                    else if(Now==AI)
                        now->add(r+4,c-4,will_win,now);
                }
                if(last_is_empty&&(lastlast_is_empty||next_is_empty))
                {
                    if(Now==Player)
                        now->add(r+4,c-4,willwill_lose,now);
                    else if(Now==AI)
                        now->add(r+4,c-4,more_will_win,now);
                }
                else if(next_is_empty&&(nextnext_is_empty||last_is_empty))
                {
                    if(Now==Player)
                        now->add(r,c,willwill_lose,now);
                    else if(Now==AI)
                        now->add(r,c,willwill_lose,now);
                }
                else
                {
                    if(next_is_empty)
                    {
                        if(Now==Player)
                            now->add(r,c,hard_lose,now);
                        else if(Now==AI)
                            now->add(r,c,hard_win,now);
                    }
                    else if(last_is_empty)
                    {
                        if(Now==Player)
                            now->add(r,c,hard_lose,now);
                        else if(Now==AI)
                            now->add(r,c,hard_win,now);
                    }
                }
            }
            if(countchess==2)
            {
                flag1=flag2=0;
                if(next_is_empty)
                    flag1=detail_case(r,c,r+1,c-1,Now);
                if(last_is_empty)
                    flag2=detail_case(r+3,c-3,r+2,c-2,Now);
            }
            if(countchess==2&&next_is_empty&&r-1>=0&&c+1<15&&chessboard[r-1][c+1]==Now)
            {
                if(r-2>=0&&c+2<15&&chessboard[r-2][c+2]==Now)
                {
                    if(Now==AI)
                        return QPoint(r,c);
                    else
                        now->add(r,c,must_lose,now);
                }
                else if(((r-2>=0&&c+2<15&&chessboard[r-2][c+2]==2)+(flag1==1)+(last_is_empty==true))>=2)
                {
                    if(Now==AI)
                        now->add(r,c,more_will_win,now);
                    else
                        now->add(r,c,more_will_lose,now);
                }
                else if(((r-2>=0&&c+2<15&&chessboard[r-2][c+2]==2)+(flag1==1)+(last_is_empty==true))>=1)
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                else
                {
                    if(Now==AI)
                        now->add(r,c,hard_win,now);
                    else
                        now->add(r,c,hard_lose,now);
                }
            }
            if(countchess==2&&last_is_empty&&r+4<15&&c-4>=0&&chessboard[r+4][c-4]==Now)
            {
                if(r+5<15&&c-5>=0&&chessboard[r+5][c-5]==Now)
                {
                    if(Now==AI)
                        return QPoint(r+3,c-3);
                    else
                        now->add(r+3,c-3,must_lose,now);
                }
                else if(((r+5<15&&c-5>=0&&chessboard[r+5][c-5]==2)+(flag2==1)+(next_is_empty==true))>=2)
                {
                    if(Now==AI)
                        now->add(r+3,c-3,more_will_win,now);
                    else
                        now->add(r+3,c-3,more_will_lose,now);
                }
                else if(((r+5<15&&c-5>=0&&chessboard[r+5][c-5]==2)+(flag2==1)+(next_is_empty==true))>=1)
                {
                    if(Now==AI)
                        now->add(r+3,c-3,willwill_win,now);
                    else
                        now->add(r+3,c-3,willwill_lose,now);
                }
                else
                {
                    if(Now==AI)
                        now->add(r+3,c-3,hard_win,now);
                    else
                        now->add(r+3,c-3,hard_lose,now);
                }
            }
            if(countchess==2&&last_is_empty&&next_is_empty)
            {
                if(flag1)
                {
                    if((nextnext_is_empty&&lastlast_is_empty)||(nextnextnext_is_empty))
                    {
                        if(Now==AI)
                            now->add(r,c,more_will_win,now);
                        else
                            now->add(r,c,more_will_lose,now);
                    }
                    else if(lastlast_is_empty||nextnext_is_empty)
                    {
                        if(Now==AI)
                            now->add(r,c,willwill_win,now);
                        else
                            now->add(r,c,willwill_win,now);
                    }
                    else
                    {
                        if(Now==AI)
                            now->add(r,c,may_win,now);
                        else
                            now->add(r,c,may_lose,now);
                    }
                }
                if(flag2)
                {
                    if((nextnext_is_empty&&lastlast_is_empty)||(lastlastlast_is_empty))
                    {
                        if(Now==AI)
                            now->add(r+3,c-3,more_will_win,now);
                        else
                            now->add(r+3,c-3,more_will_lose,now);
                    }
                    else if(lastlast_is_empty||nextnext_is_empty)
                    {
                        if(Now==AI)
                            now->add(r+3,c-3,willwill_win,now);
                        else
                            now->add(r+3,c-3,willwill_win,now);
                    }
                    else
                    {
                        if(Now==AI)
                            now->add(r+3,c-3,may_win,now);
                        else
                            now->add(r+3,c-3,may_lose,now);
                    }
                }
                if(nextnextnext_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                if(nextnext_is_empty)
                {
                    if(Now==AI)
                        now->add(r,c,may_win,now);
                    else
                        now->add(r,c,may_lose,now);
                }
                if(lastlastlast_is_empty)
                {
                    if(Now==AI)
                        now->add(r+3,c-3,willwill_win,now);
                    else
                        now->add(r+3,c-3,willwill_lose,now);
                }
                if(lastlast_is_empty)
                {
                    if(Now==AI)
                        now->add(r+3,c-3,may_win,now);
                    else
                        now->add(r+3,c-3,may_lose,now);
                }
            }
            if(countchess==1)
            {
                if(next_is_empty&&last_is_empty&&c+1<15&&r-1>=0&&chessboard[r-1][c+1]==Now&&(lastlast_is_empty||(c+2<15&&r-2>=0&&chessboard[r-2][c+2]==2)))
                {
                    if(Now==AI)
                        now->add(r,c,willwill_win,now);
                    else
                        now->add(r,c,willwill_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c-3>=0&&r+3<15&&chessboard[r+3][c-3]==Now&&(nextnext_is_empty||(c-4>=0&&r+4<15&&chessboard[r+4][c-4]==2)))
                {
                    if(Now==AI)
                        now->add(r+2,c-2,willwill_win,now);
                    else
                        now->add(r+2,c-2,willwill_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c+1<15&&r+1<15&&chessboard[r+1][c+1]==Now)
                {
                    if(Now==AI)
                        now->add(r,c,may_win,now);
                    else
                        now->add(r,c,may_lose,now);
                }
                if(next_is_empty&&last_is_empty&&c-3>=0&&r+3<15&&chessboard[r+3][c-3]==Now)
                {
                    if(Now==AI)
                        now->add(r+2,c-2,may_win,now);
                    else
                        now->add(r+2,c-2,may_lose,now);
                }
            }

        }
    }

    for(int i=0;i<15;i++) //搜索单个落子
        for(int j=0;j<15;j++)
        {
            if(chessboard[i][j]!=2)
            {
                if(i+1<15&&j+1<15&&chessboard[i+1][j+1]==2)
                   {
                        if(Now==AI)
                           now->add(i+1,j+1,hard_win,now);
                        else
                           now->add(i+1,j+1,hard_lose,now);
                    }
                else if(i+1<15&&j-1>15&&chessboard[i+1][j-1]==2)
                {
                    if(Now==AI)
                       now->add(i+1,j-1,hard_win,now);
                    else
                       now->add(i+1,j-1,hard_lose,now);
                }
                else if(i-1>0&&j+1<15&&chessboard[i-1][j+1]==2)
                {
                    if(Now==AI)
                       now->add(i-1,j+1,hard_win,now);
                    else
                       now->add(i-1,j+1,hard_lose,now);
                }
                else if(i-1>0&&j-1>0&&chessboard[i-1][j-1]==2)
                {
                    if(Now==AI)
                       now->add(i-1,j-1,hard_win,now);
                    else
                       now->add(i-1,j-1,hard_lose,now);
                }
            }
        }

    int levelmax=-3; //按照评分 选一个得分最大的落子
    QPoint *decide=NULL;
    node* temp;
    if(head->next==NULL)
    {
        return QPoint(1,1);
    }
    head=head->next;
    while(head!=now)
    {
        if(head->level>levelmax)
        {
            delete decide;
            decide=new QPoint(*head->p);
            levelmax=head->level;
        }
        temp=head;
        head=head->next;
        delete temp;
    }
    if(now->level>levelmax)
    {
        delete decide;
        decide=new QPoint(*now->p);
        levelmax=now->level;
        delete now;
    }

    return *decide;
}

void Widget::on_pushButton_clicked()   //悔棋函数
{
    if(break_flag)
        return ;
    delete signal_starttime;
    signal_starttime=new QTime(QTime::currentTime());
    ui->lcdNumber->display("00:00");
    if(modal) //双人模式
    {
        int temp=count-1;
        if(count==0)
        {
            return ;
        }
        if(temp%2==0&&count) //更改当前行动 并且删去上一个白棋
        {
            ui->text_nowchess_label->setText("白\n棋");
            ui->png_nowchess_label->setPixmap(QPixmap(":/image/imges/白棋(2).png"));
            QPoint save=whitepoint.pop();
            chessboard[save.x()][save.y()]=2;
            whitechess.pop()->close();
            count--;
        }
        else if(temp%2==1&&count) //更改当前行动 并且删去上一个黑棋
        {
            ui->text_nowchess_label->setText("黑\n棋");
            ui->png_nowchess_label->setPixmap(QPixmap(":/image/imges/黑棋.png"));
            QPoint save=blackpoint.pop();
            chessboard[save.x()][save.y()]=2;
            blackchess.pop()->close();
            count--;
        }
    }
    else //单人模式
    {
        if(count==0) //玩家尚未落子时不能悔棋
            return ;
        QPoint save=whitepoint.pop(); //玩家落子后 悔棋一次 则AI和玩家各删去一棋
        chessboard[save.x()][save.y()]=2;
        whitechess.pop()->close();
        save=blackpoint.pop();
        chessboard[save.x()][save.y()]=2;
        blackchess.pop()->close();
        count--;
    }
}

void Widget::receive_close_mainwidget() //接受关闭主界面的信号 并且调用close方法
{
    close();

}

void Widget::receive_clear_mainwidget() //接受清空主界面的信号 并调用clear方法
{
    clear_mainwidget();
}

void Widget::receive_set_modal(bool _modal) //接受设置modal的信号 设置modal 更改为对应设置并调用主界面
{
    modal=_modal;
    if(!modal)
    {
        ui->modal_VS_label->setText("玩家VS电脑");
        ui->modal_now_label->setText("玩家棋子颜色");
        if(!_player_first)
        {
            ui->png_nowchess_label->setPixmap(QPixmap(":/image/imges/黑棋.png"));
            ui->text_nowchess_label->setText("黑\n棋");
            qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
            int n_x=qrand()%7+4;
            int n_y=qrand()%7+4;
            White *e=new White(int(height()*boundary_percentage*1.1),this);
            QPoint p(height()*boundary_percentage+n_y*interval_percentage*height()-11,height()*boundary_percentage+n_x*interval_percentage*height()-11);
            e->move(p);
            e->show();
            chessboard[n_x][n_y]=1;
            QPoint q(n_x,n_y);
            whitepoint.push(q);
            whitechess.push(e);

        }
    }
    timer_start();
    show();
}

void Widget::receive_save_game(QString&s,QTime&t) //接受保存游戏信号 并调用save_game方法
{
    save_game(s,t);
}

void Widget::receive_open_game(QString &s)	//接受打开游戏信号
{
    open_game(s);
}

void Widget::on_pushButton_2_clicked()	//认输
{
    break_flag=true;
    if(count==0) //根据发出认输的人判断胜利者
        double_winner=true;
    else
        double_winner=(count-1)%2;
    End_Widget *end_widget=new End_Widget();
    connect(end_widget,SIGNAL(close_mainwidget()),this,SLOT(receive_close_mainwidget()));
    connect(end_widget,SIGNAL(clear_mainwidget()),this,SLOT(receive_clear_mainwidget()));
    connect(end_widget,SIGNAL(send_save_game(QString&,QTime&)),this,SLOT(receive_save_game(QString&,QTime&)));
    if(double_winner)
    {
        QString s="黑棋胜利！";
        end_widget->setText(s);
    }
    end_widget->show();
}

void Widget::on_pushButton_4_clicked() //退出游戏
{
    close();
}

void Widget::update_total_time() //接受计时器的timeout信号 更新totaltime时间
{
    if(break_flag)
        return ;
    int n=total_starttime->secsTo(QTime::currentTime());
    ui->totaltime_label->setText(second_to_HHMMSS(n));
}

void Widget::update_signal_time() //接受计时器的timeout信号 更新signaltime时间
{
    if(break_flag)
        return ;
    int n=signal_starttime->secsTo(QTime::currentTime());
    ui->lcdNumber->display(second_to_MMSS(n));
}

void Widget::on_pushButton_3_clicked() //调用登录对话框
{
    Log_Widget *log=new Log_Widget;
    close();
    log->show();
}

void Widget::on_pushButton_5_clicked() //调用保存对局的文件对话框
{
    onesecond_timer->stop();
    int delta_total=total_starttime->secsTo(QTime::currentTime());
    int delta_signal=signal_starttime->secsTo(QTime::currentTime());
    QTime *t=new QTime(QTime::currentTime());
    QString *ss=new QString((QFileDialog::getSaveFileName(this,"保存对局","D:/","二进制文件(*bin)")));
    save_game(*ss,*t);
    int now_total=QTime(0,0,0).secsTo(QTime::currentTime())+1;
    int now_signal=QTime(0,0,0).secsTo(QTime::currentTime())+1;
    int right_total=now_total-delta_total;
    int right_signal=now_signal-delta_signal;
    int h,m,s;
    s=right_total%60;
    right_total=right_total/60;
    m=right_total%60;
    right_total/=60;
    h=right_total;
    delete total_starttime;
    total_starttime=new QTime(h,m,s);
    s=m=h=0;
    s=right_signal%60;
    right_signal/=60;
    m=right_signal%60;
    right_signal/=60;
    h=right_signal;
    delete signal_starttime;
    signal_starttime=new QTime(h,m,s);
    onesecond_timer->start();
}
