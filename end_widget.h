#ifndef END_WIDGET_H
#define END_WIDGET_H

#include <QWidget>
#include<QFileDialog>
#include<QString>
#include<QTime>

namespace Ui {
class End_Widget;
}

class End_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit End_Widget(QWidget *parent = 0,Qt::WindowFlags f=Qt::SplashScreen);
    ~End_Widget();
    void setText(QString& s);

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::End_Widget *ui;

signals:
    void close_mainwidget();
    void clear_mainwidget();
    void send_save_game(QString& s,QTime &t);
};

#endif // END_WIDGET_H
