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
    fd_set f;
    FD_ZERO(&f);
    FD_SET(fd,&f);
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100;
    for (i=0;i < count; i++) { printf("%X ", *(buf+i));}
    while (strcmp(cash_drawer_closed,buf) != 0) {
        if (this->running == false) {
            close_fd(fd);
            return;
        }
      int i = select(fd + 1, &f,NULL,NULL,&tv);
      if (i>0) {
          if (FD_ISSET(fd,&f)) {
              count = read(fd,&buf,19);
          } else {
              count = 0;
          }
      } else {
        count = 0;
      }

      //printf("-- %i \n", count);
      //for (i=0;i < count; i++) { printf("%X ", *(buf+i));}
      if (x == cap) {
        printf("XXX cap reached \n");
        close_fd(fd);
        return;
      } else { 
        x++;
      }
    }
    close_fd(fd);
    emit cashDrawerClosed();
    return;
}
