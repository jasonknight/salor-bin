#include "salorjsapi.h"
SalorJSApi::SalorJSApi(QObject *parent) :
    QObject(parent)
{
}
void SalorJSApi::playSound(QString name) {
    SalorProcess *sp = new SalorProcess(this);
    sp->run("aplay", "/home/berserker/work/sounds/" + name + ".wav");
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
    sp->run("pole-dancer","-p "+ path + " \"" + message + "\"");
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
    printf("Creating CashDrawer Thread.\n");
    CashDrawer *cd = new CashDrawer(this);
    cd->addy = addy;
    printf("Connecting Signals.\n");
    //connect(cd,SIGNAL(cashDrawerClosed()),this,SLOT(_cashDrawerClosed()));
    connect(cd,SIGNAL(finished()),cd,SLOT(deleteLater()));
    cd->start();
    printf("Thread Started.\n");
}
void SalorJSApi::shutdown() {
  QApplication::closeAllWindows();
}
