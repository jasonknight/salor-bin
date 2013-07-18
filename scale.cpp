#include "scale.h"
#include "common_includes.h"
#include "serialport.h"
#include <unistd.h>

Scale::Scale(QString path, int protocol, int baudrate)
{
    mPath = path;
    mProtocol = protocol;
    mSerialport = new Serialport(mPath, baudrate);
    qDebug() << "[Scale]" << "[initializer]" << "initialize: path" << mPath << ", protocol" << mProtocol << ", baudrate" << QString::number(baudrate);
}

QString Scale::read() {
    QString weight;
    int fd;

    fd = mSerialport->open();

    if(fd < 0) {
        qDebug() << "[Scale]" << "[read]" << "Error: Serialport returned file descriptor" << fd;
        return "-1";
    }

    switch (mProtocol) {
      case 0:
        weight = doSamsungSpain();
        mSerialport->close();
        return weight;
        break;
      default:
        qDebug() << "[Scale]" << "[read]" << "Error: Protocol number" << mProtocol << "is not implemented.";
        mSerialport->close();
        return "-2";
        break;
    }
}

// this is a 50 millisecond one-shot write-read action
QString Scale::doSamsungSpain() {
    int count;
    QByteArray promptCode;
    QByteArray weight;

    promptCode = "$";
    count = mSerialport->write(promptCode);
    if ( count < 0 ) {
        qDebug() << "[Scale]" << "[doSamsungSpain]" << "Error: Serialport returned" << count << "on write";
        return "-3";
    } else {
        qDebug() << "[Scale]" << "[doSamsungSpain]" << "Wrote" << count << "bytes to promt scale for submission.";
    }

    usleep(50000);
    mSerialport->setNonblock();
    weight = mSerialport->read();
    if ( weight == "Error") {
        qDebug() << "[Scale]" << "[doSamsungSpain]" << "Error: Serialport encountered an error during reading. See previous log messages for more details.";
        return "-4";
    } else {
        qDebug() << "[Scale]" << "[doSamsungSpain]" << "read" << weight.size() << "bytes from scale:" << weight.toHex();
        return QString(weight);
    }
}
