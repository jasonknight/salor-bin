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
    qDebug() << "[SalorNotificator]" << "[Constructor] Ending.";
}

void SalorNotificator::start()
{
    qDebug() << "[SalorNotificator]" << "[start] Beginning.";
    QUrl url;
    settings->beginGroup("printing");
    url = QUrl(settings->value("url").toString());
    settings->endGroup();
    qDebug() << "SalorNotificator::start(): connecting to " << url.host();
    socket.connectToHost(url.host(), 2000);
    qDebug() << "[SalorNotificator]" << "[start] Ending.";
}

QString SalorNotificator::getHostname() {
    settings->beginGroup("printing");
    QString url_firstpart = settings->value("url").toString();
    settings->endGroup();

    // strip basename. it must end in a slash!
    int pos = 0;
    QRegExp rx("(htt.*\://.*)/");
    pos = rx.indexIn(url_firstpart);
    url_firstpart = rx.cap(1);

    return url_firstpart;
}

void SalorNotificator::slotSocketRead() {
    qDebug() << "[SalorNotificator]" << "[slotSocketRead] Beginning.";
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

        QString url_firstpart = getHostname();

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

    } else if (msg.indexOf("CUSTOMERSCREENEVENT") != -1) {
        qDebug() << "Server push notification for customer screen refresh" << msg;
        QString url_firstpart = getHostname();
        QStringList parts = msg.split("|");
        if (parts[1] == customerScreenId) {
            QString url;
            if (parts[2].indexOf("http") != -1) {
                // a full URL has been sent
                url = parts[2];
            } else {
                // a relative path has been sent
                url = url_firstpart + parts[2];
            }
            emit(navigateToUrl(url));
        }
    } else {
        qDebug() << "SalorNotificator::slotSocketRead(): Server sent something unsupported:" << msg;
    }
    //qDebug() << "[SalorNotificator]" << "[slotSocketRead] Ending.";
}

void SalorNotificator::writeToSocket(QString msg) {
    //qDebug() << "[SalorNotificator]" << "[writeToSocket] Beginning.";
    socket.write(msg.toAscii() + "\n");
    //qDebug() << "[SalorNotificator]" << "[writeToSocket] Ending.";
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
