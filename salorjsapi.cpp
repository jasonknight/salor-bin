#include "salorjsapi.h"
SalorJSApi::SalorJSApi(QObject *parent) :
    QObject(parent)
{
   // this->credit_thread = new CuteCredit(this);
    //this->connect(this->credit_thread,SIGNAL(dataRead(QString)),SLOT(_cuteBubbleDataRead(QString)));
    //connect(this,SIGNAL(_cuteWriteData(QString)),this->credit_thread,SLOT(writeData(QString)));
    //this->credit_thread->run();
}
void SalorJSApi::playSound(QString name) {
    SalorProcess *sp = new SalorProcess(this);
    sp->run("aplay", QStringList() << "/usr/share/sounds/salor/" + name + ".wav");
}
void SalorJSApi::printPage() {
    QPrinter printer;
    printer.setPageSize(QPrinter::A4);
    printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
    printer.setColorMode(QPrinter::Color);
        QPrintDialog* dialog = new QPrintDialog(&printer, 0);
        if (dialog->exec() == QDialog::Accepted)
        {
         this->webView->page()->mainFrame()->print(&printer);
         if (printer.outputFileName().indexOf(".pdf") != -1) {
            chmod(printer.outputFileName().toLatin1().data(),0666);
         }
        }
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
    qDebug() << path << " " << message;
    sp->run("pole-dancer",QStringList() << "-p" << path << message);
}
void SalorJSApi::mimoRefresh(QString path,int h, int w) {
    SalorCustomerScreen *scs = new SalorCustomerScreen(this);
    scs->refresh(path,h,w);
}
void SalorJSApi::completeOrderSnap(QString order_id) {
    QSize size(800,480);
    QImage image(size, QImage::Format_Mono); // mPage->viewportSize()

    QPainter painter;
    painter.begin(&image);
    this->webView->page()->mainFrame()->render(&painter);
    painter.end();
    QString name = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss") + "_" + order_id + ".png";
    // Here is where we hook in.
    //qDebug() << "Saving to: " << mOutput;
    image.save("/opt/salor_pos/logs/images/" + name, "png",1);
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
    QDir().mkpath("/opt/salor_pos/logs/images/");
    QFile m("/opt/salor_pos/logs/images/" + name + ".txt");
    if (m.open(QIODevice::ReadWrite)) {
        m.write(msg.toAscii());
        m.write(this->webView->page()->mainFrame()->toHtml().toAscii());
        m.close();
    } else {
        qDebug() << "Could not create file";
    }
    qDebug() << "writing: " << "/opt/salor_pos/logs/images/" + name << "png";
    image.save("/opt/salor_pos/logs/images/" + name + ".png", "png",1);
}
QString SalorJSApi::toperScale(QString addy) {
  int fd, j, count;
  char * weight;
  fd = open_serial_port(addy.toLatin1().data());
  request_weight_toperczer_f200_samsung_spain(fd);
  usleep(100000); // sleep 100ms until bytes are in the buffer. 50ms works too.
  weight = read_weight_toperczer_f200_samsung_spain(fd);
  close_fd(fd);
  //qDebug() << "Reading from Toper: " << QString::number(weight);
  //return QString::number(weight);
  return weight;
}
void SalorJSApi::newOpenCashDrawer(QString addy) {
    int fd;
    int count;
    int i;
    char buf[20];
    char cash_drawer_closed[5] = "\x14\x00\x00\x0f";
    int cap = 500;
    int x = 0;
    //qDebug() << "XXX: Writing open cash drawer too: " << addy;
    //printf("XXX Writing open drawer %s \n",addy.toLatin1().data());
    fd = open_serial_port(addy.toLatin1().data());
    if (fd <= 0) {
        qDebug() << "CashDrawer failed to open!";
        return;
    }

    write(fd, "\x1D\x61\xFF", 3);
    usleep(2000); //i.e. 20ms
    write(fd, "\x1B\x70\x00\xFF\x01", 5);
    close_fd(fd);
}
void SalorJSApi::shutdown() {
  QApplication::closeAllWindows();
}
void SalorJSApi::cuteWriteData(QString data) {
    emit _cuteWriteData(data);
}
void SalorJSApi::_cuteBubbleDataRead(QString data) {
    emit cuteDataRead(data);
}
