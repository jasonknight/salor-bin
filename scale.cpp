#include "scale.h"
#include "common_includes.h"
#include "serialport.h"
#include <unistd.h>

Scale::Scale(QString path, int protocol)
{
    mPath = path;
    mProtocol = protocol;
    qDebug() << "[Scale]" << "[initializer]" << "initialize: path" << mPath << "protocol" << mProtocol;
}

QString Scale::read() {
    QString weight;
    int fd;

    mSerialport = new Serialport(mPath);
    fd = mSerialport->open();

    if(fd < 0) {
        qDebug() << "[Scale]" << "[read]" << "Cannot read because file descriptor not open.";
        return "-1";
    }

    switch (mProtocol) {
      case 0:
        weight = doSamsungSpain();
        break;
      default:
        qDebug() << "[Scale]" << "[read]" << "This protocol number is not implemented.";
        return "-1";
    }
    mSerialport->close();
    return weight;
}

// this is a 100 millisecond one-shot write-read action
QString Scale::doSamsungSpain() {
    int count;
    QByteArray promptCode;
    QByteArray weight;

    promptCode = "$";
    count = mSerialport->write(promptCode);
    qDebug() << "[Scale]" << "[doSamsungSpain]" << "wrote" << count << "bytes to promt scale for submission.";
    usleep(50000);
    weight = mSerialport->read();
    qDebug() << "[Scale]" << "[doSamsungSpain]" << "read" << weight.size() << "bytes from scale:" << weight.toHex();

    return QString(weight);
}
