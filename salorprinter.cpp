#include "salorprinter.h"
#include "common_includes.h"
#ifdef WIN32
#include "winspool.h"
#endif

SalorPrinter::SalorPrinter(QObject *parent) :
    QObject(parent)
{
}

void SalorPrinter::printURL(QString printer, QString url, QString confirm_url) {
    qDebug() << "Fetching: " << url << " and sending it to path " << printer;
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(printDataReady(QNetworkReply*))
            );
    m_printer_path = printer;
    m_manager->get(QNetworkRequest(QUrl(url)));
    confirmation_url = confirm_url;
}

void SalorPrinter::printDataReady(QNetworkReply *reply) {
    QByteArray printdata = reply->readAll();
    print(m_printer_path, printdata);
}

void SalorPrinter::print(QString printer, QByteArray printdata) {
    //QString buffer;
    //buffer = new QString(*printdata);
    m_printer_path = printer;
    qDebug() << "SalorPrinter::print(): Printer is" << printer << "Buffer is" << printdata;
#ifdef LINUX
    QFile f(m_printer_path);

    if (false && f.exists() && f.open(QIODevice::WriteOnly)) {
        qDebug() << "SalorPrinter::print(): Printing to everything that QFile supports.";
        QDataStream out(&f);
        out << printdata;
        f.close();
        printed();

    } else if (m_printer_path.indexOf("tty") != -1) {
        qDebug() << "SalorPrinter::print(): Printing to a serial port.";
        int fd;
        struct termios options;
        char * port = m_printer_path.toAscii().data();

        fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

        if (fd == -1) {
          qDebug() << "SalorPrinter::print(): Unable to open" << m_printer_path;
        } else {
          tcgetattr(fd, &options); // Get the current options for the port...
          cfsetispeed(&options, B9600); // Set the baud rates
          cfsetospeed(&options, B9600);
          tcsetattr(fd, TCSANOW, &options); // Set the new options for the port...
          int r = write(fd, printdata, printdata.size());
          close(fd);
          qDebug() << "SalorPrinter::print(): printed " << QString::number(r) << "bytes";
          printed();
        }
    } else {
        qDebug() << "SalorPrinter::print(): failed to open as either file or serial port" << m_printer_path;
        //emit printerDoesNotExist();
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
        out << printdata;
        f.close();
        SalorProcess * p = new SalorProcess(this);
        p->run("lp", QStringList() << "-d" << printer_name << file_path,100000);
        delete sp;
        printed();
    } else {
        qDebug() << "file could not be written" << printer_name;
    }
#endif
#ifdef WIN32
    BOOL     bStatus = FALSE;
    DOC_INFO_1 DocInfo;
    DWORD      dwJob = 0L;
    DWORD      dwBytesWritten = 0L;
    HANDLE     hPrinter;
    wchar_t * name = new wchar_t[this->m_printer_path.length()+1];
    this->m_printer_path.toWCharArray(name);
    name[this->m_printer_path.length() + 1] = 0;
    // TODO: LD ERROR
    bStatus = OpenPrinter(name,&hPrinter, NULL);

    if (bStatus) {
        DocInfo.pDocName = L"My Document";
        DocInfo.pOutputFile = NULL;
        DocInfo.pDatatype = L"RAW";
        dwJob = StartDocPrinter( hPrinter, 1, (LPBYTE)&DocInfo );
        if (dwJob > 0) {
            bStatus = StartPagePrinter(hPrinter);
            if (bStatus) {
                bStatus = WritePrinter(hPrinter, printdata.data(), printdata.length(),&dwBytesWritten);
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
    if (dwBytesWritten != printdata.length()) {
        qDebug() << "Wrong number of bytes";
    }
#endif
}

void SalorPrinter::printed() {
    if(confirmation_url.length() > 0) {
        QNetworkAccessManager *confirmator = new QNetworkAccessManager(this);
        qDebug() << "Sending print confirmation to " << this->confirmation_url;
        confirmator->get(QNetworkRequest(QUrl(this->confirmation_url)));
    }
}
