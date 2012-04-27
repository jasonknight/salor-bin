#ifndef SALORJSAPI_H
#define SALORJSAPI_H

#include <QObject>
#include <QDebug>
#include <QVariant>
#include <QMap>
#include <QWebView>
#include <QPrinter>
#include <QPrintDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QApplication>
#include <QWebFrame>
#include <QWebPage>
#include <sys/types.h>
#include <sys/stat.h>
#include "salorjsapi.h"
#include "salorprocess.h"
#include "salorprinter.h"
#include "salor_customer_screen.h"
#include "scales.h"
#include "cashdrawer.h"
#include "cutecredit.h"
class SalorJSApi : public QObject
{
    Q_OBJECT
public:
    explicit SalorJSApi(QObject *parent = 0);
    QWebView *webView;
    CuteCredit * credit_thread;
    DrawerObserverThread * drawer_thread;

signals:
    void cuteDataRead(QString);
    void _cuteWriteData(QString);

public slots:
 void poleDancer(QString path, QString message);
 QString toperScale(QString addy);
 QString testScale() {
   qDebug() << "Reading from Test: " << "2.754";
   return QString("2.754");
 }
 void newOpenCashDrawer(QString addy);
 void startDrawerObserver(QString addy);
 void stopDrawerObserver();
 void _cashDrawerClosed();
 void shutdown();
 void printPage();
 QStringList ls(QString path,QStringList filters);
 void completeOrderSnap(QString order_id);
 void generalSnap(QString msg);
 void playSound(QString name);
 QString version();
 void mimoRefresh(QString path,int h, int w);
 void mimoImage(QString imagepath);
 void cuteWriteData(QString);
 void _cuteBubbleDataRead(QString data);
 void x11VNC(QString url, QString username, QString password, QString type);
};

#endif // SALORJSAPI_H
