#ifndef DRAWEROBSERVER_H
#define DRAWEROBSERVER_H

#include <QThread>
#include "common_includes.h"

class DrawerObserverThread : public QThread
{
    Q_OBJECT
public:
    explicit DrawerObserverThread(QObject *parent = 0, QString path = "");
    bool  stop_drawer_thread;
    void run();
    QString mPath;
    int mFiledescriptor;
    bool drawerClosed;

protected:
    void open();
    void close();
    
signals:
    void cashDrawerClosed();
    
public slots:
    
};


#endif // DRAWERBOSERVER_H
