#ifndef DRAWEROBSERVER_H
#define DRAWEROBSERVER_H

#include "common_includes.h"

class DrawerObserver : public QObject
{
    Q_OBJECT

public:
    explicit DrawerObserver();
    bool doStop;
    QString mPath;
    int mFiledescriptor;

protected:
    void openDevice();
    void closeDevice();
    
public slots:
    void observe();
    
};


#endif // DRAWERBOSERVER_H
