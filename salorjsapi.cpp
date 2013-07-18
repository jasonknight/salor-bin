#include "salorjsapi.h"
#include "common_includes.h"
#include "drawerobserver.h"
#include "customerscreen.h"
#include "salorprinter.h"
#include "scale.h"

SalorJsApi::SalorJsApi(QObject *parent, QNetworkAccessManager *nm) :
    QObject(parent)
{
    networkManager = nm;
    drawerObserver = NULL;
    drawerThread = NULL;
}

void SalorJsApi::echo(QString msg) {
    QDateTime  d = QDateTime::currentDateTime();
    qDebug() << "[echo at " << d << "] " << msg;
}

void SalorJsApi::playSound(QString name) {
#ifdef LINUX
    qDebug() << "[SalorJsApi]" << "[playSound] Beginning.";
    SalorProcess *sp = new SalorProcess(this);
    sp->run("aplay", QStringList() << "/usr/share/salor-bin/sounds/" + name + ".wav",3000);
    delete sp;
#endif
#ifdef WIN32
    QSound::play("sounds/" + name + ".wav");
#endif
    qDebug() << "[SalorJsApi]" << "[playSound] Ending.";
}

/* shows printer dialog */
void SalorJsApi::printPage() {
    qDebug() << "SalorJsApi::printPage";
    QPrinter printer;
    printer.setPageSize(QPrinter::A4);
    printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
    printer.setColorMode(QPrinter::Color);
    QPrintDialog* dialog = new QPrintDialog(&printer, 0);
    if (dialog->exec() == QDialog::Accepted) {
        webView->page()->mainFrame()->print(&printer);
        if (printer.outputFileName().indexOf(".pdf") != -1) {
            chmod(printer.outputFileName().toLatin1().data(),0775);
        }
    }
    delete dialog;
    qDebug() << "SalorJsApi::printPage complete";
}

void SalorJsApi::newOpenCashDrawer(QString addy, int baudrate) {
#ifdef LINUX
    QByteArray openCode;
    Serialport *serialport;
    int count;

    openCode = QByteArray("\x1B\x70\x30\x55\x55");

    serialport = new Serialport(addy, baudrate);
    serialport->open();
    count = serialport->write(openCode);
    serialport->close();
    serialport->deleteLater();

    qDebug() << "[SalorJsApi]" << "[newOpenCashDrawer]" << "Wrote" << count << "bytes to" << addy;
#endif
}

void SalorJsApi::startDrawerObserver(QString path, int baudrate) {
    qDebug() << "[SalorJsApi]" << "[startDrawerObserver] Beginning with argument" << path << ", baudrate" << baudrate;

    if (path.indexOf("usb") != -1 ) {
        // USB printers need the loop/thread method, since QSocketNotifer doesn't work on USB device nodes
        if ( drawerThread ) {
          qDebug() << "[SalorJsApi]" << "[startDrawerObserver] drawerThread already existing. Doing nothing.";
          return;
        }
        if ( drawerObserver ) {
          qDebug() << "[SalorJsApi]" << "[startDrawerObserver] drawerObserver already existing. Doing nothing.";
          return;
        }

        drawerObserver = new DrawerObserver(path, baudrate);
        drawerThread = new QThread(this);
        connect(drawerThread, SIGNAL(started()), drawerObserver, SLOT(startWithLoop()));
        connect(drawerThread, SIGNAL(finished()), this, SLOT(drawerThreadFinished()));
        drawerObserver->moveToThread(drawerThread);
        drawerThread->start(); // this also calls drawerObserver.start() due to the connect above
        drawerThread->quit(); // this will finish the thread AFTER the while loop of the start method has finished.

    } else {
        // real serial ports work with QNetworkNotifer which is a more elegant solution
        if ( drawerObserver ) {
          qDebug() << "[SalorJsApi]" << "[startDrawerObserver] drawerObserver already existing. Doing nothing.";
          return;
        }
        drawerObserver = new DrawerObserver(path, baudrate);
        connect(drawerObserver, SIGNAL(drawerCloseDetected()), this, SLOT(drawerCloseDetected()));
        drawerObserver->startWithNotifier();
    }
    qDebug() << "[SalorJsApi]" << "[startDrawerObserver]" << "Finished";
}

void SalorJsApi::stopDrawerObserver() {
    if ( drawerObserver ) {

        if (drawerObserver->mPath.indexOf("usb") != -1 ) {
            qDebug() << "[SalorJsApi]" << "[stopDrawerObserver]" << "Setting member variable doStop to true to break the thread loop.";
            drawerObserver->doStop = true;

        } else {
            qDebug() << "[SalorJsApi]" << "[stopDrawerObserver]" << "Stopping drawerObserver with stopWithNotifier()";
            drawerObserver->stopWithNotifier();
            drawerObserver->deleteLater();
            drawerObserver = NULL;
        }
    }
}


// this is called when drawerThread finishes
void SalorJsApi::drawerThreadFinished() {
    qDebug() << "[SalorJsApi]" << "[drawerThreadFinished]";
    if (drawerObserver->doStop == true) {
        qDebug() << "[SalorJsApi]" << "[drawerThreadFinished]" << "drawerObserver was interrupted manually. Not calling JS";
    }

    if (drawerObserver->drawerClosed == true) {
        qDebug() << "[SalorJsApi]" << "[drawerThreadFinished]" << "evaluating Javascript 'onCashDrawerClose()";
        webView->page()->mainFrame()->evaluateJavaScript("onCashDrawerClose();");
        //drawerObserver->drawerClosed = false;
    }

    drawerObserver->deleteLater();
    drawerThread->deleteLater();
    drawerObserver = NULL;
    drawerThread = NULL;
    return;
}

// this is called when QSocketNotifier signals incoming data
void SalorJsApi::drawerCloseDetected() {
    qDebug() << "[SalorJsApi]" << "[drawerCloseDetected]" << "evaluating Javascript 'onCashDrawerClose()";
    webView->page()->mainFrame()->evaluateJavaScript("onCashDrawerClose();");

    if ( drawerObserver ) {
      drawerObserver->deleteLater();
      drawerObserver = NULL;
    }
}

void SalorJsApi::printURL(QString printer, QString url, QString confirm_url, int baudrate) {
    SalorPrinter *salorprinter = new SalorPrinter(this, networkManager, printer, baudrate);
    salorprinter->printURL(url);
    // printer will delete itself later
    qDebug() << "[SalorJsApi]" << "[printURL] Ending.";
}

void SalorJsApi::printText(QString printer, QString text, int baudrate) {
    SalorPrinter *salorprinter = new SalorPrinter(this, networkManager, printer, baudrate);
    QByteArray bytes;
    bytes = QByteArray();
    bytes.append(text);
    salorprinter->print(bytes);
    // salorprinter will delete itself later
    qDebug() << "[SalorJsApi]" << "[printText] Ending.";
}

QStringList SalorJsApi::ls(QString path, QStringList filters) {
    QDir d(path);
    if (d.exists()) {
        d.setFilter(QDir::Files | QDir::Hidden | QDir::System);
        return d.entryList(filters);
    }
    return QStringList();
}

/* output plain text to Serial pole display */
void SalorJsApi::poleDancer(QString path, QString message, int baudrate) {
    SalorProcess *sp = new SalorProcess(this);
    qDebug() << "calling poledancer " << path << " " << message;
    sp->run("poledancer",QStringList() << "-p" << path << message, 2000);
    delete sp;
    qDebug() << "[SalorJsApi]" << "[poleDancer] Ending.";
}

/* output screenshot of url */
void SalorJsApi::mimoRefresh(QString path,int h, int w) {
    qDebug() << "calling mimoRefresh";
    CustomerScreen *cs = new CustomerScreen(this);
    cs->refresh(path, h, w);
    qDebug() << "[SalorJsApi]" << "[mimoRefresh] Ending.";
}

/* output image file */
void SalorJsApi::mimoImage(QString imagepath) {
    qDebug() << "Calling mimoImage";
    SalorProcess *sp = new SalorProcess(this);
    sp->run("poledancer",QStringList() << "-dlo" <<  imagepath, 3000);
    delete sp;
    qDebug() << "[SalorJsApi]" << "[mimoImage] Ending.";
}

QString SalorJsApi::weigh(QString addy, int protocol, int baudrate) {
    qDebug() << "[SalorJsApi]" << "[weigh]" << "Called with path" << addy << ", protocol" << protocol << ", baudrate" << baudrate;
    QString weight;
    Scale *sc = new Scale(addy, protocol, baudrate);
    weight = sc->read();
    return weight;
}

void SalorJsApi::cuteWriteData(QString data) {
    qDebug() << "Writing Data";
    emit _cuteWriteData(data);
}

void SalorJsApi::_cuteBubbleDataRead(QString data) {
    emit cuteDataRead(data);
}

void SalorJsApi::shutdown() {
    int pid = getpid();
    qDebug() << "Shutdown called.";
    QApplication::closeAllWindows();
#ifdef LINUX
    qDebug() << "[SalorJsApi]" << "[shutdown] Calling kill for pid.";
    kill(pid,SIGKILL); // to make really sure
#endif
}
