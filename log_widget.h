#ifndef LOG_WIDGET_H
#define LOG_WIDGET_H

#include <QWidget>
#include<QFileDialog>
#include<QString>
#include<QMessageBox>

namespace Ui {
class Log_Widget;
}

class Log_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Log_Widget(QWidget *parent = 0,Qt::WindowFlags f=Qt::SplashScreen|Qt::WindowStaysOnTopHint);
    ~Log_Widget();

signals:
    void set_modal(bool modal);
    void send_open_file(QString &s);

public slots:
    void receive_is_rightfile(bool isrightfile);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::Log_Widget *ui;
};

#endif // LOG_WIDGET_H
