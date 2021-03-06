#ifndef SALORNOTIFICATOR_H
#define SALORNOTIFICATOR_H

#include <common_includes.h>

class SalorNotificator : public QObject
{
    Q_OBJECT
public:
    explicit SalorNotificator(QObject *parent = 0, QNetworkAccessManager *m_manager = 0);
    void start();
    bool connected;
    int currentState;
    void writeToSocket(QString msg);
    QString customerScreenId;

private:
    QTcpSocket socket;
    QNetworkAccessManager *m_manager;
    QString getHostname();

    
signals:
    void onTcpPrintNotified();
    void navigateToUrl(QString);

private slots:

    
public slots:
    void slotSocketRead();
    void slotSocketConnected();
    void slotSocketDisconnected();
    void slotSocketStateChanged(QAbstractSocket::SocketState);

    
};

#endif // SALORNOTIFICATOR_H
