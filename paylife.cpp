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
     fd = open_serial_port(this->addy.toLatin1().data());
     write(fd, this->data.toLatin1().data(),32);
     close(fd);
}
