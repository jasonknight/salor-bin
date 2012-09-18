#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "common_includes.h"
class Network : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);
    
signals:
    
public slots:

protected:
   QNetworkReply *	createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0 );
    
};

#endif // NETWORK_H
