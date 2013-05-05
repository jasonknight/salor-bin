#ifndef SALORPRINTER_H
#define SALORPRINTER_H

#include "common_includes.h"

class SalorPrinter : public QObject
{
    Q_OBJECT

public:
    explicit SalorPrinter(QObject *parent = 0);
    void print(QString printer, QByteArray printdata);
    void printURL(QString printer, QString url, QString confirm_url = "");

private:
    QNetworkAccessManager * m_manager;
    QString m_printer_path;
    QString confirmation_url;
    void printed();
    
signals:

private slots:
    void printDataReady(QNetworkReply * reply);
    
public slots:
    
};

#endif // SALORPRINTER_H
