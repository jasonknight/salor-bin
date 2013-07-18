#ifndef SALORPRINTER_H
#define SALORPRINTER_H

#include "common_includes.h"
#include "serialport.h"

#ifdef WIN32
extern void display_last_error(DWORD dw);
#endif

class SalorPrinter : public QObject
{
    Q_OBJECT

public:
    explicit SalorPrinter(QObject *parent = 0, QNetworkAccessManager *nm = 0, QString printer = "", int baudrate = 9600);

private:
    QNetworkAccessManager *m_manager;
    QString m_printer;
    void printed();
    bool auth_tried;
    QNetworkRequest *m_request;
    Serialport *m_serialport;

public slots:
    void printURL(QString url);
    void print(QByteArray printdata);

private slots:
    void printDataReady();
    void onError(QNetworkReply::NetworkError);

    
};

#endif // SALORPRINTER_H
