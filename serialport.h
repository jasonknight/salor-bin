#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "common_includes.h"

class Serialport : public QObject
{
    Q_OBJECT

public:
    explicit Serialport(QString path, int baudrate = 9600);
    int m_fd;
    QString m_path;
    speed_t m_baudrate;
    int open();
    int close();
    int write(QByteArray bytes);
    void setNonblock();
    QByteArray read();
};

#endif // SERIALPORT_H
