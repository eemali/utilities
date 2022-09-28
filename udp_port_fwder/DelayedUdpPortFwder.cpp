#include "DelayedUdpPortFwder.h"
void DelayedUdpPortFwder::delayed_packet_forwarding()
{
    QTime currentTime;
    struct packet_s packet;
    QQueue<packet_s> packetQ;
    while(!done){
        while(!pause){
            currentTime = QTime::currentTime();
            while((!packetQ.isEmpty()) && (currentTime > packetQ.head().time)){
                packet = packetQ.head();
                sendUdpSockt.writeDatagram(packet.datagram.data(), dst_ip, dst_port);
                packetQ.dequeue();
            }
            currentTime = QTime::currentTime();
            if(recvUdpSockt.hasPendingDatagrams()){
                packet.time = currentTime.addMSecs(msdelay);
                packet.datagram = recvUdpSockt.receiveDatagram();
                packetQ.enqueue(packet);
            }
        }
    }
    emit finished();
}
