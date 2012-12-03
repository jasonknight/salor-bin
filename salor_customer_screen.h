#ifndef SALOR_CUSTOMER_SCREEN_H
#define SALOR_CUSTOMER_SCREEN_H

#include <QObject>
#include <QPicture>
#include <QPainter>
#include "salor_page.h"
#include <QtWebKit>
#include <QUrl>
#include <QWebView>
#include "salor_page.h"
class SalorCustomerScreen : public QObject
{
    Q_OBJECT
public:
    SalorCustomerScreen(QObject *parent = 0);
    ~SalorCustomerScreen();
    QPicture * background;
    QPicture * screen;
    QPainter * painter;
    SalorCapture * capt;
    QWebView * webView;
    SalorPage * page;
signals:
    void painted();
    void done();
public slots:
    void refresh(QString url,int h, int w);
    void finished();
};

#endif // SALOR_CUSTOMER_SCREEN_H
