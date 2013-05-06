#include "salorjsapi.h"
#include "common_includes.h"
#include "drawerobserver.h"
#include "customerscreen.h"
#include "salorprinter.h"
#include "scale.h"

SalorJsApi::SalorJsApi(QObject *parent, QNetworkAccessManager *nm) :
    QObject(parent)
{
    //drawerThread = 0;
    networkManager = nm;
    //drawerObserver = 0;
    drawerObserver = new DrawerObserver();
    drawerThread = new QThread(this);
    connect(drawerThread, SIGNAL(started()),
            drawerObserver, SLOT(observe()));
    connect(drawerThread, SIGNAL(finished()),
            this, SLOT(cashDrawerThreadFinished()));
    drawerObserver->moveToThread(drawerThread);
}

void SalorJsApi::echo(QString msg) {
    QDateTime  d = QDateTime::currentDateTime();
    qDebug() << "[echo at " << d << "] " << msg;
}

void SalorJsApi::playSound(QString name) {
#ifdef LINUX
    SalorProcess *sp = new SalorProcess(this);
    sp->run("aplay", QStringList() << "/usr/share/salor-bin/sounds/" + name + ".wav",3000);
    delete sp;
#endif
#ifdef WIN32
    QSound::play("sounds/" + name + ".wav");
#endif
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
            chmod(printer.outputFileName().toLatin1().data(),0666);
        }
    }
    delete dialog;
    qDebug() << "SalorJsApi::printPage complete";
}

void SalorJsApi::newOpenCashDrawer(QString addy) {
#ifdef LINUX
    int count;    
    qDebug() << "Attempting to open CashDrawer at " << addy;
    FILE * fd = fopen(addy.toLatin1().data(),"w");
    if (fd <= 0) {
        qDebug() << "CashDrawer failed to open!";
        return;
    }
    count = fwrite("\x1B\x70\x00\x55\x55", sizeof(char), 6, fd);
    qDebug() << "Wrote "  << count << " bytes to printer.";
    fclose(fd);
#endif
}

void SalorJsApi::startDrawerObserver(QString path) {
  qDebug() << "SalorJSApi::startDrawerObserver on" << path;
  if (drawerThread->isRunning() == false) {
      qDebug() << "    drawerThread->start()";
      drawerObserver->mPath = path;
      drawerThread->start();
      drawerThread->quit(); // this will quit the thread AFTER observe() has finished
  } else {
      qDebug() << "    drawerThread is already running. Doing nothing.";
  }
}

void SalorJsApi::stopDrawerObserver() {
    qDebug() << "SalorJSApi::stopDrawerObserver";
    drawerObserver->doStop = true;
}

void SalorJsApi::cashDrawerThreadFinished() {
    qDebug() << "SalorJsApi::cashDrawerClosed()";
    if (drawerObserver->doStop == true) {
        qDebug() << "    drawerObserver was interrupted. Not calling JS.";
        drawerObserver->doStop = false;
    }
    if (drawerObserver->drawerClosed == true) {
        webView->page()->mainFrame()->evaluateJavaScript("onCashDrawerClose();");
        drawerObserver->drawerClosed = false;
    }

}

void SalorJsApi::printURL(QString printer, QString url, QString confirm_url) {
    SalorPrinter *salorprinter = new SalorPrinter(this, networkManager, printer);
    salorprinter->printURL(url);
    // printer will delete itself later
}

void SalorJsApi::printText(QString printer, QString text) {
    SalorPrinter *salorprinter = new SalorPrinter(this, networkManager, printer);
    QByteArray bytes;
    bytes = QByteArray();
    bytes.append(text);
    salorprinter->print(bytes);
    // salorprinter will delete itself later
}

QStringList SalorJsApi::ls(QString path,QStringList filters) {
    QDir d(path);
    if (d.exists()) {
        d.setFilter(QDir::Files | QDir::Hidden | QDir::System);
        return d.entryList(filters);
    }
    return QStringList();
}

/* output plain text to Serial pole display */
void SalorJsApi::poleDancer(QString path, QString message) {
    SalorProcess *sp = new SalorProcess(this);
    qDebug() << "calling poledancer " << path << " " << message;
    sp->run("poledancer",QStringList() << "-p" << path << message, 2000);
    delete sp;
}

/* output screenshot of url */
void SalorJsApi::mimoRefresh(QString path,int h, int w) {
    qDebug() << "calling mimoRefresh";
    CustomerScreen *cs = new CustomerScreen(this);
    cs->refresh(path, h, w);
}

/* output image file */
void SalorJsApi::mimoImage(QString imagepath) {
    qDebug() << "Calling mimoImage";
    SalorProcess *sp = new SalorProcess(this);
    sp->run("poledancer",QStringList() << "-dlo" <<  imagepath, 3000);
    delete sp;
}

QString SalorJsApi::weigh(QString addy, int protocol) {
  char *weight;
  Scale *sc = new Scale(addy, protocol);
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
    kill(pid,SIGKILL); // to make really sure
#endif
}
