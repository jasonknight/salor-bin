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
DrawerObserverThread::DrawerObserverThread(QObject *parent) : QThread(parent)
{
}
void DrawerObserverThread::run() {
    this->running = true;
    int fd;
    int count = 0;
    int i = 0;
    int j = 0;
    char buf[20];
    char cash_drawer_closed[5] = "\x14\x00\x00\x0f";
    qDebug() << "CashDrawerClosingThread called" << addy;
    stop_drawer_thread = false;
    while (i < 5 && count <= 0 && !stop_drawer_thread) {
      i += 1;
      fd = open_serial_port_for_scale(addy.toLatin1().data());
      qDebug() << QString("Opened file descriptor 0x%1. Reading...").arg(uchar(fd),0,16);
      count = read(fd, buf, 7);
      qDebug() << "Read numer of bytes: " << count;
      for (j=0;j<count;j++) { printf("%X|",*(buf+j)); } // debug

      qDebug() << "i = " << i;
      close(fd);
      sleep(1);
    }
    cashDrawerClosed();
    return;
}
