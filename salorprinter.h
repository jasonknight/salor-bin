#ifndef SALORPRINTER_H
#define SALORPRINTER_H

#include "common_includes.h"

class SalorPrinter : public QObject
{
    Q_OBJECT
public:
    explicit SalorPrinter(QObject *parent = 0);
    QNetworkAccessManager * m_manager;
    QString m_printer_path;
    QString confirmation_url;
    
signals:
    void printed();
    void printerDoesNotExist();
    
public slots:
    void printURL(QString path, QString url, QString confirm_url);
    void pageFetched(QNetworkReply * reply);
    
};

#endif // SALORPRINTER_H
