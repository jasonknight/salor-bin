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
    explicit SalorPrinter(QObject *parent = 0);
    void print(QString printer, QByteArray printdata);
    void printURL(QString printer, QString url, QString confirm_url = "");
    void setPrinterNames();

private:
    QNetworkAccessManager * m_manager;
    QString m_printer_path;
    QString confirmation_url;
    void printed();
    bool auth_tried;

private slots:
    void printDataReady(QNetworkReply * reply);
    void on_authenticationRequired(QNetworkReply *reply, QAuthenticator *auth);

    
};

#endif // SALORPRINTER_H
