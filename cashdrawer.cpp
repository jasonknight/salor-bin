#include "cashdrawer.h"
#include "scales.h"
#include <QDebug>
#include <sys/stat.h>
#include "common_includes.h"
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
    char closed_code[5] = "\x14";
    char opened_code[5] = "\x10";
    bool drawer_was_open = false;

    qDebug() << "Called DrawerObserverThread::run()";

    stop_drawer_thread = false;
    fd = open_serial_port_for_drawer(addy.toLatin1().data());
    if (fd == -1) {
      qDebug() << "Could not open" << addy << ". Prolly another DrawerObserver running or File not existing. Not starting another DrawerObserver.";
      return;
    } else {
      qDebug() << QString("Opened file descriptor 0x%1 for cash drawer.").arg(uchar(fd),0,16);
    }
 
    //--------
    count = write(fd, "\x1B\x40", 2);
    qDebug() << "Wrote "  << count << " bytes to initialize printer.";
    usleep(6000); //60ms
    count = write(fd, "\x1D\x61\x01", 3);
    qDebug() << "Wrote "  << count << " bytes to enable printer feedback.";
    //--------
    
    //count = read(fd, buf, 20); // clear serial buffer
    //qDebug() << "Cleared" << count << "bytes from the serial buffer on the first run.";
    //usleep(200000); // the printer will wait for a clear buffer to send any pending bytes...
    //count = read(fd, buf, 20); // ... so clear serial buffer again
    //qDebug() << "Cleared" << count << "bytes from the serial buffer on the second run.";
    int j = 0;
    while (i < (2 * close_after_seconds) && !stop_drawer_thread) {
      i += 1;
      for (j = 0; j < 8; j++) {
        buf[j] = '\0';
      }
      count = read(fd, buf, 7);
      
      if (drawer_was_open) {
          qDebug() << (5*close_after_seconds-i) << "Waiting for Drawer close. Bytes from drawer: " << count;
      } else {
          qDebug() << (5*close_after_seconds-i) << "Waiting for Drawer open. Bytes from drawer: " << count;
      }
      
      for (j = 0; j < 8; j++) {
        printf("[%X]",buf[j]);
      }
      printf("\n");
      fflush(stdout);
      // What state are we in?
      if ( drawer_was_open == false ) {
        // drawer hasn't been opened, so let's loop
        // over the buf until we find the code
        for (j = 0; j < 7; j++) {
            if (buf[j] == 0x10) {
                // score, the drawer is open
                drawer_was_open = true;
                qDebug() << "Open Drawer detected.";
                break;
            }
        }
      } else if ( drawer_was_open == true ) {
        // the drawer was open, so we loop to find 0x14
        for (j = 0; j < 7; j++) {
            if (buf[j] == 0x14) {
                // score, the drawer is open
                stop_drawer_thread = true;
                qDebug() << "Closed Drawer detected. Halting thread.";
                break;
            }
        }
      }
      if (stop_drawer_thread) {
        break;
      }
      

      /* if (!drawer_was_open && strstr(&buf[0],&opened_code[0]) != NULL ) {
          drawer_was_open = true;
          qDebug() << "Open Drawer detected.";
      }
      if (drawer_was_open && strstr(&buf[0],&closed_code[0]) != NULL ) {
          stop_drawer_thread = true;
          qDebug() << "Closed Drawer detected. Halting thread.";
      } */
      

      //count = 0;
      //int j = 0;
      //for (j=0;j<count;j++) { printf("%X|",*(buf+j)); } // debug
      
      usleep(500000);
    }
    close(fd);
    cashDrawerClosed();
    return;
}
