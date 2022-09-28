#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTime>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QQueue>
#include <QFuture>

struct packet_s
{
    double time;
    QNetworkDatagram datagram;
};
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

signals:
    void start_forwarding();

private slots:
    void on_PlayPushButton_clicked();
    void on_StopPushButton_clicked();
    void on_PausePushButton_toggled(bool checked);
    void on_ClosePushButton_clicked();
    bool packet_forwarding();

    void on_DelaySpinBox_valueChanged(int arg1);

private:
    Ui::Widget *ui;    
    QUdpSocket recvUdpSockt;
    QUdpSocket sendUdpSockt;
    QHostAddress src_ip;
    quint16 src_port;
    QHostAddress dst_ip;
    quint16 dst_port;
    int msdelay;
    bool done;
    bool pause;
    int nInPckts;
    int nOutPckts;
    QFuture<bool> future;
};

#endif // WIDGET_H
