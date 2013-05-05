#ifndef NETWORK_H
#define NETWORK_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>

class Network : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);

protected:
    QNetworkReply *	createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0 );
    
signals:
    
public slots:
    
};

#endif // NETWORK_H
