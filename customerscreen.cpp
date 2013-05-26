#include "customerscreen.h"
#include "salorcapture.h"
#include "common_includes.h"

CustomerScreen::CustomerScreen(QObject *parent) :
    QObject(parent)
{
}

CustomerScreen::~CustomerScreen() {
    delete webView;
    delete page;
    delete capt;
}

void CustomerScreen::finished() {
    this->deleteLater();
}

void CustomerScreen::refresh(QString url,int w, int h) {
    // The screenshot taking is done in salor_page.cpp, scroll down and look at SalorCapture class
    qDebug() << "CustomerScreen.refresh called with " << url;
    this->webView = new QWebView();
    qDebug() << "Setting up SalorPage";
    this->page = new SalorPage();
    qDebug() << "Setting viewport size";
    QSize size(w,h);
    page->setViewportSize(size);
    page->setAttribute(QWebSettings::JavascriptEnabled, "off");
    qDebug() << "Setting up SalorPage Done";

    QString outputfile = PathWorking + "/salor_customer_screen.bmp";
    qDebug() << "Outputfile is" << outputfile;
    
    this->capt = new SalorCapture(page, outputfile, 0,"","");

    webView->setPage((QWebPage*)this->page);
    connect(
        webView->page(),
        SIGNAL(loadFinished(bool)),
        this->capt,
        SLOT(DocumentComplete(bool))
    );
    connect(
        this->capt,
        SIGNAL(done()),
        this,
        SLOT(finished())
    );
    webView->load(QUrl(url));

    qDebug() << "SalorCustomerScreen::refresh complete.";
}
