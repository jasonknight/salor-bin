#include "salorjsapi.h"
#include "common_includes.h"
SalorJSApi::SalorJSApi(QObject *parent) : QObject(parent)
{
    SalorJSApi::drawer_thread = NULL;
    //this->credit_thread = new CuteCredit(0);
    //connect(this->credit_thread,SIGNAL(dataRead(QString)),this,SLOT(_cuteBubbleDataRead(QString)));
   // connect(this,SIGNAL(_cuteWriteData(QString)),this->credit_thread,SLOT(writeData(QString)));
    //this->credit_thread->start();
}
void SalorJSApi::playSound(QString name) {
    SalorProcess *sp = new SalorProcess(this);
    sp->run("aplay", QStringList() << "/usr/share/salor-bin/sounds/" + name + ".wav",3000);
     delete sp;
}


void SalorJSApi::printPage() {
    qDebug() << "SalorJSApi::printPage";
    QPrinter printer;
    printer.setPageSize(QPrinter::A4);
    printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
    printer.setColorMode(QPrinter::Color);
    QPrintDialog* dialog = new QPrintDialog(&printer, 0);
    if (dialog->exec() == QDialog::Accepted) {
        this->webView->page()->mainFrame()->print(&printer);
        if (printer.outputFileName().indexOf(".pdf") != -1) {
            chmod(printer.outputFileName().toLatin1().data(),0666);
        }
    }
    qDebug() << "SalorJSApi::printPage complete";
}

QStringList SalorJSApi::ls(QString path,QStringList filters) {
    QDir d(path);
    if (d.exists()) {
        d.setFilter(QDir::Files | QDir::Hidden | QDir::System);
        return d.entryList(filters);
    }
    return QStringList();
}

void SalorJSApi::poleDancer(QString path, QString message) {
    SalorProcess *sp = new SalorProcess(this);
    qDebug() << "calling poledancer " << path << " " << message;
    sp->run("poledancer",QStringList() << "-p" << path << message,2000);
    delete sp;
}

void SalorJSApi::mimoRefresh(QString path,int h, int w) {
    qDebug() << "calling mimoRefresh";
    SalorCustomerScreen *scs = new SalorCustomerScreen(this);
    scs->refresh(path,h,w);
}

void SalorJSApi::mimoImage(QString imagepath) {
    qDebug() << "Calling mimoImage";
    SalorProcess *sp = new SalorProcess(this);
    sp->run("poledancer",QStringList() << "-dlo" <<  imagepath,3000);
    delete sp;
}

void SalorJSApi::completeOrderSnap(QString order_id) {
    qDebug() << "calling complete order snap";
    QSize size(800,480);
    QImage image(size, QImage::Format_Mono); // mPage->viewportSize()

    QPainter painter;
    painter.begin(&image);
    this->webView->page()->mainFrame()->render(&painter);
    painter.end();
    QString name = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss") + "_" + order_id + ".png";
    image.save("/tmp/" + name, "png",1);
}
void SalorJSApi::generalSnap(QString msg) {
    qDebug() << "GeneralSnap called";
    QSize size(800,480);
    QImage image(size, QImage::Format_RGB16); // mPage->viewportSize()

    QPainter painter;
    painter.begin(&image);
    this->webView->page()->mainFrame()->render(&painter);
    painter.end();
    QString name = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss") + QString::number(qrand());
    QDir().mkpath("/tmp/salor-bin-images/");
    QFile m("/tmp/salor-bin-images/" + name + ".txt");
    if (m.open(QIODevice::ReadWrite)) {
        m.write(msg.toAscii());
        m.write(this->webView->page()->mainFrame()->toHtml().toAscii());
        m.close();
    } else {
        qDebug() << "Could not create file";
    }
    qDebug() << "writing: " << "/tmp/salor-bin-images/" + name << "png";
    image.save("/tmp/salor-bin-images/" + name + ".png", "png",1);
    qDebug() << "generalSnap complete";
}
QString SalorJSApi::toperScale(QString addy) {
  int fd;
  char * weight;
  fd = open_serial_port_for_scale(addy.toLatin1().data());
  request_weight_toperczer_f200_samsung_spain(fd);
  usleep(100000); // sleep 100ms until bytes are in the buffer. 50ms works too.
  weight = read_weight_toperczer_f200_samsung_spain(fd);
  close(fd);
  //qDebug() << "Reading from Toper: " << QString::number(weight);
  //return QString::number(weight);
  return weight;
}

void SalorJSApi::newOpenCashDrawer(QString addy) {
    int fd;
    int count;
    qDebug() << "Attempting to open CashDrawer at " << addy;
    fd = open_serial_port_for_drawer(addy.toLatin1().data());
    if (fd <= 0) {
        qDebug() << "CashDrawer failed to open!";
        return;
    }
    count = write(fd, "\x1B\x40", 2);
    qDebug() << "Wrote "  << count << " bytes to initialize printer.";
    usleep(5000); //50ms
    count = write(fd, "\x1D\x61\x01", 3);
    qDebug() << "Wrote "  << count << " bytes to enable printer feedback.";
    usleep(5000); //50ms
    count = write(fd, "\x1B\x70\x00\xFF\x01", 5);
    qDebug() << "Wrote "  << count << " bytes to open cash drawer.";
    close(fd);
}

void SalorJSApi::startDrawerObserver(QString addy) {
  if (SalorJSApi::drawer_thread == NULL) {
    qDebug() << "Called SalorJSApi::startDrawerObserver. Creating, remembering and starting new thread.";
    DrawerObserverThread * drawerobserver = new DrawerObserverThread(this);
    SalorJSApi::drawer_thread = drawerobserver;
    drawerobserver->addy = addy;
    connect(drawerobserver,SIGNAL(cashDrawerClosed()),this,SLOT(_cashDrawerClosed()));
    drawerobserver->start();
  } else {
    qDebug() << "Called SalorJSApi::startDrawerObserver. Apparently a thread is already running, so doing nothing.";
  }
}

void SalorJSApi::stopDrawerObserver() {
  if (SalorJSApi::drawer_thread) {
      qDebug() << "Called SalorJSApi::stopDrawerObserver(). Stopping thread and setting remembered thread address to NULL.";
      SalorJSApi::drawer_thread->stop_drawer_thread = true;
      SalorJSApi::drawer_thread = NULL;
  } else {
      qDebug() << "Called SalorJSApi::stopDrawerObserver(). Apparently no thread active, so doing nothing.";
  }
}

void SalorJSApi::_cashDrawerClosed() {
    qDebug() << "Drawer is closed. Calling JS complete_order_hide();.\n";
    //delete SalorJSApi::drawer_thread;
    this->webView->page()->mainFrame()->evaluateJavaScript("complete_order_hide();");
}

void SalorJSApi::shutdown() {
  int pid = getpid();
  qDebug() << "Shutdown called.";
  kill(pid,SIGKILL);
  QApplication::closeAllWindows();
}
void SalorJSApi::cuteWriteData(QString data) {
    qDebug() << "Writing Data";
    emit _cuteWriteData(data);
}
void SalorJSApi::_cuteBubbleDataRead(QString data) {
    emit cuteDataRead(data);
}
void SalorJSApi::remoteService(QString url, QString username, QString password, QString type) {
#ifdef LINUX
    pid_t cpid;
    QString cmd;
    if (type == "vnc") {
        cmd = "expect /usr/share/red-e_vnc_reverse_connect.expect ";
    } else {
        cmd = "expect /usr/share/red-e_ssh_reverse_connect.expect ";
    }
    cmd += url + " " + username + " " + password;
    qDebug() << "Executing command: " << cmd;
    cpid = fork();
    if (cpid==0) {
        system(cmd.toAscii().data());
        exit(0);
    }
#endif
}
bool SalorJSApi::remoteServiceConnectionOpen(QString type) {
    QString cmd;
    if (type == "vnc") {
        cmd = "netstat -pna | grep :28";
    } else {
        cmd = "netstat -pna | grep :26";
    }
    FILE* pipe = popen(cmd.toAscii().data(), "r");
    if (!pipe) {
        qDebug() << "Couldn't open pipe";
        return false;
    }
    char buffer[128];
    QString result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
                result += buffer;
    }
    pclose(pipe);
    qDebug() << "Result is: " << result;
    if (result.length() > 3) {
        return true;
    } else {
        qDebug() << "Nopers...";
    }
    return false;
}
