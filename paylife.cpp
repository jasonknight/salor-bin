#include "paylife.h"
#include <QDebug>
#include <QFile>
#include "scales.h"
#include "paylife_structs.h"
static char calclrc(char * data,int len) {
   char etx = 0x03;
   char lrc;
   char * bytes;
   int i;
   bytes = (char *) malloc(sizeof(char) * len + 132);
    sprintf(bytes,"%s%c",data,etx);
    while (i < len + 1) {
      lrc ^= bytes[i];
      i++;
    }
    return lrc;
}
static QString parse_data(char * data,int len) {
    QString ret;
    int i;
    char c;
    char msg[256];
    int inmsg = 0;
    int index;
    for (i = 0; i < len; i++) {
        c = data[i];
        if (c == 0x05) {
            ret += " ENQ ";
            inmsg = 1;
            index = 0;
        } else if (c == 0x06) {
            ret += " ACK ";
        } else if (c == 0x15) {
            ret += " NAK ";
        } else if (c == 0x03) {
            ret += " ETX ";
            inmsg = 0;
            char lrc[24];
            sprintf(lrc,"%x",data[i + 1]);
            ret += " LRC[";
            ret += lrc;
            ret += "] ";
            char lrcheck;
            lrcheck = calclrc(msg,index + 1);

            ret += " LRC2[";
            ret += lrcheck;
            ret += "] ";

        } else if (c == 0x00) {
            ret += " NULL ";
        } else if (c == 0x06) {
            ret += " ACK ";
        } else if (c > 0x20 && c < 0x7E) {
            char b[24];
            sprintf(b,"%c",c);
            ret += b;
        }
        if (inmsg == 1) {
            if (index > 132) {
                inmsg = 0;
            } else {
                msg[index] = c;
                index++;
            }
        }
    }
    return ret;
}
PayLife::PayLife(QObject *parent) :
    QThread(parent)
{
}
void PayLife::run() {
  qDebug() << "In Thread, returning: Data: " << this->data << " and addy: " << this->addy;
  this->sendingData = 0;
    int fd;
    int count;
    int i;
	char buff[132];
     this->descriptor = open_paylife(this->addy.toLatin1().data());
	if (this->descriptor <= 0) {
            qDebug() << "Could not open device!";
        }
  while (this->descriptor > 0) {
    if (this->sendingData == 1) {
        sleep(1);
        continue;
    }
    count = read(this->descriptor,&buff,132);
    for (i = 0; i < strlen(buff); i++) {
        if (buff[i] == 0x05) {
            emit dataRead("PayLife","ENQ");
        } else {
            printf("\nSTX\n");
            for (i = 0; i < strlen(buff); i++) {
                printf("[%x]",buff[i]);
            }
            printf("\nChars:\n");
            for (i = 0; i < strlen(buff); i++) {
                printf("[%c]",buff[i]);
            }
            printf("\nETX\n");
            //char * bytes = (char *) malloc(sizeof(char) * strlen(buff) + 8);
            //sprintf(bytes,"%X",buff);
            emit dataRead("PayLife",parse_data(buff,strlen(buff)));
            //free(bytes);
        }
    }
    //printf("\n");
    //char * x = (char *) malloc(sizeof(char) * strlen(buff) + 4);
   // sprintf(x,"%X",buff);
    //emit dataRead("PayLife",QString(x));
    //write(this->descriptor, "\x06",1);
    usleep(500000);
  }
     close(fd);
}
void PayLife::sendPayLifeData(QString data) {
    this->sendingData = 1;

    if (data == "ACK") {
        write(this->descriptor, "\x06",1);
        this->sendingData = 0;
        return;
    }
  int len = data.length();
  int i = 0;
  char lrc = 0;
  char stx = 0x02;
  char etx = 0x03;
  char ack = 0x06;
  char * bytes;
  int ret;
  char buffer[132];
  char inbuffer[132];

  bytes = (char *) malloc(sizeof(char) * len + 132);
  strcpy(bytes,data.toLatin1().data());
  qDebug() << "PayLife Sending data: " << data << " of len " << QString::number(len) <<
          " " << " Bytes is: " << bytes ;
   sprintf(bytes,"%s%c",bytes,etx);
  while (i < len + 1) {
    lrc ^= bytes[i];
    i++;
  }
  //lrc = calclrc(bytes,strlen(bytes));

  sprintf(buffer,"%c%s%c",stx,bytes,lrc);
  qDebug() << "\nStarting";
    printf("Writing: [%s]\n", buffer);
  write(this->descriptor,buffer,len + 4);
  qDebug() << "Done";
  free(bytes);
  this->sendingData = 0;
}
