#include "network.h"

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
        qDebug() << "Setting to prefer cache for: " << request.url().toString();
    }
    return QNetworkAccessManager::createRequest(operation, request, device);
}
