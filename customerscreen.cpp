#include "customerscreen.h"
#include "salorcapture.h"
#include "common_includes.h"

CustomerScreen::CustomerScreen(QObject *parent) :
    QObject(parent)
{
}

CustomerScreen::~CustomerScreen() {
    qDebug() << "### SalorCustomerScreen deleted.";
    delete webView;
    delete page;
    delete capt;
}

void CustomerScreen::finished() {
    qDebug() << "### SalorCustomerScreen finished.";
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

#ifdef LINUX
    QString PathWorking = QDir::homePath() + "/.Salor";
#endif
#ifdef WIN32
    QString PathWorking = _getcwd(__path,__size);
#endif
#ifdef MAC
    QString PathWorking = getcwd(__path,__size);
#endif

    QString outputfile = PathWorking + "/salor_customer_screen.bmp";
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
