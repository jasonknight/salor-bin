#include "salorprinter.h"
#include <QDebug>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
SalorPrinter::SalorPrinter(QObject *parent) :
    QObject(parent)
{
    this->m_manager = new QNetworkAccessManager(this);
    this->connect(this->m_manager,SIGNAL(finished(QNetworkReply*)),SLOT(pageFetched(QNetworkReply*)));
}
void SalorPrinter::printURL(QString path, QString url) {
    qDebug() << "Fetching: " << url << " and sending it to path " << path;
    this->m_printer_path = path;
    this->m_manager->get(QNetworkRequest(QUrl(url)));
}

void SalorPrinter::pageFetched(QNetworkReply *reply) {
    QByteArray ba = reply->readAll();
    qDebug() << "Buffer is: " << ba;
    if (this->m_printer_path.indexOf("tty") != -1) {
        int fd;
        struct termios options;
        char * port = this->m_printer_path.toAscii().data();

        fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

        if (fd == -1) {

          printf("Unable to open port %s", port);

        } else {

          tcgetattr(fd, &options); // Get the current options for the port...
          cfsetispeed(&options, B9600); // Set the baud rates
          cfsetospeed(&options, B9600);
          //printf("c_cflag = %X \n", options.c_cflag);
          //options.c_cflag &= ~CSIZE;
          //printf("c_cflag = %X \n", options.c_cflag);
          //options.c_cflag |= (CLOCAL | CREAD | CS7 | PARENB); // Enable the receiver and set local mode...  | CS7 | PARENB
          //printf("c_cflag = %X \n", options.c_cflag);
          tcsetattr(fd, TCSANOW, &options); // Set the new options for the port...
          //fcntl(fd, F_SETFL, 0); // seems to enable blocking mode
          int r = write(fd,ba.constData(),ba.size());
          close(fd);
          qDebug() << "Printed " << QString::number(r) << "bytes";
          qDebug() << "print completed.";
          emit printed();
        }
        return;
    }
    QFile f(this->m_printer_path);
    if (f.exists() && f.open(QIODevice::WriteOnly)) {
        QTextStream out(&f);
        out.setCodec("ISO 8859-1");
        out << ba;
        f.close();
        qDebug() << "print completed.";
        emit printed();

    } else {
        qDebug() << "Failed to open file";
        emit printerDoesNotExist();
    }
}
