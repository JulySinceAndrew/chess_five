#include<QApplication>
#include"widget.h"
#include"log_widget.h"

bool _player_first=true;

int main(int argc,char*argv[])
{
    QApplication app(argc,argv);
    Log_Widget log; //新建一个登录界面
    log.show(); //show这个登录界面
    return app.exec();  //进入QT的事件循环
}
