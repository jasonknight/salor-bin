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
    int count;
    int i = 0;
    int j = 0;
    int close_after_seconds = 30;
    char buf[20];
    char cash_drawer_closed[5] = "\x14\x00\x00\x0f";

    stop_drawer_thread = false;
    fd = open_serial_port_for_scale(addy.toLatin1().data());
    if (fd == -1) {
      qDebug() << "Could not open" << addy << ". Prolly another DrawerObserver running or File not existing. Not starting another DrawerObserver.";
      return;
    } else {
      qDebug() << QString("Opened file descriptor 0x%1 for cash drawer.").arg(uchar(fd),0,16);
    }
    count = read(fd, buf, 20); // clear serial buffer
    qDebug() << "Cleared" << count << "bytes from the serial buffer on the first run.";
    usleep(200000); // the printer will wait for a clear buffer to send any pending bytes...
    count = read(fd, buf, 20); // ... so clear serial buffer again
    qDebug() << "Cleared" << count << "bytes from the serial buffer on the second run.";
    count = 0;
    while (i < (5 * close_after_seconds) && count <= 0 && !stop_drawer_thread) {
      i += 1;
      count = read(fd, buf, 7);
      qDebug() << (5*close_after_seconds-i) <<"Read numer of bytes from drawer: " << count;
      //for (j=0;j<count;j++) { printf("%X|",*(buf+j)); } // debug
      usleep(200000);
    }
    close(fd);
    cashDrawerClosed();
    return;
}
