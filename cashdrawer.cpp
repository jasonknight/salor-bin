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
    int close_after_seconds = 30;

    char buf[20];
    char closed_code[5] = "\x14\x00\x00\x0f";
    char opened_code[5] = "\x10\x00\x00\x0f";
    bool drawer_was_open = false;

    stop_drawer_thread = false;
    fd = open_serial_port_for_drawer(addy.toLatin1().data());
    if (fd == -1) {
      qDebug() << "Could not open" << addy << ". Prolly another DrawerObserver running or File not existing. Not starting another DrawerObserver.";
      return;
    } else {
      qDebug() << QString("Opened file descriptor 0x%1 for cash drawer.").arg(uchar(fd),0,16);
    }
    //count = read(fd, buf, 20); // clear serial buffer
    //qDebug() << "Cleared" << count << "bytes from the serial buffer on the first run.";
    //usleep(200000); // the printer will wait for a clear buffer to send any pending bytes...
    //count = read(fd, buf, 20); // ... so clear serial buffer again
    //qDebug() << "Cleared" << count << "bytes from the serial buffer on the second run.";

    while (i < (2 * close_after_seconds) && !stop_drawer_thread) {
      i += 1;
      count = read(fd, buf, 7);

      if (!drawer_was_open && strcmp(opened_code, buf) == 0 ) {
          drawer_was_open = true;
          qDebug() << "Open Drawer detected.";
      }
      if (drawer_was_open && strcmp(closed_code, buf) == 0) {
          stop_drawer_thread = true;
          qDebug() << "Closed Drawer detected. Halting thread.";
      }
      if (drawer_was_open) {
          qDebug() << (5*close_after_seconds-i) << "Waiting for Drawer close. Bytes from drawer: " << count;
      } else {
          qDebug() << (5*close_after_seconds-i) << "Waiting for Drawer open. Bytes from drawer: " << count;
      }

      //count = 0;
      //int j = 0;
      //for (j=0;j<count;j++) { printf("%X|",*(buf+j)); } // debug
      usleep(500000);
    }
    close(fd);
    cashDrawerClosed();
    return;
}
