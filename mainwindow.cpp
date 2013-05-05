#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "salorpage.h"
#include "network.h"
#include "salorprinter.h"
#include "optionsdialog.h"
#include "salorcookiejar.h"
#include "downloader.h"
#include "common_includes.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    this->shown = false;
    progress = 0;
    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    QWebSettings::globalSettings()->setOfflineStoragePath(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PrintElementBackgrounds, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    webView = new QWebView();
    this->page = new SalorPage(this);
    this->page->main = this;
    this->page->setForwardUnsupportedContent(true);
    webView->setPage(this->page);

    //signals and slots
    connect(this->page,SIGNAL(unsupportedContent(QNetworkReply*)),this->page,SLOT(downloadFile(QNetworkReply*)));
    connect(this->page,SIGNAL(addWidget(QWidget*)),this,SLOT(addStatusBarWidget(QWidget*)));
    connect(this->page,SIGNAL(removeWidget(QWidget*)),this,SLOT(removeStatusBarWidget(QWidget*)));
    connect(this->page,SIGNAL(fileProgressUpdated(int)),this,SLOT(setProgress(int)));

    //networmanager
    Network* net = new Network(this);
    webView->page()->setNetworkAccessManager(net);

    //cookiejar
    SalorCookieJar * jar = new SalorCookieJar(this);
    webView->page()->networkAccessManager()->setCookieJar(jar);

    //salorprinter
    this->sp = new SalorPrinter(this);

    //salorjsapi
    this->js = new SalorJsApi(this);
    this->js->webView = this->webView;

    //diskcache
    QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
    diskCache->setCacheDirectory(PathCache);
    webView->page()->networkAccessManager()->setCache(diskCache);

    //statusbar
    statusBar = new QStatusBar(this);
    statusBar->setMaximumHeight(20);
    this->setStatusBar(statusBar);
    progressBar = new QProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->setMaximumWidth(50);
    urlLabel = new QLabel("Location");
    printCounterLabel = new QLabel();
    statusBar->addPermanentWidget(urlLabel);
    statusBar->addPermanentWidget(progressBar);
    statusBar->addPermanentWidget(printCounterLabel);

    setCentralWidget(webView);
    webView->show();
    webView->load(this->to_url);
    connectSlots();
    timerSetup();
    counterSetup();
}

void MainWindow::connectSlots() {
    connect(webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this,SLOT(addJavascriptObjects()));
    connect(webView->page(), SIGNAL(windowCloseRequested()), this,SLOT(windowCloseRequested()));
    connect(this->page->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),this->page,SLOT(resetJsErrors()));
    connect(webView, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(webView, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_P), this);
    connect( shortcut, SIGNAL(activated()), this->js, SLOT(printPage()));

    QShortcut *f2 = new QShortcut(QKeySequence(Qt::Key_F2), this);
    connect( f2, SIGNAL(activated()), this, SLOT(lastFiveOrders()));

    QShortcut *ctrlf2 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F2), this);
    connect( ctrlf2, SIGNAL(activated()), this, SLOT(customersIndex()));

    QShortcut *f3 = new QShortcut(QKeySequence(Qt::Key_F3), this);
    connect( f3, SIGNAL(activated()), this, SLOT(showSearch()));

    QShortcut *f4 = new QShortcut(QKeySequence(Qt::Key_F4), this);
    connect( f4, SIGNAL(activated()), this, SLOT(showCashDrop()));

    QShortcut *f5 = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect( f5, SIGNAL(activated()), this, SLOT(endDayReport()));

    QShortcut *ctrlins = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Insert), this);
    connect( ctrlins, SIGNAL(activated()), this, SLOT(editLastAddedItem()));

    QShortcut *endk = new QShortcut(QKeySequence(Qt::Key_End), this);
    connect( endk, SIGNAL(activated()), this, SLOT(completeOrder()));

    QShortcut *zoomin = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this);
    connect( zoomin, SIGNAL(activated()), this, SLOT(incZoom()));

    QShortcut *zoomout = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus), this);
    connect( zoomout, SIGNAL(activated()), this, SLOT(decZoom()));

    #ifdef MAC
        QShortcut *showOptions = new QShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_O), this);
        connect( showOptions, SIGNAL(activated()), this, SLOT(showOptionsDialog()));
    #else
        QShortcut *showOptions = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this);
        connect( showOptions, SIGNAL(activated()), this, SLOT(showOptionsDialog()));
    #endif

    QSettings settings("JolieRouge", "Salor");
    qreal z = settings.value("zoomFactor").toReal();
    if (z) {
      this->webView->page()->mainFrame()->setZoomFactor(z);
    }
}

QWebView* MainWindow::getWebView() {
    return this->webView;
}

void MainWindow::incZoom() {
  QSettings settings("JolieRouge", "Salor");
  qreal z = this->webView->page()->mainFrame()->zoomFactor();
  z = z + 0.05;
  this->webView->page()->mainFrame()->setZoomFactor(z);
  settings.setValue("zoomFactor",z);
}

void MainWindow::decZoom() {
  QSettings settings("JolieRouge", "Salor");
  qreal z = this->webView->page()->mainFrame()->zoomFactor();
  z = z - 0.05;
  this->webView->page()->mainFrame()->setZoomFactor(z);
  settings.setValue("zoomFactor",z);
}

void MainWindow::addStatusBarWidget(QWidget *w) {
    qDebug() << "Adding widget";
    this->statusBar->addWidget(w);
}

void MainWindow::removeStatusBarWidget(QWidget *w) {
    this->statusBar->removeWidget(w);
}

void MainWindow::setProgress(int p) {
    //qDebug() << "set progress called" << QString::number(p);
    progress = p;
    adjustTitle();
}

void MainWindow::finishLoading(bool) {
     //progress = 100;
     adjustTitle();
}

void MainWindow::executeJS(QString &js) {
    webView->page()->mainFrame()->evaluateJavaScript(js);
}

void MainWindow::adjustTitle() {
     if (progress <= 0 || progress >= 100)
         setWindowTitle(webView->title());
     else
         setWindowTitle(QString("%1 (%2%)").arg(webView->title()).arg(progress));
     this->progressBar->setValue(progress);
     this->urlLabel->setText(this->webView->url().encodedHost());
}

void MainWindow::navigateToUrl(QString url) {
    this->webView->load(QUrl(url));
}

void MainWindow::customersIndex() {
    this->webView->page()->mainFrame()->evaluateJavaScript("window.location = '/customers';");
}

void MainWindow::endDayReport() {
    this->webView->page()->mainFrame()->evaluateJavaScript("window.location = '/orders/report_day';");
}

void MainWindow::editLastAddedItem() {
    this->webView->page()->mainFrame()->evaluateJavaScript("editLastAddedItem();");
}

void MainWindow::lastFiveOrders() {
    this->webView->page()->mainFrame()->evaluateJavaScript("onF2Key();");
}

void MainWindow::completeOrder() {
    this->webView->page()->mainFrame()->evaluateJavaScript("onEndKey();");
}

void MainWindow::showSearch() {
    this->webView->page()->mainFrame()->evaluateJavaScript("showSearch();");
}

void MainWindow::showCashDrop() {
    this->webView->page()->mainFrame()->evaluateJavaScript("show_cash_drop();");
}

void MainWindow::addJavascriptObjects() {
    if (!this->shown) {
       this->shown = true; // i.e. this is ready
    }
    attach();
}

void MainWindow::repaintViews() {
    webView->update();
}

void MainWindow::attach(){
    this->webView->page()->mainFrame()->addToJavaScriptWindowObject("SalorPrinter", this->sp);
    this->webView->page()->mainFrame()->addToJavaScriptWindowObject("Salor", this->js);
    this->page->resetJsErrors();
}

void MainWindow::windowCloseRequested() {
  QApplication::closeAllWindows();
}

void MainWindow::showOptionsDialog() {
    OptionsDialog* d = new OptionsDialog(this);
    connect(
            d, SIGNAL(navigateToUrl(QString)),
            this, SLOT(navigateToUrl(QString))
            );
    connect(
            d, SIGNAL(clearCache()),
            webView->page()->networkAccessManager()->cache(), SLOT(clear())
            );
    connect(
            d, SIGNAL(resetPrinterCounter(int)),
            this, SLOT(resetPrinterCounter(int))
            );
    d->show();
}

void MainWindow::timerSetup() {
    qDebug() << "MainWindow::printTimerSetup()";
    mainTimer = new QTimer(this);
    mainTimer->start(1000);
    connect(
        mainTimer, SIGNAL(timeout()),
        this, SLOT(timerTimeout())
    );
}


void MainWindow::counterSetup() {
    qDebug() << "MainWindow::counterSetup()";
    QSettings * settings = new QSettings(PathSettings, QSettings::IniFormat);
    settings->beginGroup("printing");
    intervalPrint = settings->value("interval").toInt();
    settings->endGroup();

    if (intervalPrint < 10) intervalPrint = 10; // security measure
    counterPrint = intervalPrint;
}

void MainWindow::timerTimeout() {
    //qDebug() << "MainWindow::timerTimeout()";
    counterPrint--;
    if (counterPrint == 0) {
        counterPrint = intervalPrint;
        qDebug() << "counter print 0";
    }
    printCounterLabel->setText(QString::number(counterPrint));
    /*
    settings->beginGroup("printer-info");
    QNetworkRequest request(QUrl::fromUserInput(settings->value("url").toString()));
    QNetworkAccessManager * manager = new QNetworkAccessManager(this);
    connect(manager,
            SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this,
            SLOT(on_authenticationRequired(QNetworkReply*,QAuthenticator*))
    );

    QSslConfiguration c = request.sslConfiguration();
    c.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(c);

    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(on_printoutFetched(QNetworkReply*)));
    manager->get(request);
    settings->endGroup();*/
}

void MainWindow::resetPrinterCounter(int value){
    counterPrint = value;
}
