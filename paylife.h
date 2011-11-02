#ifndef PAYLIFE_H
#define PAYLIFE_H

#include <QThread>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <QWebView>
class PayLife : public QThread
{
    Q_OBJECT
public:
    explicit PayLife(QObject *parent = 0);
    QString addy;
    QString data;
    QString error;
    int sendingData;
    int descriptor;
    int errorNumber;
    QWebView * webView;
    bool running;
    void run();
    
signals:
    void payLifeConfirmed();
    void payLifeError();
    void dataRead(QString source, QString data);
public slots:
  void sendPayLifeData(QString data);
  void log(QString txt);
  QString parse_data(char * data,int len);
};

static int open_paylife(char *port) {

  int fd;
  struct termios options;

  fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

  if (fd == -1) {

    printf("Unable to open port %s", port);

  } else {

    tcgetattr(fd, &options); // Get the current options for the port...
    cfsetispeed(&options, B19200); // Set the baud rates
    cfsetospeed(&options, B19200);
    //printf("c_cflag = %X \n", options.c_cflag);
    options.c_cflag &= ~CSIZE;
    //printf("c_cflag = %X \n", options.c_cflag);
    options.c_cflag |= (CLOCAL | CREAD | CS7 | PARENB); // Enable the receiver and set local mode...  | CS7 | PARENB
    //printf("c_cflag = %X \n", options.c_cflag);
    tcsetattr(fd, TCSANOW, &options); // Set the new options for the port...
    //fcntl(fd, F_SETFL, 0); // seems to enable blocking mode
  }

  return(fd);
}
#endif // PAYLIFE_H
