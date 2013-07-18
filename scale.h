#ifndef SCALE_H
#define SCALE_H

#include "common_includes.h"
#include "serialport.h"

class Scale
{
public:
    Scale(const QString path, int protocol, int baudrate = 9600);
    QString read();


protected:
    QString mPath;
    int mProtocol;
    QString doSamsungSpain();
    Serialport *mSerialport;
};

#endif // SCALE_H
