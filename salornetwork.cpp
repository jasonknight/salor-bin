#include "salornetwork.h"
#include <QDateTime>
#include "common_includes.h"

SalorNetwork::SalorNetwork(QObject *parent) :
    QNetworkAccessManager(parent)
{
}

QNetworkReply * SalorNetwork::createRequest(
        QNetworkAccessManager::Operation operation,
        const QNetworkRequest & originalRequest,
        QIODevice * device)
{
    QNetworkRequest request = originalRequest;
    qDebug() << "[SalorNetwork]" << "[createRequest] Beginning.";
    if (
            (originalRequest.url().toString().indexOf(".css") != -1) ||
            (originalRequest.url().toString().indexOf(".png") != -1) ||
            (originalRequest.url().toString().indexOf(".js") != -1)
       ) {
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
            QNetworkRequest::PreferCache);
    } else {
      qDebug() << originalRequest.url().toString() << " : " << QDateTime::currentDateTime();
    }
    QSslConfiguration c = request.sslConfiguration();
    c.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(c);
    qDebug() << "[SalorNetwork]" << "[createRequest] setting of ssl crap complete. ";
    return QNetworkAccessManager::createRequest(operation, request, device);
}


