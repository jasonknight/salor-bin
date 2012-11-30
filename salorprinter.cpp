#include "salorprinter.h"
#include <QDebug>
#include <QList>
#include <QPrinter>
#include <QPrinterInfo>
#include <QTextDocument>
#include "salorprocess.h"
#include "common_includes.h"
SalorPrinter::SalorPrinter(QObject *parent) :
    QObject(parent)
{
    this->m_manager = new QNetworkAccessManager(this);
    this->connect(this->m_manager,SIGNAL(finished(QNetworkReply*)),SLOT(pageFetched(QNetworkReply*)));
}

void SalorPrinter::printURL(QString path, QString url, QString confirm_url) {
    qDebug() << "Fetching: " << url << " and sending it to path " << path;
    this->m_printer_path = path;
    this->m_manager->get(QNetworkRequest(QUrl(url)));
    this->confirmation_url = confirm_url;
}

void SalorPrinter::pageFetched(QNetworkReply *reply) {
    QByteArray ba = reply->readAll();
    qDebug() << "Buffer is: " << QString(ba).toAscii();
#ifdef LINUX
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
        if(this->confirmation_url.length() > 0) {
          QNetworkAccessManager *confirmator = new QNetworkAccessManager(this);
          qDebug() << "Sending print confirmation to " << this->confirmation_url;
          confirmator->get(QNetworkRequest(QUrl(this->confirmation_url)));
        }

    } else {
        qDebug() << "Failed to open file";
        emit printerDoesNotExist();
    }
#endif
#ifdef MAC
    QString printer_name = this->m_printer_path;
    QString file_path = QDir::tempPath() + "/" + printer_name;
    qDebug() << "Path is: " << file_path;
    QFile f(file_path);
    if (f.open(QIODevice::WriteOnly)) {
        QTextStream out(&f);
        out.setCodec("ISO 8859-1");
        out << ba;
        f.close();
        SalorProcess * p = new SalorProcess(this);
        p->run("lp", QStringList() << "-d" << printer_name << file_path,100000);
        emit printed();
    } else {
        qDebug() << "file could not be written" << printer_name;
    }
#endif
#ifdef WINDOWS
    BOOL     bStatus = FALSE;
    DOC_INFO_1 DocInfo;
    DWORD      dwJob = 0L;
    DWORD      dwBytesWritten = 0L;
    HANDLE     hPrinter;
    wchar_t * name = new wchar_t[this->m_printer_path.length()+1];
    this->m_printer_path.toWCharArray(name);
    name[this->m_printer_path.length() + 1] = 0;
    bStatus = OpenPrinter(name,&hPrinter, NULL);

    if (bStatus) {
        DocInfo.pDocName = L"My Document";
        DocInfo.pOutputFile = NULL;
        DocInfo.pDatatype = L"RAW";
        dwJob = StartDocPrinter( hPrinter, 1, (LPBYTE)&DocInfo );
        if (dwJob > 0) {
            bStatus = StartPagePrinter(hPrinter);
            if (bStatus) {
                bStatus = WritePrinter(hPrinter,ba.data(),ba.length(),&dwBytesWritten);
                EndPagePrinter(hPrinter);
            } else {
                qDebug() << "could not start printer";
            }
            EndDocPrinter(hPrinter);
        } else {
            qDebug() << "Couldn't create job";
        }
        ClosePrinter(hPrinter);
    } else {
        qDebug() << "Could not open printer";
    }
    if (dwBytesWritten != ba.length()) {
        qDebug() << "Wrong number of bytes";
    }
#endif
}
