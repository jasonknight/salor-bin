#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "salorpage.h"
#include "salornetwork.h"
#include "optionsdialog.h"
#include "salorcookiejar.h"
#include "downloader.h"
#include "common_includes.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    if (settings->value("kiosk").toString() == "true")
        setWindowFlags(Qt::FramelessWindowHint);

    //networkManager = new QNetworkAccessManager(this);
    networkManager = new SalorNetwork(this);
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    shown = false;
    progress = 0;

    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    QWebSettings::globalSettings()->setOfflineStoragePath(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PrintElementBackgrounds, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    webView = new QWebView();
    page = new SalorPage(this);
    page->main = this;
    page->setForwardUnsupportedContent(true);
    webView->setPage(page);

    //networmanager
    //SalorNetwork* net = new SalorNetwork(this, networkManager);
    webView->page()->setNetworkAccessManager(networkManager);

    //cookiejar
    SalorCookieJar * jar = new SalorCookieJar(this);
    webView->page()->networkAccessManager()->setCookieJar(jar);

    //salorjsapi
    js = new SalorJsApi(this);
    js->webView = webView;

    //diskcache
    QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
    diskCache->setCacheDirectory(PathCache);
    webView->page()->networkAccessManager()->setCache(diskCache);

    //optionsdialog
    optionsDialog = new OptionsDialog(this, networkManager);
    connect(
            optionsDialog, SIGNAL(navigateToUrl(QString)),
            this, SLOT(navigateToUrl(QString))
            );
    connect(
            optionsDialog, SIGNAL(clearCache()),
            webView->page()->networkAccessManager()->cache(), SLOT(clear())
            );
    connect(
            optionsDialog, SIGNAL(setPrinterCounter(int)),
            this, SLOT(setPrinterCounter(int))
            );
    connect(
            optionsDialog, SIGNAL(setPrinterNames()),
            this, SLOT(setPrinterNames())
            );

    //statusbar
    statusBar = new QStatusBar(this);
    statusBar->setMaximumHeight(20);
    setStatusBar(statusBar);
    progressBar = new QProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->setMaximumWidth(70);
    urlLabel = new QLabel("Location");
    printCounterLabel = new QLabel();
    closeButton = new QPushButton();
    closeButton->setText("X");
    statusBar->addPermanentWidget(urlLabel);
    statusBar->addPermanentWidget(progressBar);
    statusBar->addPermanentWidget(printCounterLabel);
    statusBar->addPermanentWidget(closeButton);

    setCentralWidget(webView);
    webView->show();
    webView->load(to_url);
    connectSlots();
    counterSetup();
    timerSetup();
    setPrinterNames();
}

void MainWindow::setPrinterNames() {
    remotePrinterNames.clear();
    localPrinterNames.clear();
    qDebug() << "MainWindow::setPrinterNames()";
    QStringList groups = (*settings).childGroups();
    // filter out all setting groups that are not printer definitions
    for (int i = 0; i < groups.size(); i++) {
        QString group = groups[i];
        if (group.indexOf("printer") != -1) {
            remotePrinterNames << group;
        }
    }
#ifdef LINUX
    QStringList filters;
    filters << "ttyS*" << "ttyUSB*" << "usb";
    QDir * devs = new QDir("/dev", "*", QDir::Name, QDir::System);
    (*devs).setNameFilters(filters);
    QStringList nodes;
    nodes = devs->entryList();
    foreach (QString node, nodes) {
        localPrinterNames << "/dev/" + node;
    }
#endif
#ifdef WIN32
    QList<QPrinterInfo> printer_list = QPrinterInfo::availablePrinters();
    for (int i = 0; i < printer_list.length(); i++) {
        QPrinterInfo info = printer_list.at(i);
        localPrinterNames << info.printerName();
     }
#endif
#ifdef MAC
    int i;
    cups_dest_t *dests, *dest;
    int num_dests = cupsGetDests(&dests);
    for (i = num_dests, dest = dests; i > 0; i--, dest++) {
        localPrinterNames << dest->name;
     }
#endif
    qDebug() << "MainWindow::setPrinterNames():" << localPrinterNames;
}

void MainWindow::shutdown() {
    int pid = getpid();
    qDebug() << "Shutdown called.";
    QApplication::closeAllWindows();
#ifdef LINUX
    kill(pid,SIGKILL); // to make really sure
#endif
}

void MainWindow::connectSlots() {
    connect(webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this,SLOT(addJavascriptObjects()));
    connect(webView->page(), SIGNAL(windowCloseRequested()), this,SLOT(windowCloseRequested()));
    connect(webView, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(webView, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
    connect(page->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),page,SLOT(resetJsErrors()));
    connect(page,SIGNAL(unsupportedContent(QNetworkReply*)),page,SLOT(downloadFile(QNetworkReply*)));
    connect(page,SIGNAL(addWidget(QWidget*)),this,SLOT(addStatusBarWidget(QWidget*)));
    connect(page,SIGNAL(removeWidget(QWidget*)),this,SLOT(removeStatusBarWidget(QWidget*)));
    connect(page,SIGNAL(fileProgressUpdated(int)),this,SLOT(setProgress(int)));
    connect(this,SIGNAL(setPrinterNames()), sp, SLOT(setPrinterNames()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(shutdown()));

    QShortcut *showPrint = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_P), this);
    connect( showPrint, SIGNAL(activated()), js, SLOT(printPage()));

    QShortcut *f1 = new QShortcut(QKeySequence(Qt::Key_F1), this);
    connect( f1, SIGNAL(activated()), this, SLOT(f1Pressed()));

    QShortcut *f2 = new QShortcut(QKeySequence(Qt::Key_F2), this);
    connect( f2, SIGNAL(activated()), this, SLOT(f2Pressed()));

    QShortcut *f3 = new QShortcut(QKeySequence(Qt::Key_F3), this);
    connect( f3, SIGNAL(activated()), this, SLOT(f3Pressed()));

    QShortcut *f4 = new QShortcut(QKeySequence(Qt::Key_F4), this);
    connect( f4, SIGNAL(activated()), this, SLOT(f4Pressed()));

    QShortcut *f5 = new QShortcut(QKeySequence(Qt::Key_F5), this);
    connect( f5, SIGNAL(activated()), this, SLOT(f5Pressed()));

    QShortcut *endk = new QShortcut(QKeySequence(Qt::Key_End), this);
    connect( endk, SIGNAL(activated()), this, SLOT(endPressed()));

    QShortcut *ctrlins = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Insert), this);
    connect( ctrlins, SIGNAL(activated()), this, SLOT(ctrlinsPressed()));

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

    qreal z = settings->value("zoomFactor").toReal();
    if (z) {
      webView->page()->mainFrame()->setZoomFactor(z);
    }
}

QWebView* MainWindow::getWebView() {
    return webView;
}

void MainWindow::incZoom() {
  qreal z = webView->page()->mainFrame()->zoomFactor();
  z = z + 0.05;
  webView->page()->mainFrame()->setZoomFactor(z);
  settings->setValue("zoomFactor",z);
}

void MainWindow::decZoom() {
  qreal z = webView->page()->mainFrame()->zoomFactor();
  z = z - 0.05;
  webView->page()->mainFrame()->setZoomFactor(z);
  settings->setValue("zoomFactor",z);
}

void MainWindow::addStatusBarWidget(QWidget *w) {
    qDebug() << "Adding widget";
    statusBar->addWidget(w);
}

void MainWindow::removeStatusBarWidget(QWidget *w) {
    statusBar->removeWidget(w);
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
     progressBar->setValue(progress);
     urlLabel->setText(webView->url().encodedHost());
}

void MainWindow::navigateToUrl(QString url) {
    webView->load(QUrl(url));
}


void MainWindow::f1Pressed() {
    webView->page()->mainFrame()->evaluateJavaScript("f1Pressed();");
}

void MainWindow::f2Pressed() {
    webView->page()->mainFrame()->evaluateJavaScript("f2Pressed();");
}

void MainWindow::f3Pressed() {
    webView->page()->mainFrame()->evaluateJavaScript("f3Pressed();");
}

void MainWindow::f4Pressed() {
    webView->page()->mainFrame()->evaluateJavaScript("f4Pressed();");
}

void MainWindow::f5Pressed() {
    webView->page()->mainFrame()->evaluateJavaScript("f5Pressed();");
}

void MainWindow::endPressed() {
    webView->page()->mainFrame()->evaluateJavaScript("endPressed();");
}

void MainWindow::ctrlinsPressed() {
    webView->page()->mainFrame()->evaluateJavaScript("ctrlinsPressed();");
}

void MainWindow::addJavascriptObjects() {
    if (!shown) {
       shown = true; // i.e. this is ready
    }
    attach();
}

void MainWindow::repaintViews() {
    webView->update();
}

void MainWindow::attach(){
    //webView->page()->mainFrame()->addToJavaScriptWindowObject("SalorPrinter", sp);
    webView->page()->mainFrame()->addToJavaScriptWindowObject("Salor", js);
    page->resetJsErrors();
}

void MainWindow::windowCloseRequested() {
  QApplication::closeAllWindows();
}

void MainWindow::showOptionsDialog() {
    optionsDialog->show();
    optionsDialog->init();
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
    settings->beginGroup("printing");
    intervalPrint = settings->value("interval").toInt();
    settings->endGroup();

    if (intervalPrint < 10) intervalPrint = 10; // security measure
    counterPrint = intervalPrint;
}

void MainWindow::timerTimeout() {
    //qDebug() << "MainWindow::timerTimeout(): intervalPrint is " << intervalPrint;
    counterPrint--;
    if (counterPrint == 0) {
        counterPrint = intervalPrint;
        foreach(QString remoteprinter, remotePrinterNames) {
            settings->beginGroup(remoteprinter);
            sp = new SalorPrinter(this, networkManager, settings->value("localprinter").toString());
            sp->printURL(settings->value("url").toString());
            settings->endGroup();
        }
    }
    printCounterLabel->setText(QString::number(counterPrint));
}

void MainWindow::setPrinterCounter(int value){
    counterPrint = value;
}
