#include "salorprinter.h"
#include "common_includes.h"
#ifdef WIN32
#include "winspool.h"
#endif

SalorPrinter::SalorPrinter(QObject *parent) :
    QObject(parent)
{
    auth_tried = false;
}

void SalorPrinter::setPrinterNames() {
    remotePrinterNames.clear();
    localPrinterNames.clear();
    qDebug() << "SalorPrinter::setPrinterNames()";
    QStringList groups = (*settings).childGroups();
    // filter out all setting groups that are not printer definitions
    for (int i = 0; i < groups.size(); i++) {
        QString group = groups[i];
        if (group.indexOf("printer") != -1) {
            remotePrinterNames << group;
        }
    }
#ifdef LINUX
    QStringList filters;
    filters << "ttyS*" << "ttyUSB*" << "usb";
    QDir * devs = new QDir("/dev", "*", QDir::Name, QDir::System);
    (*devs).setNameFilters(filters);
    QStringList nodes;
    nodes = devs->entryList();
    foreach (QString node, nodes) {
        localPrinterNames << "/dev/" + node;
    }
#endif
#ifdef WIN32
    QList<QPrinterInfo> printer_list = QPrinterInfo::availablePrinters();
    for (int i = 0; i < printer_list.length(); i++) {
        QPrinterInfo info = printer_list.at(i);
        localPrinterNames << info.printerName();
     }
#endif
#ifdef MAC
    int i;
    cups_dest_t *dests, *dest;
    int num_dests = cupsGetDests(&dests);
    for (i = num_dests, dest = dests; i > 0; i--, dest++) {
        localPrinterNames << dest->name;
     }
#endif
}

void SalorPrinter::printURL(QString printer, QString url, QString confirm_url) {
    qDebug() << "Fetching: " << url << " and sending it to path " << printer;

    QNetworkRequest request = QNetworkRequest(QUrl(url));

    QSslConfiguration c = request.sslConfiguration();
    c.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(c);

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(on_authenticationRequired(QNetworkReply*,QAuthenticator*))
    );
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(printDataReady(QNetworkReply*))
            );

    m_printer_path = printer;
    m_manager->get(request);
    confirmation_url = confirm_url;
}

void SalorPrinter::on_authenticationRequired(QNetworkReply *reply, QAuthenticator *auth) {
    qDebug() << "Authentication required";
    auth->setUser("blah");
    auth->setPassword("blah");
    if(auth_tried == true) {
      // problem with the authenticationRequired signal is that it will cache wrong auth username/password, which leads to an endless loop with the server is immediately asking again for authentication. so, we kill it at the second attempt.
      delete m_manager;
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
    print(m_printer_path, printdata);
}

void SalorPrinter::print(QString printer, QByteArray printdata) {
    //QString buffer;
    //buffer = new QString(*printdata);
    m_printer_path = printer;
    qDebug() << "SalorPrinter::print(): Printer is" << printer << "Buffer is" << printdata;
#ifdef LINUX
    QFile f(m_printer_path);

    if (f.exists() && f.open(QIODevice::WriteOnly)) {
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
    QString printer_name = m_printer_path;
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
    wchar_t * name = new wchar_t[m_printer_path.length()+1];
    m_printer_path.toWCharArray(name);
    name[m_printer_path.length()] = 0;
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
    if(confirmation_url.length() > 0) {
        QNetworkAccessManager *confirmator = new QNetworkAccessManager(this);
        qDebug() << "Sending print confirmation to " << confirmation_url;
        confirmator->get(QNetworkRequest(QUrl(confirmation_url)));
    }
}
