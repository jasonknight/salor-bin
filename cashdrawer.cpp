#include "cashdrawer.h"
#include "scales.h"
CashDrawer::CashDrawer(QObject *parent) :
    QThread(parent)
{
}
void CashDrawer::run() {
    printf("THREAD: Starting CashDrawer Thread.\n");
    char buf[20];
    char cash_drawer_closed[20] = "\x10\x00\x00\x0f";
    int cap = 1000;
    int x = 0;
    int fd;
    fd = open_serial_port(this->addy.toLatin1().data());
    read(fd, &buf, 19);
    strcpy(buf,"");
    close_fd(fd);
    while( strcmp(cash_drawer_closed,buf) != 0 ) {
        fd = open_serial_port(this->addy.toLatin1().data());
        read(fd, &buf[0], 19);
        close_fd(fd);
        usleep(500 * 1000);
        if (x == cap) {
            printf("\nTHREAD: CashDrawer Thread cap reached, exiting.\n");
            close_fd(fd);
            return;
        } else {
            x++;
        }
    }
    printf("THREAD: Emitting cashDrawerClosed().\n");
    emit cashDrawerClosed();
    return;
}