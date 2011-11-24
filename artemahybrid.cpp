#include "artemahybrid.h"
#include <QBasicTimer>

static char calculateLRC(QString s) {
    QByteArray buffer = s.toAscii();
    char lrc = 0;
    for(int i = 0; i < buffer.size(); i++) {
       lrc ^= buffer[i];
    }
    lrc ^= 0x03;
    return lrc;
}
static checkLRC(char lrc1,char lrc2) {
    return (lrc1 == lrc2);
}
ArtemaHybrid::ArtemaHybrid(QObject *parent) :
    QThread(parent)
{
}
void ArtemaHybrid::run() {
    QBasicTimer Tpol;               // 5 seconds
    QBasicTimer T0;                 // 1 second timeout
    QBasicTimer T1;                 // 6 seconds, repeat timer for send
    QBasicTimer T2;                 // 6 seconds, repeat timer for receive
    QString buffer;

    char ENQ = 0x05;
    char STX = 0x02;
    char ETX = 0x03;
    char ACK = 0x06;
    char NAK = 0x15;
    char b;
    bool parity_flag = false;
    int w = -1;

    this->running = true;
    this->m_ready = false;
    this->m_data_to_send = NULL;
    this->m_terminal_state = "OFFLINE";

    while (this->running) {
        b = this->read();
        switch(this->m_state) {
            case 0:
                if (b & 0b01111111 == ENQ && this->checkParity(b)) {
                    this->newState(1,"ONLINE");
                } else if (!Tpol.isActive()) {
                    this->newState(0,"OFFLINE");
                    emit pollingTimeOut();
                }
                Tpol.start(5000,this);
                break;
           case 1:
                if (this->m_ready && this->m_data_to_send) {
                    w = 0;
                    this->newState(3,"ONLINE");
                } else if (this->m_ecr_ready) {
                    this->send(ACK);
                    this->newState(2,"ONLINE");
                } else {
                    this->m_ready = false;
                    this->newState(0,"OFFLINE");
                }
                break;
           case 2:
                if (!Tpol.isActive()) {
                    this->newState(0,"OFFLINE");
                    emit pollingTimeOut();
                } else if (b & 0b01111111 == ENQ && this->checkParity(b)) {
                    this->m_ready = true;
                    Tpol.start(5000,this);
                    this->m_state = 1;
                } else if (b & 0b01111111 == STX && this->checkParity(b)) {
                    parity_flag = true;
                    buffer = "";
                    this->newState(5,"ONLINE");
                }
                break;
            case 3:
                if (w < 3) {
                    this->send(STX);
                    this->send(this->m_data_to_send);
                    this->send(ETX);
                    this->send(calculateLRC(this->m_data_to_send));
                    T1.start(6000,this);
                    this->newState(4,"ONLINE");
                } else {
                    this->newState(0,"OFFLINE");
                }
                break;
             case 4:
                if ((b & 0b01111111 == ENQ || b & 0b01111111 == NAK) && this->checkParity(b)) {
                    w++;
                    this->newState(3,"ONLINE");
                } else if (b & 0b01111111 == ACK && this->checkParity(b)) {
                      this->newState(0,"ONLINE");
                      emit dataSent();
                      this->m_data_to_send = "";
                } else if (!T1.isActive()) {
                    emit sendTimeOut();
                    this->newState(0,"OFFLINE");
                }
                break;
             case 5:
                if (!T0.isActive()) {
                    this->newState(0,this->m_terminal_state);
                    emit receiveTimeOut();
                } else if (b & 0b01111111 == ETX && this->checkParity(b)) {
                    T0.start(1000,this);
                    this->newState(6,"ONLINE");
                } else if (b != NULL) {
                    if (!checkParity(b)) {
                        parity_flag = false;
                    }
                    buffer += b & 0b01111111;
                    T0.start(1000,this);
                }
                break;
            case 6:
                if (!T0.isActive()) {
                    this->newState(0,this->m_terminal_state);
                    emit receiveTimeOut();
                } else if (b != NULL) {
                    if (this->checkParity(b) && checkLRC(b & 0b01111111,calculateLRC(buffer)) && parity_flag) {
                        this->send(ACK);
                        this->newState(0,"ONLINE");
                        emit bytesRead(buffer);
                        buffer = "";
                    } else {
                        T2.start(6000,this);
                        this->newState(7, "ONLINE");
                    }
                }
                break;
             case 7:
                if (!T2.isActive()) {
                    this->newState(0,"OFFLINE");
                    emit repeatTimeOut();
                } else if (b & 0b01111111 == STX && this->checkParity(b)) {
                    T0.start(1000,this);
                    parity_flag = true;
                    buffer = "";
                    this->newState(5,"ONLINE");
                } else if (b & 0b01111111 == ENQ && this->checkParity(b)) {
                    Tpol.start(5000,this);
                    this->newState(1,"ONLINE");
                }
                break;
        }
    }
}
bool ArtemaHybrid::checkParity(char b) {
    int i;
    char maskedb;
    int j = 0;
    for (i = 0; i < 6; i++ ) {
        maskedb = b;
        maskedb &= 2 ^ i;
        if (maskedb == 2 ^ i) {
            j++;
         }
    }
    return (j % 2 == 0);
}
char ArtemaHybrid::encodeParity(char b) {
    if (!checkParity(b)) {
        b |= 128;
    }
    return b;
}
void ArtemaHybrid::send(char b) {

}
void ArtemaHybrid::send(QString buffer) {

}
