#ifndef SALORNETWORK_H
#define SALORNETWORK_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>

class SalorNetwork : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit SalorNetwork(QObject *parent = 0);

protected:
    QNetworkReply *	createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0 );
    
signals:
    
public slots:
    
};

#endif // SALORNETWORK_H
