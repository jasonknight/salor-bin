#ifndef SALOR_CUSTOMER_SCREEN_H
#define SALOR_CUSTOMER_SCREEN_H

#include <QObject>
#include <QPicture>
#include <QPainter>
#include <salor_settings.h>
#include "salor_page.h"

class SalorCustomerScreen : public QObject
{
    Q_OBJECT
public:
    explicit SalorCustomerScreen(QObject *parent = 0);
    QPicture * background;
    QPicture * screen;
    QPainter * painter;
    SalorCapture * capt;
signals:
    void painted();
public slots:
    void refresh(QString url,int h, int w);
    void print(QString url);
    void flush();

};

#endif // SALOR_CUSTOMER_SCREEN_H
