#ifndef SCALES_H
#define SCALES_H
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
static int open_serial_port(char *port) {

  int fd;
  struct termios options;

  fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

  if (fd == -1) {

    printf("Unable to open port %s", port);

  } else {

    tcgetattr(fd, &options); // Get the current options for the port...
    cfsetispeed(&options, B9600); // Set the baud rates
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD); // Enable the receiver and set local mode...
    tcsetattr(fd, TCSANOW, &options); // Set the new options for the port...
    //fcntl(fd, F_SETFL, 0); // seems to enable blocking mode
  }

  return(fd);
}


static int request_weight_toperczer_f200_samsung_spain(int fd) {
  // Scale: Toperczer F-200 (branded DIBAL), Protocol: Samsung Spain
  int count;
  count = write(fd, "$", 1);
  return(count);
}


static float read_weight_toperczer_f200_samsung_spain(int fd) {
  // Scale: Toperczer F-200 (branded DIBAL), Protocol: Samsung Spain
  int count, i;
  float weight;
  char buffer[10];
  count = read(fd, buffer, 8);
  //for (i=0;i<8;i++) { printf("%H ",*(buffer+i)); } // debug
  sscanf(buffer, "%f", &weight);
  return weight;
}
static void read_fd(int fd,char &buf, int bytes) {
    read(fd, &buf, bytes);
}
static void close_fd(int i) {
  close(i);
}
#endif
