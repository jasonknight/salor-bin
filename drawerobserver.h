#ifndef DRAWEROBSERVER_H
#define DRAWEROBSERVER_H

#include "common_includes.h"

class DrawerObserver : public QObject
{
    Q_OBJECT

public:
    explicit DrawerObserver();
    QString mPath;

private:
    int mFiledescriptor;
    bool mdrawerClosed;
    bool mdrawerOpened;
    QSocketNotifier *m_notifier;

protected:
    void openDevice();
    void closeDevice();
    
private slots:
    void readData(int);

public slots:
    void start();
    void stop();

signals:
    void drawerCloseDetected();
    
};


#endif // DRAWERBOSERVER_H
