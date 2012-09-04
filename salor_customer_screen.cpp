#include "salor_customer_screen.h"
#include <QImage>
#include <QtWebKit>
#include <QUrl>
#include <QWebView>
#include "salor_page.h"
SalorCustomerScreen::SalorCustomerScreen(QObject *parent) :
    QObject(parent)
{
}
void SalorCustomerScreen::refresh(QString url,int w, int h) {
    // The screenshot taking is done in salor_page.cpp, scroll down and look at SalorCapture class
    qDebug() << "CustomerScreen.refresh called with " << url;
    QWebView * webView = new QWebView();
   // qDebug() << "Setting up SalorPage";
    SalorPage * page = new SalorPage();
    //qDebug() << "Setting viewport size";
    QSize size(w,h);
    page->setViewportSize(size);
    page->setAttribute(QWebSettings::JavascriptEnabled, "off");
    //qDebug() << "Setting up SalorPage Done";

    this->capt = new SalorCapture(page, "/tmp/salor_customer_screen.bmp", 0,"","");

    //QTimer::singleShot(wait, &main, SLOT(Timeout()));

    //qDebug() << "Trying to connnect";
    webView->setPage((QWebPage*)page);
    //qDebug() << "Connecting slots";
    /* Connections */
    connect(
            webView->page(),
            SIGNAL(loadFinished(bool)),
            this->capt,
            SLOT(DocumentComplete(bool))
            );
    webView->load(QUrl(url));
}
