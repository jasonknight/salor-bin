#include "mainwindow.h"
#include "salor_settings.h"

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

    SalorPage* page = new SalorPage(this);
    SalorSettings* s = SalorSettings::getSelf();
    this->resize(
            QSize(
                  s->getValue("width").toInt(),
                  s->getValue("height").toInt()
                   )
            );

    webView = new QWebView();

    webView->setPage((QWebPage*)page);
    webView->resize(
               s->getValue("width").toInt(),
               s->getValue("height").toInt()
                );
    QWebSettings *defaultSettings = QWebSettings::globalSettings();
/*
    if (s->getValue("PluginsEnabled").toBool() == true) {
        defaultSettings->setAttribute(QWebSettings::PluginsEnabled, true);
    }
    if (s->getValue("OfflineStorageEnabled").toBool() == true) {
        defaultSettings->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled,true);
        defaultSettings->setOfflineStoragePath(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    }
*/
    webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    setCentralWidget(webView);
    webView->show();
    webView->installEventFilter(this);
    
    connectSlots();

    linkClicked(QUrl(
                SalorSettings::getSelf()->getValue("start").toString()
              )
         );
    qDebug() << "End of Init";
}
QWebView* MainWindow::getWebView() {
    return this->webView;
}

void MainWindow::connectSlots() {
    qDebug() << "Connecting Slots";

  connect(webView->page(),SIGNAL(linkClicked(QUrl)),this,SLOT(linkClicked(QUrl)));
  connect(webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this,SLOT(addJavascriptObjects()));
  qDebug() << "Slots Connected";
}
void MainWindow::addJavascriptObjects() {
    if (!this->shown) {
       this->shown = true; // i.e. this is ready
    }
    attach();
}
void MainWindow::linkClicked(QUrl url) {

    webView->load(url);
}
void MainWindow::repaintViews() {

    QSize s = webView->page()->view()->size();
    int y = s.height() / 2;
    int x = 0;
    QRect r(x,y,s.width(),y);
     webView->page()->view()->repaint(r);
}
void MainWindow::attach(){

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
