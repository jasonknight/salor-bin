#include "cashdrawer.h"
#include "scales.h"
#include <QDebug>
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
    qDebug() << "XXX: Writing open cash drawer too: " << addy;
    //printf("XXX Writing open drawer %s \n",addy.toLatin1().data());
    fd = open_serial_port(addy.toLatin1().data());
    if (fd <= 0) {
        qDebug() << "CashDrawer failed to open!";
    }
    write(fd, "\x1D\x61\xFF", 3);
    usleep(2000); //i.e. 20ms
    write(fd, "\x1B\x70\x00\x20\x00", 5);
    read(fd, &buf, 19);
    usleep(2000); //i.e. 20ms
    read(fd, &buf, 19);
    strcpy(buf,"");
    usleep(2000);
    read(fd, &buf, 19);
    strcpy(buf,"");
    read(fd, &buf, 19);
    for (i=0;i < count; i++) { printf("%X ", *(buf+i));}
    while (strcmp(cash_drawer_closed,buf) != 0) {
        if (this->running == false) {
            close_fd(fd);
            return;
        }
      usleep(20000); //i.e. 20m
      count = read(fd,&buf,19);
      printf("-- %i \n", count);
      for (i=0;i < count; i++) { printf("%X ", *(buf+i));}
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
