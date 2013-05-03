#ifndef CUSTOMERSCREEN_H
#define CUSTOMERSCREEN_H

#include <QObject>
#include <QPicture>
#include <QPainter>
#include <QUrl>
#include <QWebView>
#include "salorpage.h"
#include "salorcapture.h"
#include "common_includes.h"

class CustomerScreen : public QObject
{
    Q_OBJECT
public:
    explicit CustomerScreen(QObject *parent = 0);
    ~CustomerScreen();
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

#endif // CUSTOMERSCREEN_H
