#ifndef SALORNOTIFICATOR_H
#define SALORNOTIFICATOR_H

#include <common_includes.h>

class SalorNotificator : public QObject
{
    Q_OBJECT
public:
    explicit SalorNotificator(QObject *parent = 0, QNetworkAccessManager *m_manager = 0);
    void start();
    void writestuff(QString text);
    bool connected;
    int currentState;

private:
    QTcpSocket socket;
    QNetworkAccessManager *m_manager;

    
signals:
    void onTcpPrintNotified();

private slots:

    
public slots:
    void slotSocketRead();
    void slotSocketConnected();
    void slotSocketDisconnected();
    void slotSocketStateChanged(QAbstractSocket::SocketState);

    
};

#endif // SALORNOTIFICATOR_H
