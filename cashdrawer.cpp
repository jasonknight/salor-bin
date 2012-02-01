#include "cashdrawer.h"
#include "scales.h"
#include <QDebug>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
CashDrawer::CashDrawer(QObject *parent) :
    QThread(parent)
{
}
void CashDrawer::run() {
    this->running = true;
    int fd;
    int count;
    int i;
    char buf[20];
    char cash_drawer_closed[5] = "\x14\x00\x00\x0f";
    int cap = 500;
    int x = 0;
    //qDebug() << "XXX: Writing open cash drawer too: " << addy;
    //printf("XXX Writing open drawer %s \n",addy.toLatin1().data());
    fd = open_serial_port(addy.toLatin1().data());
    if (fd <= 0) {
        qDebug() << "CashDrawer failed to open!";
        return;
    }

    write(fd, "\x1D\x61\xFF", 3);
    usleep(2000); //i.e. 20ms
    write(fd, "\x1B\x70\x00\xFF\x01", 5);
    close_fd(fd);
    emit finished();
    return;
}
