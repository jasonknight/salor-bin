#ifndef CASHDRAWER_H
#define CASHDRAWER_H
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <QDebug>
#include <QThread>

class DrawerObserverThread : public QThread
{
    Q_OBJECT
public:
    explicit DrawerObserverThread(QObject *parent = 0);
    QString addy;
    bool  stop_drawer_thread;
    void run();
    bool running;
signals:
    void cashDrawerClosed();
public slots:

};

static int open_serial_port_for_drawer(char *port) {
  int fd;
  struct termios options;

  fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
      qDebug() << "Unable to open port for cash drawer" << QString(port);
  } else {
      qDebug() << "Opened port for cash drawer" << QString(port);
    tcgetattr(fd, &options); // Get the current options for the port...
    cfsetispeed(&options, B9600); // Set the baud rates
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD); // Enable the receiver and set local mode...
    tcsetattr(fd, TCSANOW, &options); // Set the new options for the port...
  }
  return(fd);
}

#endif // CASHDRAWER_H
