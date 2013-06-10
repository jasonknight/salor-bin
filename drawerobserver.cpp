#include "drawerobserver.h"
#include "salorjsapi.h"
#include "common_includes.h"

DrawerObserver::DrawerObserver() :
    QObject()
{
    mPath = "";
    mFiledescriptor == -1;
    doStop = false;
    drawerClosed = false;
}

void DrawerObserver::openDevice() {
    qDebug() << "[DrawerObserver]" << "[openDevice] Beginning.";
#ifdef LINUX
     struct termios options;

     mFiledescriptor = open(mPath.toLatin1().data(), O_RDWR | O_NOCTTY | O_NDELAY);
     if (mFiledescriptor == -1) {
         qDebug() << "DrawerObserverThread::open(): Unable to open port for drawer" << mPath;
     } else {
         QString("Opened file descriptor 0x%1 for cash drawer.").arg(uchar(mFiledescriptor),0,16);
         tcgetattr(mFiledescriptor, &options); // Get the current options for the port...
         cfsetispeed(&options, B9600); // Set the baud rates
         cfsetospeed(&options, B9600);
         options.c_cflag |= (CLOCAL | CREAD); // Enable the receiver and set local mode...
         tcsetattr(mFiledescriptor, TCSANOW, &options); // Set the new options for the port...
     }
#endif
#ifdef WIN32
    // TODO: How to read from system printer???
#endif
     qDebug() << "[DrawerObserver]" << "[openDevice] Ending.";
}

void DrawerObserver::closeDevice() {
    qDebug() << "[DrawerObserver]" << "[closeDevice] Beginning.";
    close(mFiledescriptor);
}

void DrawerObserver::observe() {
    qDebug() << "[DrawerObserver]" << "[observe] Beginning.";
    doStop = false;
    drawerClosed = false;

    int count = 0;
    int i = 0;
    int close_after_seconds = 30;

    char buf[20];
    char closed_code[5] = "\x14";
    char opened_code[5] = "\x10";
    bool drawer_was_open = false;

    qDebug() << "Called DrawerObserverThread::run()";

    openDevice();
    if (mFiledescriptor == -1) return;

    count = write(mFiledescriptor, "\x1B\x40", 2);
    qDebug() << "Wrote "  << count << " bytes to initialize printer.";
    usleep(5000); // 50ms
    count = write(mFiledescriptor, "\x1D\x61\x01", 3);
    qDebug() << "Wrote "  << count << " bytes to enable printer feedback.";

    int j = 0;
    while (i < (2 * close_after_seconds) && !doStop) {
      i += 1;
      for (j = 0; j < 8; j++) {
        buf[j] = '\0'; // initialize
      }
      count = read(mFiledescriptor, buf, 7);

      if (drawer_was_open) {
          qDebug() << (5*close_after_seconds-i) << "Waiting for Drawer close. Bytes from drawer: " << count;
      } else {
          qDebug() << (5*close_after_seconds-i) << "Waiting for Drawer open. Bytes from drawer: " << count;
      }

      for (j = 0; j < 8; j++) {
        printf("[%X]",buf[j]); // debug output
      }
      printf("\n");
      fflush(stdout);

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
                drawerClosed = true;
                qDebug() << "Closed Drawer detected. Halting thread.";
                break;
            }
        }
      }
      if (doStop || drawerClosed) {
          break;
      }
      usleep(500000);
    }
    closeDevice();
    // variables doStop and drawerClosed will be inspected by the caller, so we leave them alone
    qDebug() << "[DrawerObserver]" << "[observe] Ending.";
    return;
}
