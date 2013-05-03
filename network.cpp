#include "network.h"
#include <QDateTime>
#include "common_includes.h"

Network::Network(QObject *parent) :
    QNetworkAccessManager(parent)
{
}

QNetworkReply * Network::createRequest(
    QNetworkAccessManager::Operation operation,
    const QNetworkRequest & originalRequest,
    QIODevice * device)
{
    QNetworkRequest request = originalRequest;

    if (
            (originalRequest.url().toString().indexOf(".css") != -1) ||
            (originalRequest.url().toString().indexOf(".png") != -1) ||
            (originalRequest.url().toString().indexOf(".js") != -1)
       ) {
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
            QNetworkRequest::PreferCache);
    } else {
      //qDebug() << originalRequest.url().toString() << " : " << QDateTime::currentDateTime();
    }
    return QNetworkAccessManager::createRequest(operation, request, device);
}


