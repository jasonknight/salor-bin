#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebKit>
#include "salor_settings.h"
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();
    void connectSlots();

private slots:

public slots:
    void linkClicked(QUrl);
    QWebView* getWebView();
    void addJavascriptObjects();
protected:
    void changeEvent(QEvent *e);
private:
    QWebView *webView;
    SalorSettings *s;


    void attach();
};

#endif // MAINWINDOW_H
