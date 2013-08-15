#include "customerscreen.h"
#include "salorcapture.h"
#include "common_includes.h"

CustomerScreen::CustomerScreen(QObject *parent, QNetworkAccessManager *nm) :
    QObject(parent)
{
    m_manager = nm;
}

CustomerScreen::~CustomerScreen() {
    delete webView;
    delete page;
    delete capt;
}

void CustomerScreen::finished() {
    qDebug() << "[CustomerScreen]" << "[finished]" << "Called.";
    deleteLater();
}

void CustomerScreen::refresh(QString url, int w, int h) {
    QSize size(w,h);
    QString outputfile = PathWorking + "/salor_customer_screen.bmp";

    qDebug() << "[CustomerScreen]" << "[refresh]" << "called";

    qDebug() << "[CustomerScreen]" << "[refresh]" << "Instatiating SalorPage";
    page = new SalorPage();

    qDebug() << "[CustomerScreen]" << "[refresh]" << "Setting NetworkManager for SalorPage";
    page->setNetworkAccessManager(m_manager);

    qDebug() << "[CustomerScreen]" << "[refresh]" << "Setting viewport size for SalorPage";
    page->setViewportSize(size);

    qDebug() << "[CustomerScreen]" << "[refresh]" << "Turning JS off for SalorPage";
    page->setAttribute(QWebSettings::JavascriptEnabled, "off");


    qDebug() << "[CustomerScreen]" << "[refresh]" << "Instantiating SalorCapture for outputfile" << outputfile;
    capt = new SalorCapture(page, outputfile, 0, "", "");


    qDebug() << "[CustomerScreen]" << "[refresh]" << "Instantiating QWebView";
    webView = new QWebView();

    qDebug() << "[CustomerScreen]" << "[refresh]" << "Setting QWebPage for QWebView";
    webView->setPage((QWebPage*)page);

    qDebug() << "[CustomerScreen]" << "[refresh]" << "Connecting signals for QWebView";
    connect(
        page,
        SIGNAL(loadFinished(bool)),
        capt,
        SLOT(DocumentComplete(bool))
    );
    connect(
        capt,
        SIGNAL(done()),
        this,
        SLOT(finished())
    );

    qDebug() << "[CustomerScreen]" << "[refresh]" << "Loading url" << url << "in QWebView";
    webView->load(QUrl(url));

    qDebug() << "[CustomerScreen]" << "[refresh]" << "Done.";
}
