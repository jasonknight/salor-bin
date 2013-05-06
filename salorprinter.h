#ifndef SALORPRINTER_H
#define SALORPRINTER_H

#include "common_includes.h"

#ifdef WIN32
extern void display_last_error(DWORD dw);
#endif

class SalorPrinter : public QObject
{
    Q_OBJECT

public:
    explicit SalorPrinter(QObject *parent = 0, QNetworkAccessManager *nm = 0, QString printer = "");

private:
    QNetworkAccessManager *m_manager;
    QString m_printer;
    void printed();
    bool auth_tried;
    QNetworkRequest *m_request;

public slots:
    void printURL(QString url);
    void print(QByteArray printdata);

private slots:
    void printDataReady(QNetworkReply * reply);
    void on_authenticationRequired(QNetworkReply *reply, QAuthenticator *auth);

    
};

#endif // SALORPRINTER_H
