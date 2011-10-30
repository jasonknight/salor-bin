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
    int fd;
    int count;
    int i;
	char buff[132];
     this->descriptor = open_serial_port(this->addy.toLatin1().data());
	if (this->descriptor <= 0) {
		qDebug() << "Could not open device!";
            } else {
                qDebug() << "Opened device: " << QString::number(this->descriptor) << "\n";
            }
  while (this->descriptor > 0) {
	  count = read(this->descriptor,&buff,132);
    if (count > 1) {
      qDebug() << "Received: " << QString(buff);
    }
    write(this->descriptor, "\x06",1);
    usleep(100000);
  }
     close(fd);
}
void PayLife::sendPayLifeData(QString data) {

  int len = data.length();
  int i = 0;
  char lrc = 0;
  char stx = 0x02;
  char etx = 0x03;
  char ack = 0x06;
  char * bytes;
  int ret;
  char buffer[132];

  bytes = (char *) malloc(sizeof(char) * len + 1);
  strcpy(bytes,data.toLatin1().data());
  qDebug() << "PayLife Sending data: " << data << " of len " << QString::number(len) <<
          " " << " Bytes is: " << bytes ;
  while (i < len - 1) {
    lrc ^= bytes[i];
    i++;
  }
  printf("LRC: %c\n",lrc);
  /* sprintf(buffer,"%c%c%s%c%c",ack,stx,bytes,etx,lrc);
  for (i = 0; i < strlen(buffer); i++) {
    printf("c: %X ", buffer[i]);
  }
  printf(" [[ %s ]] \n", buffer); */
  qDebug() << "Starting";
  write(this->descriptor,&ack,1);
  usleep(100000);
  write(this->descriptor,&stx,1);
  usleep(100000);
  ret = write(this->descriptor,bytes,len - 1);
  usleep(100000);
  write(this->descriptor,&etx,1);
  usleep(100000);
  write(this->descriptor,&lrc,1);
  qDebug() << "Done";
  free(bytes);
}
