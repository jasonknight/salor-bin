#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "common_includes.h"

class Serialport : public QObject
{
    Q_OBJECT

public:
    explicit Serialport(QString path, int baudrate = 9600);
    QString m_path;
#ifdef LINUX
    int m_fd;
    speed_t m_baudrate;
#endif
#ifdef WINDOWS
    HANDLE m_fd;
    char m_baudrate[64];
#endif
    int open();
    int close();
    int write(QByteArray bytes);
    void setNonblock();
    QByteArray read();
};

#endif // SERIALPORT_H
