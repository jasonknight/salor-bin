#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebKit>
#include <QSplashScreen>
#include "salor_customer_screen.h"
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();
    void connectSlots();
    bool mousePressed;
    bool shown;
    SalorCustomerScreen * scs;
    QString to_url;
private slots:

public slots:
    void linkClicked(QUrl);
    void repaintViews();
    QWebView* getWebView();
    void addJavascriptObjects();
   // bool eventFilter(QObject *, QEvent *);
    void windowCloseRequested();
    QString toperScale(QString addy);
    QString testScale() {
      qDebug() << "Reading from Test: " << "2.754";
      return QString("2.754");
    }
    void newOpenCashDrawer(QString addy);
    void cashDrawerClosed(QString addy);
    void _cashDrawerClosed();
    void payLifeSend(QString addy, QString data);
    void shutdown();
protected:
    //void changeEvent(QEvent *e);
private:
    QWebView *webView;
    void attach();
};

#endif // MAINWINDOW_H
