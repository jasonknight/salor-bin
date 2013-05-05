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
    int counterPrint;
    
private:
    Ui::MainWindow *ui;
    QWebView *webView;
    SalorJsApi *js;
    SalorPrinter *sp;
    QBoxLayout * layout;
    int progress;
    QProgressBar * progressBar;
    QLabel * urlLabel;
    QLabel * printCounterLabel;
    void attach();
    QTimer * mainTimer;
    void timerSetup();
    void counterSetup();
    int intervalPrint;

private slots:
    void timerTimeout();

public slots:
    void setPrinterCounter(int);
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
