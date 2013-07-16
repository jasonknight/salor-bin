#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "common_includes.h"

class Serialport : public QObject
{
    Q_OBJECT

public:
    explicit Serialport(QString path);
    int m_fd;
    QString m_path;
    int open();
    int close();
    int write(QByteArray bytes);
    QByteArray read();
};

#endif // SERIALPORT_H
