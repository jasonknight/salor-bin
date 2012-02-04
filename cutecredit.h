#ifndef CUTECREDIT_H
#define CUTECREDIT_H

#include <QObject>
#include <QThread>
#include <stdio.h>

class CuteCredit : public QThread
{
    Q_OBJECT
public:
    explicit CuteCredit(QObject *parent = 0);
    void run();
    bool running;

signals:
    void dataRead(QString);

public slots:
    void writeData(QString);
    void stopRunning();

};

#endif // CUTECREDIT_H
