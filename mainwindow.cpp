#include "mainwindow.h"
#include <QWebPage>
#include <QDesktopServices>
#include <QSysInfo>
#include "salor_page.h"
#include <QApplication>
#include "scales.h"
#include "cashdrawer.h"
#include "paylife.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
}

MainWindow::~MainWindow()
{

}

void MainWindow::init() {
    this->shown = false;
    this->scs = new SalorCustomerScreen(this);
    SalorPage* page = new SalorPage(this);
    webView = new QWebView();
    //webView->setPage((QWebPage*)page);
/*
    if (s->getValue("PluginsEnabled").toBool() == true) {
        defaultSettings->setAttribute(QWebSettings::PluginsEnabled, true);
    }
    if (s->getValue("OfflineStorageEnabled").toBool() == true) {
        defaultSettings->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled,true);
        defaultSettings->setOfflineStoragePath(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    }
*/

    setCentralWidget(webView);
    webView->show();
    webView->load(this->to_url);
    connectSlots();
}
QWebView* MainWindow::getWebView() {
    return this->webView;
}

void MainWindow::connectSlots() {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(repaintViews()));
    timer->start(500);
    connect(webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this,SLOT(addJavascriptObjects()));
    connect(webView->page(), SIGNAL(windowCloseRequested()), this,SLOT(windowCloseRequested()));
    this->installEventFilter(this);
}
void MainWindow::addJavascriptObjects() {
    if (!this->shown) {
       this->shown = true; // i.e. this is ready
    }
    attach();
}
void MainWindow::linkClicked(QUrl url) {


}
 void MainWindow::repaintViews() {
    webView->update();
}
void MainWindow::attach(){
    this->webView->page()->mainFrame()->addToJavaScriptWindowObject("CustomerScreen", this->scs);
    this->webView->page()->mainFrame()->addToJavaScriptWindowObject("Salor", this);
    repaintViews();
}



/*void MainWindow::changeEvent(QEvent *e){
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //xlation code
        break;
    default:
        break;
    }
}*/
void MainWindow::shutdown() {
  windowCloseRequested();
}
void MainWindow::windowCloseRequested() {
  qDebug() << "Called";
  QApplication::closeAllWindows();
}
/* bool MainWindow::eventFilter(QObject *, QEvent *e)
{
    switch (e->type()) {
      case QEvent::KeyRelease:
          {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
            QString key = "salorKeyRelease(";
            key = key + QString::number(keyEvent->key());
            key = key +  "); null";
            this->webView->page()->mainFrame()->evaluateJavaScript(key);
          }
          break;
      default:
        return false;
          break;
    }
    return false;
}
*/

// Cash Drawer functions

void MainWindow::newOpenCashDrawer(QString addy) {
  this->cashDrawerClosed(addy);
}
void MainWindow::cashDrawerClosed(QString addy) {
    printf("Creating CashDrawer Thread.\n");
    CashDrawer * cd = new CashDrawer(this);
    cd->addy = addy;
    printf("Connecting Signals.\n");
    connect(cd,SIGNAL(cashDrawerClosed()),this,SLOT(_cashDrawerClosed()));
    connect(cd,SIGNAL(finished()),cd,SLOT(deleteLater()));
    cd->start();
    printf("Thread Started.\n");
}
void MainWindow::payLifeSend(QString addy,QString data) {
    printf("Creating Paylife Thread.\n");
    PayLife * pl = new PayLife(0);
    pl->addy = addy;
    pl->data = data;
    printf("Connecting PayLife Signals.\n");
    connect(pl,SIGNAL(payLifeConfirmed()),this,SLOT(_cashDrawerClosed()));
    connect(pl,SIGNAL(finished()),pl,SLOT(deleteLater()));
    pl->start();
    printf("PayLife Thread Started.\n");
}
void MainWindow::_cashDrawerClosed() {
    printf("Exiting complete_order_hide();.\n");
    this->webView->page()->mainFrame()->evaluateJavaScript("complete_order_hide();");
}
// Scale functions, static functions are defined in scales.h

QString MainWindow::toperScale(QString addy) {
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

