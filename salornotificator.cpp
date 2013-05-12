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
    QUrl url;
    url = QUrl(settings->value("url").toString());
    qDebug() << "SalorNotificator::start(): connecting to " << url.host();
    socket.connectToHost(url.host(), 2000);
}

void SalorNotificator::slotSocketRead(){
    QString msg;
    msg = socket.readAll();
    msg.replace("\n", "");
    if (msg == "")
        return;

    //qDebug() << "TCP read from server" << msg;
    if (msg.indexOf("ID") != -1) {
        qDebug() << "Server push notification to submit ID";
        settings->beginGroup("printing");
        socket.write("ID|" + settings->value("username").toString().toAscii() + "\n");
        settings->endGroup();
    } else if (msg.indexOf("PING") != -1) {
        //qDebug() << "Server sent PING";
    } else if (msg.indexOf("PRINTEVENT") != -1) {
        //qDebug() << "Server push notification for printing";
        QStringList parts = msg.split("|");
        QString url_firstpart = settings->value("url").toString();
        settings->beginGroup(parts[1]);
        QString localprinter = settings->value("localprinter").toString();
        QString url = url_firstpart + settings->value("url").toString();
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
        qDebug() << "SalorNotificator::slotSocketRead(): Server sent something unsupported:" << msg;
    }
}

void SalorNotificator::writeToSocket(QString msg) {
    //qDebug() << "SalorNotificator::writeToSocket:" << msg;
    socket.write(msg.toAscii() + "\n");
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
    qDebug() << "SalorNotificator::slotSocketStateChange:" << QString::number(currentState);
}