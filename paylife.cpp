#include "paylife.h"
#include <QDebug>
#include <QFile>
PayLife::PayLife(QObject *parent) :
    QThread(parent)
{
}
void PayLife::run() {
  qDebug() << "In Thread, returning: Data: " << this->data << " and addy: " << this->addy;
  QFile machine(this->addy);
   if (machine.open(QIODevice::ReadWrite)) {
       QTextStream out(&machine);
       out << this->data;
   }
   machine.close();
}
