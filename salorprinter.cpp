#include "salorprinter.h"
#include "common_includes.h"
#ifdef WIN32
#include "winspool.h"
#endif

SalorPrinter::SalorPrinter(QObject *parent, QNetworkAccessManager *nm, QString printer) :
    QObject(parent)
{
    auth_tried = false;
    m_printer = printer;
    m_manager = nm;
}

void SalorPrinter::printURL(QString url) {
    qDebug() << "Fetching: " << url << " and sending it to path " << m_printer;

    QNetworkRequest request = QNetworkRequest(QUrl(url));
    QSslConfiguration c = request.sslConfiguration();
    c.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(c);

    connect(m_manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(on_authenticationRequired(QNetworkReply*,QAuthenticator*))
    );
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(printDataReady(QNetworkReply*))
            );
    m_manager->get(request);
}

void SalorPrinter::on_authenticationRequired(QNetworkReply *reply, QAuthenticator *auth) {
    qDebug() << "Authentication required";
    auth->setUser("blah");
    auth->setPassword("blah");
    if(auth_tried == true) {
      // problem with the authenticationRequired signal is that it will cache wrong auth username/password, which leads to an endless loop with the server is immediately asking again for authentication.
      //m_manager->deleteLater();
    }
    auth_tried = true;
}

void SalorPrinter::printDataReady(QNetworkReply *reply) {
    //qDebug() << "SalorPrinter::printDataReady";
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    int status = statusCode.toInt();
    QByteArray printdata;
    if (status != 200) {
        printdata.append(reply->errorString());
        printdata.append("\n\n\n\n");
      qDebug() << "ERROR" << QString::number(status) << reply->errorString();
    } else {
      printdata = reply->readAll();
    }
    print(printdata);
    reply->deleteLater();
}

void SalorPrinter::print(QByteArray printdata) {
    //QString buffer;
    //buffer = new QString(*printdata);
    qDebug() << "SalorPrinter::print(): Printer is" << m_printer << "Buffer is" << printdata;
#ifdef LINUX
    QFile f(m_printer);

    if (f.exists() && f.open(QIODevice::WriteOnly)) {
        qDebug() << "SalorPrinter::print(): Printing to everything that QFile supports.";
        QDataStream out(&f);
        out << printdata;
        f.close();
        printed();

    } else if (m_printer.indexOf("tty") != -1) {
        qDebug() << "SalorPrinter::print(): Printing to a serial port.";
        int fd;
        struct termios options;
        char * port = m_printer.toAscii().data();

        fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

        if (fd == -1) {
          qDebug() << "SalorPrinter::print(): Unable to open" << m_printer;
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
        qDebug() << "SalorPrinter::print(): failed to open as either file or serial port" << m_printer;
        //emit printerDoesNotExist();
    }
#endif
#ifdef MAC
    QString printer_name = m_printer;
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
    BOOL       bStatus = FALSE;
    DOC_INFO_1 DocInfo;
    DWORD      dwJob = 0L;
    DWORD      dwBytesWritten = 0L;
    HANDLE     hPrinter;
    wchar_t * name = new wchar_t[m_printer.length()+1];
    m_printer.toWCharArray(name);
    name[m_printer.length()] = 0;
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
                qDebug() << " Page Ended";
            } else {
                qDebug() << "could not start printer";
            }
            EndDocPrinter(hPrinter);
        } else {
            qDebug() << "Couldn't create job";
        }
        ClosePrinter(hPrinter);
    } else {
        DWORD dw = GetLastError();
        qDebug() << "Could not open printer";
        qDebug() << QString::number(dw);
        //display_last_error(dw);
    }
    if (dwBytesWritten != printdata.length()) {
        DWORD dw = GetLastError();
        qDebug() << "Wrong number of bytes";
        qDebug() << QString::number(dw);
        //display_last_error(dw);
    } else {
        qDebug() << " Bytes were written";
    }
#endif
}

void SalorPrinter::printed() {
    /*
    if(confirmation_url.length() > 0) {
        QNetworkAccessManager *confirmator = new QNetworkAccessManager(this);
        qDebug() << "Sending print confirmation to " << confirmation_url;
        confirmator->get(QNetworkRequest(QUrl(confirmation_url)));
    }
    */
}
