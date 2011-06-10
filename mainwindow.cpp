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
    
    connectSlots();

}
QWebView* MainWindow::getWebView() {
    return this->webView;
}

void MainWindow::connectSlots() {

  connect(webView->page(),SIGNAL(linkClicked(QUrl)),this,SLOT(linkClicked(QUrl)));
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(repaintViews()));
  timer->start(25);
   // connect(webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this,SLOT(addJavascriptObjects()));
}
void MainWindow::addJavascriptObjects() {

    attach();
}
void MainWindow::linkClicked(QUrl url) {

    webView->load(url);
}
void MainWindow::repaintViews() {
  webView->page()->view()->update();
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

