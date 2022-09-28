#ifndef DELAYED_UDPPORTFWDER_H
#define DELAYED_UDPPORTFWDER_H
#include <QTime>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QQueue>

struct packet_s
{
    QTime time;
    QNetworkDatagram datagram;
};

class DelayedUdpPortFwder : public QObject
{
   Q_OBJECT

public slots:
    void delayed_packet_forwarding();

signals:
    void finished();

public:
    QUdpSocket recvUdpSockt;
    QUdpSocket sendUdpSockt;
    quint16 src_port;
    QHostAddress dst_ip;
    quint16 dst_port;
    int msdelay;
    bool done;
    bool pause;
};
#endif // DELAYED_UDPPORTFWDER_H
