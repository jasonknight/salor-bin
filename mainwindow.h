#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "salorpage.h"
#include "salorjsapi.h"
#include "salorprinter.h"
#include "common_includes.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();
    SalorPage * page;
    QStatusBar * statusBar;
    QString to_url;
    void connectSlots();
    bool shown;
    
private:
    Ui::MainWindow *ui;
    QWebView *webView;
    SalorJsApi *js;
    SalorPrinter *sp;
    QBoxLayout * layout;
    int progress;
    QProgressBar * status_bar_progressBar;
    QLabel * status_bar_urlLabel;
    void attach();

public slots:
    void setProgress(int);
    void finishLoading(bool);
    void adjustTitle();
    void repaintViews();
    void addJavascriptObjects();
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
    void showOptionsDialog();
    void navigateToUrl(QString url);
    void executeJS(QString &js);
    void addStatusBarWidget(QWidget * w);
    void removeStatusBarWidget(QWidget * w);
    QWebView* getWebView();
};

#endif // MAINWINDOW_H
