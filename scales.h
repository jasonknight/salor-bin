#ifndef SCALES_H
#define SCALES_H
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <QDebug>

static int open_serial_port_for_scale(char *port) {
  int fd;
  struct termios options;

  fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
      qDebug() << "Unable to open port for scale" << QString(port);
  } else {
      qDebug() << "Opened port for scale" << QString(port);
    tcgetattr(fd, &options); // Get the current options for the port...
    cfsetispeed(&options, B9600); // Set the baud rates
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD); // Enable the receiver and set local mode...
    tcsetattr(fd, TCSANOW, &options); // Set the new options for the port...
  }
  return(fd);
}

static int request_weight_toperczer_f200_samsung_spain(int fd) {
  int count;
  count = write(fd, "$", 1);
  return(count);
}

static char * read_weight_toperczer_f200_samsung_spain(int fd) {
  int count;
  char buffer[10];

  qDebug() << "Starting read from scale...";
  count = read(fd, buffer, 7);
  qDebug() << "Read from scale:" << QString(*buffer);
  //int i;
  //for (i=0;i<count;i++) { printf("%X|",*(buffer+i)); } // debug
  return buffer;
}
#endif
