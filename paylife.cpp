#include "paylife.h"
#include <QDebug>
#include <QFile>
#include "scales.h"
#include "paylife_structs.h"
static char calclrc(char * data,int len) {
   char lrc;
   int i;
    while (i < len + 1) {
      lrc ^= data[i];
      i++;
    }
    return lrc;
}
QString PayLife::parse_data(char * data,int len) {
    qDebug() << "Beginning parse_data";
    QString ret;
    int i;
    char c;
    for (i = 0; i < len; i++) {
        c = data[i];
        if (c == 0x05) {
            ret += " ENQ ";
        } else if (c == 0x06) {
            ret += " ACK ";
        } else if (c == 0x02) {
            ret += " STX ";
        } else if (c == 0x15) {
            ret += " NAK ";
        } else if (c == 0x03) {
            ret += " ETX ";
        } else if (c == 0x00) {
            ret += " NULL ";
        } else if (c == 0x06) {
            ret += " ACK ";
        } else if (c > 0x19 && c < 0x7E) {
            ret += c;
        }
    }
    this->log(ret.toAscii());
    qDebug() << "End parse_data";
    qDebug() << ret;
    return ret;
}
PayLife::PayLife(QObject *parent) :
    QThread(parent)
{
}
void PayLife::run() {
    this->running = true;
    this->sendingData = 0;
    int fd;
    int count;
    int i;
    int times_without_msg = 0;
    this->descriptor = open_paylife(this->addy.toLatin1().data());
    sleep(1);
    if (this->descriptor <= 0) {
        emit dataRead("PayLife","N000Could Not Open Device!0");
    }
    this->running = true;
        qDebug() << "HERE  1";
    while (this->running) {
        if (this->sendingData == 1) {
            sleep(1);
            continue;
        }
        char buff[1024];
        strcpy(&buff[0],"");
        count = read(this->descriptor,&buff,1024);
        //write(this->descriptor, "\x06",1);
        printf("Count is: %d\n",count);
        if (buff[0] == 0x05) {
            emit dataRead("PayLife","ENQ");
            times_without_msg = 0;
        }
        if (count > 1) {
            emit dataRead("PayLife",parse_data(buff,strlen(buff)));
            times_without_msg = 0;
        } else {
            times_without_msg++;
        }
        if (times_without_msg > 5) {
            qDebug() << "Haven seen a msg in awhile, closing!";
            //this->running = false;
            sleep(1);
        } else {
            sleep(1);
        }
    }
    qDebug() << "Closing thread";
     close(this->descriptor);
     this->descriptor = 0;
     this->running = false;
     emit finished();
}
void PayLife::sendPayLifeData(QString data) {
    this->sendingData = 1;
    if (data.length() <= 3) {
        printf("Writing: %s", data.toLatin1().data());
        if (data == "ACK") {
            write(this->descriptor,"\x06",1);
        } else if (data == "FF") {
            write(this->descriptor,"\x0C",1);
        } else if (data == "CR") {
            write(this->descriptor,"\x0D",1);
        } else if (data == "NAK") {
            write(this->descriptor,"\x15",1);
        } else if (data == "LF") {
            write(this->descriptor,"\x0A",1);
        }
        this->sendingData = 0;
        return;
    }

    this->log(data);
  int len = data.length();
  int i = 0;
  char lrc = 0;
  char stx = 0x02;
  char etx = 0x03;
  char ack = 0x06;
  char * bytes;
  int ret;
  char buffer[1024];
  char inbuffer[1024];
    qDebug() << "HERE  4";
  bytes = (char *) malloc(sizeof(char) * 1024);
  strcpy(bytes,data.toLatin1().data());
  qDebug() << "HERE  5";
  qDebug() << "PayLife Sending data: " << data << " of len " << QString::number(len) <<
          " " << " Bytes is: " << bytes ;
  //qDebug() << "sprintf 5";
   sprintf(bytes,"%s%c",bytes,etx);
   qDebug() << "HERE  6";
  while (i < len + 1) {
    lrc ^= bytes[i];
    i++;
  }
  qDebug() << "HERE  7";
  //lrc = calclrc(bytes,strlen(bytes));
    //qDebug() << "sprintf 6";
  sprintf(buffer,"%c%s%c",stx,bytes,lrc);
  qDebug() << "HERE  8";
  qDebug() << "\nStarting";
    printf("Writing: [%s]\n", buffer);
    this->log(buffer);
    qDebug() << "HERE  9";
  write(this->descriptor,buffer,len + 3);
  qDebug() << "Done";
  free(bytes);
  this->sendingData = 0;
}
void PayLife::log(QString txt) {
    QFile file("/tmp/paylife.log");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        file.write(txt.toAscii());
        file.write("\n");
        file.close();
    } else {
        qDebug() << "Couldn't open file...";
    }
    qDebug() << txt;
}
bool PayLife::checkParity(char b) {
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
char PayLife::encodeParity(char b) {
    if (!checkParity(b)) {
        b |= 128;
    }
    return b;
}
