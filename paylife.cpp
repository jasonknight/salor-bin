#include "paylife.h"
#include <QDebug>
#include <QFile>
#include "scales.h"
PayLife::PayLife(QObject *parent) :
    QThread(parent)
{
}
void PayLife::run() {
  qDebug() << "In Thread, returning: Data: " << this->data << " and addy: " << this->addy;
    int fd;
    int count;
    int i;
	char buff[32];
     fd = open_serial_port(this->addy.toLatin1().data());
	if (fd <= 0) {
		qDebug() << "Could not open device!";
	}
	count = read(fd,&buff,32);
     write(fd,"\x06\x02",2);
     write(fd, this->data.toLatin1().data(),15);
     write(fd,"\x03",1);
	count = read(fd,&buff,32);
     close(fd);
}
