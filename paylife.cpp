#include "paylife.h"
#include <QDebug>
#include <QFile>
#include "scales.h"
#include "paylife_structs.h"
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
          if (buff[0] != 0x05) {
              for (i = 0; i < strlen(buff); i++) {
                printf(" %x ", buff[i]);
              }
              printf("EOT\n\n");
          }
    write(this->descriptor, "\x06",1);
    sleep(4);
  }
     close(fd);
}
void PayLife::sendPayLifeData(QString data) {
    this->sendingData = 1;
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

  bytes = (char *) malloc(sizeof(char) * len + 8);
  strcpy(bytes,data.toLatin1().data());
  qDebug() << "PayLife Sending data: " << data << " of len " << QString::number(len) <<
          " " << " Bytes is: " << bytes ;
  sprintf(bytes,"%s%c",bytes,etx);
  while (i < len + 1) {
    lrc ^= bytes[i];
    i++;
  }
  printf("LRC: %c\n",lrc);
  sprintf(buffer,"%c%s%c%c",stx,bytes,lrc);
  for (i = 0; i < strlen(buffer); i++) {
    printf(" [%c] ",buffer[i]);
  }
  qDebug() << "\nStarting";
  read(this->descriptor,&inbuffer,1);
  while (inbuffer[0] != 0x05) {
    read(this->descriptor,&inbuffer,132);
  }
  printf("ENQ received\n");
  write(this->descriptor,buffer,len + 4);
  //usleep(100000);
  sleep(1);
  printf("Received: ");
  read(this->descriptor,&inbuffer,132);
  for (i = 0; i < strlen(inbuffer); i++) {
    printf(" %x ",inbuffer[i]);
  }
  printf("\n");
  qDebug() << "Done";
  free(bytes);
  this->sendingData = 0;
}
