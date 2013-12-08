#include "salorprinter.h"
#include "common_includes.h"
#include "serialport.h"
#ifdef WIN32
#include "winspool.h"
#endif

SalorPrinter::SalorPrinter(QObject *parent, QNetworkAccessManager *nm, QString printer, int baudrate, QString callback_js) :
    QObject(parent)
{
    auth_tried = false;
    m_printer = printer;
    m_manager = nm;
    m_serialport = new Serialport(printer, baudrate);
    m_callback_js = callback_js;
}

void SalorPrinter::printURL(QString url) {
    qDebug() << "Fetching: " << url << " and sending it to path " << m_printer;

    QNetworkRequest request = QNetworkRequest(QUrl(url));
    QNetworkReply *reply = m_manager->get(request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onError(QNetworkReply::NetworkError))
    );
    connect(reply, SIGNAL(finished()),
            this, SLOT(printDataReady())
    );
}

void SalorPrinter::onError(QNetworkReply::NetworkError error) {
    qDebug() << "SalorPrinter::onError(): Error during request";
}

void SalorPrinter::printDataReady() {
    qDebug() << "[SalorPrinter]" << "[printDataReady]" << "called";
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    int status = statusCode.toInt();
    QByteArray printdata;
    if (status != 200) {
        //printdata.append(reply->errorString());
        //printdata.append("\n\n\n\n");
        qDebug() << "SalorPrinter::printDataReady() ERROR" << QString::number(status) << reply->errorString();
    } else {
        printdata = reply->readAll();
    }
    print(printdata);

    qDebug() << "[SalorPrinter]" << "[printDataReady]" << "emitting signal 'printingDone' for evaluating callback JS" << m_callback_js;
    emit printingDone(m_callback_js);

    reply->deleteLater(); // good practice according to the Qt documentation of QNetworkAccessManager
    qDebug() << "[SalorPrinter]" << "[printDataReady] Ending.";
}

void SalorPrinter::print(QByteArray printdata) {

    if (m_printer == "") {
        return;
    }

    if (printdata.size() < 3) {
        qDebug() << "[SalorPrinter]" << "[print]" << "Not printing anything shorter than 3 characters." << m_printer;
        return;
    }

#ifdef LINUX
    qDebug() << "[SalorPrinter]" << "[print]" << "Printing to serial port" << m_printer;

    int fd;
    fd = m_serialport->open();

    if(fd < 0) {
        qDebug() << "[SalorPrinter]" << "[print]" << "Error: Serialport returned file descriptor" << fd;
        return;
    }

    m_serialport->write(printdata);
    m_serialport->close();
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
    } else {
        qDebug() << "file could not be written" << printer_name;
    }
#endif
#ifdef WIN32
    if (m_printer.indexOf("COM") != -1 ) {
        qDebug() << "[SalorPrinter]" << "[print]" << "Printing to serial port" << m_printer;
        m_serialport->open();
        m_serialport->write(printdata);
        m_serialport->close();

    } else {
        qDebug() << "[SalorPrinter]" << "[print]" << "Printing to system printer" << m_printer;

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
                    //qDebug() << "Page Ended";
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
            //qDebug() << " Bytes were written";
        }
    }
#endif
    printed();
}

void SalorPrinter::printed() {
    qDebug() << "[SalorPage]" << "[printed] Beginning. ";
    deleteLater();
    /*
    if(confirmation_url.length() > 0) {
        QNetworkAccessManager *confirmator = new QNetworkAccessManager(this);
        qDebug() << "Sending print confirmation to " << confirmation_url;
        confirmator->get(QNetworkRequest(QUrl(confirmation_url)));
    }
    */
}
