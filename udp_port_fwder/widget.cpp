#include "widget.h"
#include "ui_widget.h"
#include <QtConcurrent/QtConcurrent>
#include <sys/time.h>
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QIntValidator *portValidator = new QIntValidator(0, 65535, this);
    ui->SrcPortLineEdit->setValidator(portValidator);
    ui->DstPortLineEdit->setValidator(portValidator);
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegx("^" + ipRange
                   + "\\." + ipRange
                   + "\\." + ipRange
                   + "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegx, this);
    ui->SrcIPLineEdit->setValidator(ipValidator);
    ui->DstIPLineEdit->setValidator(ipValidator);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_PlayPushButton_clicked()
{
    src_ip   = QHostAddress(ui->SrcIPLineEdit->text());
    src_port = ui->SrcPortLineEdit->text().toInt();
    recvUdpSockt.bind(QHostAddress::AnyIPv4, src_port, QUdpSocket::BindFlag::ShareAddress);
    if (src_ip.isMulticast()){
        if (false == recvUdpSockt.joinMulticastGroup(src_ip)){
            QMessageBox msgBox(QMessageBox::Critical,"Socket Error","");
            msgBox.setText("Could not able to join source multicast group");
            msgBox.exec();
            return;
        }
    }
    dst_ip   = QHostAddress(ui->DstIPLineEdit->text());
    dst_port = ui->DstPortLineEdit->text().toInt();
#if 0
    sendUdpSockt.bind(QHostAddress::AnyIPv4, dst_port, QUdpSocket::BindFlag::ShareAddress);
    if (dst_ip.isMulticast()){
        if (false == sendUdpSockt.joinMulticastGroup(dst_ip)){
            QMessageBox msgBox(QMessageBox::Critical,"Socket Error","");
            msgBox.setText("Could not able to join destination multicast group");
            msgBox.exec();
            return;
        }
    }
#endif
    msdelay  = ui->DelaySpinBox->text().toInt();
    done = false;
    pause = false;
    nInPckts = 0;
    nOutPckts = 0;
    ui->NumInPcktLCDNumber->display(nInPckts);
    ui->NumOutPcktLCDNumber->display(nOutPckts);
    ui->SrcIPLineEdit->setEnabled(false);
    ui->SrcPortLineEdit->setEnabled(false);
    ui->DstIPLineEdit->setEnabled(false);
    ui->DstPortLineEdit->setEnabled(false);

    ui->PlayPushButton->setEnabled(false);
    ui->PausePushButton->setEnabled(true);
    ui->StopPushButton->setEnabled(true);

    future = QtConcurrent::run(this, &Widget::packet_forwarding);
}

void Widget::on_StopPushButton_clicked()
{
    ui->SrcIPLineEdit->setEnabled(true);
    ui->SrcPortLineEdit->setEnabled(true);
    ui->DstIPLineEdit->setEnabled(true);
    ui->DstPortLineEdit->setEnabled(true);

    ui->PlayPushButton->setEnabled(true);
    ui->PausePushButton->setEnabled(false);
    ui->StopPushButton->setEnabled(false);
    done = true;
    pause = true;
    bool rv = future.result();
    if (rv){
        recvUdpSockt.close();
    }
}

void Widget::on_PausePushButton_toggled(bool checked)
{
    if (checked){
        ui->PausePushButton->setText("Continue");
        pause = true;
    }
    else {
        ui->PausePushButton->setText("Pause");
        pause = false;
    }
}

void Widget::on_ClosePushButton_clicked()
{
    done = true;
    pause = true;
    bool rv = future.result();
    if (rv){
        recvUdpSockt.close();
        this->close();
    }
}

bool packet_priority(const packet_s &lhs, const packet_s &rhs)
{
    return lhs.time < rhs.time;
}

bool Widget::packet_forwarding()
{
    struct packet_s packet;
    QQueue<packet_s> packetQ;
    struct timeval tv;
    double now;
    while(!done){
        gettimeofday(&tv, NULL);
        now = tv.tv_sec + tv.tv_usec * 1e-6;
        while((!packetQ.isEmpty()) && (now > packetQ.head().time)){
            packet = packetQ.head();
            sendUdpSockt.writeDatagram(packet.datagram.data(), dst_ip, dst_port);
            packetQ.dequeue();
            nOutPckts = nOutPckts + 1;
            ui->NumOutPcktLCDNumber->display(nOutPckts);
            ui->NumPcktInQLCDNumber->display(packetQ.size());
        }
        if(recvUdpSockt.hasPendingDatagrams()){
            gettimeofday(&tv, NULL);
            packet.time = tv.tv_sec + tv.tv_usec * 1e-6 + msdelay * 1e-3;
            packet.datagram = recvUdpSockt.receiveDatagram();
            if(!pause){
                packetQ.enqueue(packet);
                nInPckts = nInPckts + 1;
                ui->NumInPcktLCDNumber->display(nInPckts);
                ui->NumPcktInQLCDNumber->display(packetQ.size());
            }
        }        
    }
    return true;
}

void Widget::on_DelaySpinBox_valueChanged(int val)
{
    if (msdelay != val){
        msdelay = val;
    }
}
