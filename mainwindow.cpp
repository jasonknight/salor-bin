#include "mainwindow.h"
#include <QWebPage>
#include <QDesktopServices>
#include <QSysInfo>
#include "salor_page.h"
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
    webView->setPage((QWebPage*)page);
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
    webView->load(QUrl("http://salor"));
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
}



void MainWindow::changeEvent(QEvent *e){
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //xlation code
        break;
    default:
        break;
    }
}
bool MainWindow::eventFilter(QObject *, QEvent *e)
{
    switch (e->type()) {
    case QEvent::MouseButtonPress:
        if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton) {
            mousePressed = true;
        }
        repaintViews();
        break;
    case QEvent::ContentsRectChange:
        repaintViews();
        break;
    case QEvent::ToolTipChange:
        repaintViews();
        break;
    case QEvent::Wheel:
        repaintViews();
        break;
    case QEvent::MouseButtonRelease:
        if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton) {
            mousePressed = false;
        }
       repaintViews();
        break;
    case QEvent::MouseMove:
        if (mousePressed) {
            return false;
        }
        repaintViews();
        break;
    default:
        break;
    }
    return false;
}
