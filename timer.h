#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QThread>
class Timer : public QThread
{
    Q_OBJECT
public:
    explicit Timer(QObject *parent = 0);
    long ms;
signals:
    void timeout();
public slots:
    void go(long ms);
protected:
    void run();

};

#endif // TIMER_H
