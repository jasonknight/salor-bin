#include "mainwindow.h"
#include <QWebPage>
#include <QDesktopServices>
#include <QSysInfo>
#include "salor_page.h"
#include <QApplication>
#include <QShortcut>
#include <QPrinter>
#include <QPrintDialog>
#include <QNetworkDiskCache>
#include "scales.h"
#include "cashdrawer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "salorprinter.h"
#include "salorcookiejar.h"
#include "salorprocess.h"
#include "salorjsapi.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

}

MainWindow::~MainWindow()
{

}
void MainWindow::incZoom() {
  QSettings settings("JolieRouge", "Salor");
  qreal z = this->webView->page()->mainFrame()->zoomFactor();
  z = z + 0.05;
  this->webView->page()->mainFrame()->setZoomFactor(z);
  settings.setValue("zoomFactor",z);
}
void MainWindow::decZoom() {
  QSettings settings("JolieRouge", "Salor");
  qreal z = this->webView->page()->mainFrame()->zoomFactor();
  z = z - 0.05;
  this->webView->page()->mainFrame()->setZoomFactor(z);
  settings.setValue("zoomFactor",z);
}
void MainWindow::init() {
    this->shown = false;
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    QWebSettings::globalSettings()->setOfflineStoragePath(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PrintElementBackgrounds, true);

    webView = new QWebView();
    this->page = new SalorPage(this);
    webView->setPage(this->page);
    SalorCookieJar * jar = new SalorCookieJar(this);
    webView->page()->networkAccessManager()->setCookieJar(jar);
    this->sp = new SalorPrinter(this);
    this->js = new SalorJSApi(this);
    this->js->webView = this->webView;
    connect(page,SIGNAL(generalSnap(QString)),this->js,SLOT(generalSnap(QString)));
    //QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
    //QString location = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    //diskCache->setCacheDirectory(location);
    //webView->page()->networkAccessManager()->setCache(diskCache);
    setCentralWidget(webView);
    webView->show();
    webView->load(this->to_url);
    connectSlots();
}
QWebView* MainWindow::getWebView() {
    return this->webView;
}

void MainWindow::connectSlots() {
    //QTimer *timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(repaintViews()));
    //timer->start(899);
    connect(webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this,SLOT(addJavascriptObjects()));
    connect(webView->page(), SIGNAL(windowCloseRequested()), this,SLOT(windowCloseRequested()));
    connect(this->page->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this->page,SLOT(resetJsErrors()));
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+p"), this);
    connect( shortcut, SIGNAL(activated()), this->js, SLOT(printPage()));

    QShortcut *f2 = new QShortcut(QKeySequence(Qt::Key_F2), this);
    connect( f2, SIGNAL(activated()), this, SLOT(lastFiveOrders()));

    QShortcut *ctrlf2 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F2), this);
    connect( ctrlf2, SIGNAL(activated()), this, SLOT(customersIndex()));

    QShortcut *f3 = new QShortcut(QKeySequence(Qt::Key_F3), this);
    connect( f3, SIGNAL(activated()), this, SLOT(showSearch()));

    QShortcut *f4 = new QShortcut(QKeySequence(Qt::Key_F4), this);
    connect( f4, SIGNAL(activated()), this, SLOT(showCashDrop()));

    QShortcut *f5 = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect( f5, SIGNAL(activated()), this, SLOT(endDayReport()));

    QShortcut *ctrlins = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Insert), this);
    connect( ctrlins, SIGNAL(activated()), this, SLOT(editLastAddedItem()));

    QShortcut *endk = new QShortcut(QKeySequence(Qt::Key_End), this);
    connect( endk, SIGNAL(activated()), this, SLOT(completeOrder()));

    QShortcut *zoomin = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this);
    connect( zoomin, SIGNAL(activated()), this, SLOT(incZoom()));

    QShortcut *zoomout = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus), this);
    connect( zoomout, SIGNAL(activated()), this, SLOT(decZoom()));

    QSettings settings("JolieRouge", "Salor");
    qreal z = settings.value("zoomFactor").toReal();
    if (z) {
      this->webView->page()->mainFrame()->setZoomFactor(z);
    }
}
void MainWindow::customersIndex() {
    this->webView->page()->mainFrame()->evaluateJavaScript("window.location = '/customers';");
}
void MainWindow::endDayReport() {
    this->webView->page()->mainFrame()->evaluateJavaScript("window.location = '/orders/report_day';");
}
void MainWindow::editLastAddedItem() {
    this->webView->page()->mainFrame()->evaluateJavaScript("editLastAddedItem();");
}
void MainWindow::lastFiveOrders() {
    this->webView->page()->mainFrame()->evaluateJavaScript("onF2Key();");
}
void MainWindow::completeOrder() {
    this->webView->page()->mainFrame()->evaluateJavaScript("onEndKey();");
}
void MainWindow::showSearch() {
    this->webView->page()->mainFrame()->evaluateJavaScript("showSearch();");
}
void MainWindow::showCashDrop() {
    this->webView->page()->mainFrame()->evaluateJavaScript("show_cash_drop();");
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
    this->webView->page()->mainFrame()->addToJavaScriptWindowObject("SalorPrinter", this->sp);
    this->webView->page()->mainFrame()->addToJavaScriptWindowObject("Salor", this->js);
    this->page->resetJsErrors();
}

void MainWindow::windowCloseRequested() {
  QApplication::closeAllWindows();
}
