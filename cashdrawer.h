#ifndef CASHDRAWER_H
#define CASHDRAWER_H

#include <QThread>

class CashDrawer : public QThread
{
    Q_OBJECT
public:
    explicit CashDrawer(QObject *parent = 0);
    QString addy;
    void run();
signals:
    void cashDrawerClosed();
public slots:

};

#endif // CASHDRAWER_H
