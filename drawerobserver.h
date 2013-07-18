#ifndef DRAWEROBSERVER_H
#define DRAWEROBSERVER_H

#include "common_includes.h"
#include "serialport.h"

class DrawerObserver : public QObject
{
    Q_OBJECT

public:
    explicit DrawerObserver(QString path, int baudrate = 9600);
    QString mPath;

    // for loop method (USB)
    bool doStop;
    bool drawerClosed;

private:
    int mBaudrate;
    Serialport *mSerialport;

    // for notifier method (RS232)
    QSocketNotifier *m_notifier;
    bool mdrawerClosed;
    bool mdrawerOpened;


protected:
    void openDevice();
    void closeDevice();
    
private slots:
    void readData(int);

public slots:
    void startWithLoop();

    void startWithNotifier();
    void stopWithNotifier();

signals:
    void drawerCloseDetected();
    
};


#endif // DRAWERBOSERVER_H
