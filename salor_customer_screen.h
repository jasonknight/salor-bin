#ifndef SALOR_CUSTOMER_SCREEN_H
#define SALOR_CUSTOMER_SCREEN_H

#include <QObject>
#include <QPicture>
#include <QPainter>
#include <salor_settings.h>

class SalorCustomerScreen : public QObject
{
    Q_OBJECT
public:
    explicit SalorCustomerScreen(QObject *parent = 0);
    QPicture * background;
    QPicture * screen;
    QPainter * painter;
signals:
    void painted();
public slots:

    void paintImage();
    void flush();

};

#endif // SALOR_CUSTOMER_SCREEN_H
