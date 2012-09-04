#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebKit>
#include <QSplashScreen>
#include "salor_customer_screen.h"
#include "cashdrawer.h"
#include <QVariant>
#include "salorprocess.h"
#include "salorjsapi.h"
#include "salorprinter.h"
#include "salor_page.h"
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();
    void connectSlots();
    bool mousePressed;
    bool shown;
    QString to_url;
    SalorPage * page;
private slots:

public slots:
    void repaintViews();
    QWebView* getWebView();
    void addJavascriptObjects();
   // bool eventFilter(QObject *, QEvent *);
    void windowCloseRequested();
    void lastFiveOrders();
    void completeOrder();
    void showSearch();
    void showCashDrop();
    void incZoom();
    void decZoom();
    void customersIndex();
    void editLastAddedItem();
    void endDayReport();
signals:
    void camWasCaptured(int id,QString filePath);
    void dataRead(QString source, QString data);
protected:
    //void changeEvent(QEvent *e);
private:
    QWebView *webView;
    SalorPrinter *sp;
    SalorJSApi *js;
    void attach();
};

#endif // MAINWINDOW_H
