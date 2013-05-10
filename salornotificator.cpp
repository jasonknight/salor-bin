#include "salornotificator.h"
#include "salorprinter.h"
#include "common_includes.h"
#include <QObject>

SalorNotificator::SalorNotificator(QObject *parent, QNetworkAccessManager *nm) :
    QObject(parent)
{
    //socket = QTcpSocket(this);
    connected = false;
    currentState = 0;
    m_manager = nm;

    connect(&socket, SIGNAL(readyRead()),
            this, SLOT(slotSocketRead()));
    connect(&socket, SIGNAL(connected()),
            this, SLOT(slotSocketConnected()));
    connect(&socket, SIGNAL(disconnected()),
            this, SLOT(slotSocketDisconnected()));
    connect(&socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(slotSocketStateChanged(QAbstractSocket::SocketState)));
}

void SalorNotificator::start()
{
    settings->beginGroup("printing");
    QUrl url(settings->value("url").toString());
    settings->endGroup();
    //qDebug() << url.host();
    socket.connectToHost(url.host(), 2000);
}

void SalorNotificator::slotSocketRead(){
    QByteArray msg;
    msg = socket.readAll();
    //qDebug() << "TCP read from server" << msg;
    if (msg.indexOf("ID") != -1) {
        qDebug() << "Server push notification to submit ID";
        settings->beginGroup("printing");
        socket.write(settings->value("username").toString().toAscii() + "\n");
        settings->endGroup();
    } else if (msg.indexOf("printer") != -1) {
        //qDebug() << "Server push notification for printing";
        msg.replace("\n", "");
        settings->beginGroup(msg);
        QString localprinter = settings->value("localprinter").toString();
        QString url = settings->value("url").toString();
        settings->endGroup();
        if (localprinter == "") {
            //qDebug() << "Not fetching anything for local printer" << msg;
            // don't print when localprinter combo box has been left empty
        } else {
            SalorPrinter *printer = new SalorPrinter(this, m_manager, localprinter);
            printer->printURL(url);
            // printer instance will delete itself after printing. we cannot do it here since long running network requests are involved.
        }
        emit onTcpPrintNotified();
    } else {
        //qDebug() << "Unknown server request" << msg;
    }
}

void SalorNotificator::slotSocketConnected(){
    qDebug() << "SalorNotificator::slotSocketConnected()";
    connected = true;
}

void SalorNotificator::slotSocketDisconnected(){
    qDebug() << "SalorNotificator::slotSocketDisconnected()";
    connected = false;
}

void SalorNotificator::slotSocketStateChanged(QAbstractSocket::SocketState state) {
    currentState = (int)state;
    //qDebug() << "SalorNotificator::slotSocketStateChange:" << QString::number(currentState);
}
