#ifndef SALORJSAPI_H
#define SALORJSAPI_H

#include <QObject>
#include "common_includes.h"
#include "drawerobserver.h"
#include "salorprocess.h"

class SalorJsApi : public QObject
{
    Q_OBJECT

public:
    explicit SalorJsApi(QObject *parent = 0, QNetworkAccessManager *nm = 0);
    QWebView *webView;
    DrawerObserver *drawerObserver;
    QThread *drawerThread;

private:
    QNetworkAccessManager *networkManager;

signals:
    void cuteDataRead(QString);
    void _cuteWriteData(QString);
    
public slots:
    void poleDancer(QString path, QString message, int baudrate = 9600);
    void newOpenCashDrawer(QString addy, int baudrate = 9600);
    void startDrawerObserver(QString addy, int baudrate = 9600);
    void stopDrawerObserver();
    void printPage();
    void playSound(QString name);
    void echo(QString msg);
    void mimoRefresh(QString path, int h, int w);
    void mimoImage(QString imagepath);
    QStringList ls(QString path,QStringList filters);
    void printURL(QString printer, QString url, QString callback_js = "", int baudrate = 9600);
    void printText(QString printer, QString text, int baudrate = 9600);
    void shutdown();
    void cuteWriteData(QString);
    void _cuteBubbleDataRead(QString data);
    QString weigh(QString addy, int protocol, int baudrate = 9600);
    void drawerThreadFinished();
    void drawerCloseDetected();
    void evaluateJS(QString js);
    
};

#endif // SALORJSAPI_H
