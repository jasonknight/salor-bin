#include "timer.h"
#include <iostream>

#include <QTime>


Timer::Timer(QObject *parent) :
    QThread(parent)
{
        this->ms = 0;

}
void Timer::go(long ms) {
    this->ms = ms;
    this->start();
}
void Timer::run() {
    if (!this->ms)
        return;
    while(true) {
        // You should calulate the sleep time here
        this->msleep(this->ms);
        emit timeout();
    }
}

