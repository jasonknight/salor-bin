#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "salorpage.h"
#include "salornetwork.h"
#include "optionsdialog.h"
#include "salorcookiejar.h"
#include "downloader.h"
#include "salornotificator.h"
#include "common_includes.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "[MainWindow]" << "[Constructor] Contructing MainWindow Object.";

    networkManager = new SalorNetwork(this);
    qDebug() << "[MainWindow]" << "[Constructor] SalorNetwork instantiated.";

    ui->setupUi(this);
    qDebug() << "[MainWindow]" << "[Constructor] UI Has been set up.";
}

MainWindow::~MainWindow()
{
    qDebug() << "[MainWindow]" << "[Destructor] deleting UI.";
    delete ui;
}

void MainWindow::init()
{
    qDebug() << "[MainWindow]" << "[init] Beginning.";
    shown = false;
    progress = 0;

    if (fullscreen == true) {
        setWindowFlags(Qt::FramelessWindowHint);
        qDebug() << "[MainWindow]" << "[Constructor] Setting Qt::FramelessWindowHint";
    }

    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    QWebSettings::globalSettings()->setOfflineStoragePath(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PrintElementBackgrounds, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    qDebug() << "[MainWindow]" << "[init]  QWebSettings updated.";

    webView = new QWebView();
    page = new SalorPage(this);
    page->main = this;
    page->setForwardUnsupportedContent(true);
    qDebug() << "[MainWindow]" << "[init] SalorPage instantiated and assigned.";
    webView->setPage(page);

    //networmanager
    //SalorNetwork* net = new SalorNetwork(this, networkManager);
    webView->page()->setNetworkAccessManager(networkManager);
    qDebug() << "[MainWindow]" << "[init] NetworkAccessManager for SalorPage set.";

    //cookiejar
    SalorCookieJar * jar = new SalorCookieJar(this);
    jar->customerScreenId = customerScreenId;
    jar->setup();
    webView->page()->networkAccessManager()->setCookieJar(jar);
    qDebug() << "[MainWindow]" << "[init] SalorCookieJar instantiated and set.";

    //salorjsapi
    js = new SalorJsApi(this, networkManager);
    js->webView = webView;
    qDebug() << "[MainWindow]" << "[init] SalorJsApi instantiated and injected with networkManager and webView.";

    //diskcache
    QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
    diskCache->setCacheDirectory(PathCache);
    webView->page()->networkAccessManager()->setCache(diskCache);

    //optionsdialog
    optionsDialog = new OptionsDialog(this, networkManager);
    optionsDialog->customerScreenId = customerScreenId;

    // when go button on options dialog is pressed
    connect(
            optionsDialog, SIGNAL(navigateToUrl(QString)),
            this, SLOT(navigateToUrl(QString))
            );
    // when clear cache button on options dialog is pressed
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
    qDebug() << "[MainWindow]" << "[init] OptionsDialog instantiated and signals connected.";


    // salorNotificator
    salorNotificator = new SalorNotificator(this, networkManager);
    qDebug() << "[MainWindow]" << "[Constructor] SalorNotificator instantiated.";
    salorNotificator->customerScreenId = customerScreenId;
    connect( salorNotificator,
             SIGNAL(onTcpPrintNotified()),
             this,
             SLOT(onTcpPrintNotified())
    );
    connect( salorNotificator,
             SIGNAL(connectionStatusChanged(QAbstractSocket::SocketState)),
             this,
             SLOT(updateNotificatorLabel(QAbstractSocket::SocketState))
    );
    // when customer screen event is sent
    connect(
            salorNotificator, SIGNAL(navigateToUrl(QString)),
            this, SLOT(navigateToUrl(QString))
            );
    qDebug() << "[MainWindow]" << "[Constructor] onTcpPrintNotified connected.";


    //statusbar
    statusBar = new QStatusBar(this);
    //statusBar->setMaximumHeight(20);
    setStatusBar(statusBar);
    progressBar = new QProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->setMaximumWidth(70);
    urlLabel = new QLabel("Location");
    printCounterLabel = new QLabel();
    notificatorStatusLabel = new QLabel();
    closeButton = new QPushButton();
    closeButton->setText("X");
    statusBar->addPermanentWidget(urlLabel);
    statusBar->addPermanentWidget(progressBar);
    statusBar->addPermanentWidget(printCounterLabel);
    statusBar->addPermanentWidget(notificatorStatusLabel);
    statusBar->addPermanentWidget(closeButton);
    qDebug() << "[MainWindow]" << "[init] Status bar created, widgets added.";

    setCentralWidget(webView);
    webView->show();
    webView->load(to_url);
    connectSlots();
    counterSetup();
    timerSetup();
    setPrinterNames();
    qDebug() << "[MainWindow]" << "[init] Ending.";
}

void MainWindow::setPrinterNames() {
    qDebug() << "[MainWindow]" << "[setPrinterNames] Beginning.";
    remotePrinterNames.clear();
    localPrinterNames.clear();
    localPrinterNames << ""; // entry for not fetching something from the server
    //qDebug() << "MainWindow::setPrinterNames()";
    QStringList groups = (*settings).childGroups();
    // filter out all setting groups that are not printer definitions
    for (int i = 0; i < groups.size(); i++) {
        QString group = groups[i];
        if (group.indexOf("printer") != -1) {
            remotePrinterNames << group;
        }
    }
#ifdef LINUX
    qDebug() << "[MainWindow]" << "[setPrinterNames] LINUX is defined.";
    QStringList filters;
    filters << "ttyS*" << "ttyUSB*" << "lp*";
    QDir * devs = new QDir("/dev", "*", QDir::Name, QDir::System);
    (*devs).setNameFilters(filters);
    QStringList nodes;
    nodes = devs->entryList();
    foreach (QString node, nodes) {
        localPrinterNames << "/dev/" + node;
    }

    devs = new QDir("/dev/usb", "*", QDir::Name, QDir::System);
    (*devs).setNameFilters(filters);
    nodes = devs->entryList();
    foreach (QString node, nodes) {
        localPrinterNames << "/dev/usb/" + node;
    }

#endif
#ifdef WINDOWS
    qDebug() << "[MainWindow]" << "[setPrinterNames] WIN32 is defined.";
    QList<QPrinterInfo> printer_list = QPrinterInfo::availablePrinters();
    for (int i = 0; i < printer_list.length(); i++) {
        QPrinterInfo info = printer_list.at(i);
        localPrinterNames << info.printerName();
    }
    localPrinterNames << "COM1";
    localPrinterNames << "COM2";
    localPrinterNames << "COM3";
    localPrinterNames << "COM4";
    localPrinterNames << "COM5";
    localPrinterNames << "COM6";
    localPrinterNames << "COM7";
    localPrinterNames << "COM8";

#endif
#ifdef MAC
    qDebug() << "[MainWindow]" << "[setPrinterNames] MAC is defined.";
    int i;
    cups_dest_t *dests, *dest;
    int num_dests = cupsGetDests(&dests);
    for (i = num_dests, dest = dests; i > 0; i--, dest++) {
        localPrinterNames << dest->name;
     }
#endif
    qDebug() << "MainWindow::setPrinterNames():" << localPrinterNames;
    qDebug() << "[MainWindow]" << "[setPrinterNames] Ending.";
}

void MainWindow::shutdown() {
    int pid = getpid();
    qDebug() << "Shutdown called.";
    QApplication::closeAllWindows();
#ifdef LINUX
    qDebug() << "[MainWindow]" << "[shutdown] LINUX defined, so using kill().";
    //QApplication::closeAllWindows();
    QCoreApplication::quit();
    //kill(pid,SIGKILL); // to make really sure
#endif
}

void MainWindow::connectSlots() {
    qDebug() << "[MainWindow]" << "[connectSlots] Beginning";
    connect(webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this,SLOT(addJavascriptObjects()));
    connect(webView->page(), SIGNAL(windowCloseRequested()), this,SLOT(windowCloseRequested()));
    connect(webView, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
    connect(webView, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
    connect(page->mainFrame(),SIGNAL(javaScriptWindowObjectCleared()),page,SLOT(resetJsErrors()));
    connect(page,SIGNAL(unsupportedContent(QNetworkReply*)),page,SLOT(downloadFile(QNetworkReply*)));
    connect(page,SIGNAL(addWidget(QWidget*)),this,SLOT(addStatusBarWidget(QWidget*)));
    connect(page,SIGNAL(removeWidget(QWidget*)),this,SLOT(removeStatusBarWidget(QWidget*)));
    connect(page,SIGNAL(fileProgressUpdated(int)),this,SLOT(setProgress(int)));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(shutdown()));
    qDebug() << "[MainWindow]" << "[connectSlots] main slots are now connected.";

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
        qDebug() << "[MainWindow]" << "[connectSlots] MAC is defined, so using different modifier keys.";
        QShortcut *showOptions = new QShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_O), this);
        connect( showOptions, SIGNAL(activated()), this, SLOT(showOptionsDialog()));
    #else
        QShortcut *showOptions = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this);
        connect( showOptions, SIGNAL(activated()), this, SLOT(showOptionsDialog()));
    #endif
    qDebug() << "[MainWindow]" << "[connectSlots] Keyboard Shortcuts defined.";
    qreal z = settings->value("zoomFactor").toReal();
    if (z) {
        qDebug() << "[MainWindow]" << "[connectSlots] z is set, setZoomFactor.";
      webView->page()->mainFrame()->setZoomFactor(z);
    }
}

QWebView* MainWindow::getWebView() {
    return webView;
}

void MainWindow::incZoom() {
  qDebug() << "[MainWindow]" << "[incZoom] Beginning";
  qreal z = webView->page()->mainFrame()->zoomFactor();
  z = z + 0.05;
  webView->page()->mainFrame()->setZoomFactor(z);
  settings->setValue("zoomFactor",z);
  qDebug() << "[MainWindow]" << "[incZoom] Ending.";
}

void MainWindow::decZoom() {
  qreal z = webView->page()->mainFrame()->zoomFactor();
  z = z - 0.05;
  webView->page()->mainFrame()->setZoomFactor(z);
  settings->setValue("zoomFactor",z);
  qDebug() << "[MainWindow]" << "[decZomm] Ending.";
}

void MainWindow::addStatusBarWidget(QWidget *w) {
    qDebug() << "[MainWindow]" << "[addStatusBarWidget] Beginning.";
    statusBar->addWidget(w);
    qDebug() << "[MainWindow]" << "[addStatusBarWidget] Ending.";
}

void MainWindow::removeStatusBarWidget(QWidget *w) {
    qDebug() << "[MainWindow]" << "[removeStatusBarWidget] Beginning";
    statusBar->removeWidget(w);
    qDebug() << "[MainWindow]" << "[removeStatusBarWidget] Ending.";
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
    qDebug() << "[MainWindow]" << "[executeJS]  executing: " << js;
    webView->page()->mainFrame()->evaluateJavaScript(js);
}

void MainWindow::adjustTitle() {
    qDebug() << "[MainWindow]" << "[adjustTitle] Beginning";
     if (progress <= 0 || progress >= 100)
         setWindowTitle(webView->title());
     else
         setWindowTitle(QString("%1 (%2%)").arg(webView->title()).arg(progress));
     progressBar->setValue(progress);
     urlLabel->setText(webView->url().encodedHost());

     qDebug() << "[MainWindow]" << "[adjustTitle] Ending.";
}

void MainWindow::navigateToUrl(QString url) {
    qDebug() << "[MainWindow]" << "[navigateToUrl] Beginning.";
    QUrl qurl;
    qurl = QUrl(url);
    webView->load(qurl);
    qDebug() << "[MainWindow]" << "[navigateToUrl] Ending.";
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
    qDebug() << "[MainWindow]" << "[addJavascriptObjects] Beginning";
    if (!shown) {
       shown = true; // i.e. this is ready
    }
    attach();
    qDebug() << "[MainWindow]" << "[addJavascriptObjects] Ending.";
}

void MainWindow::repaintViews() {
    qDebug() << "[MainWindow]" << "[repaintViews] Beginning.";
    webView->update();
    qDebug() << "[MainWindow]" << "[repaintViews] Ending.";
}

void MainWindow::attach(){
    //webView->page()->mainFrame()->addToJavaScriptWindowObject("SalorPrinter", sp);
    qDebug() << "[MainWindow]" << "[attach] Attaching SalorJsApi to main window object.";
    webView->page()->mainFrame()->addToJavaScriptWindowObject("Salor", js);
    page->resetJsErrors();
    qDebug() << "[MainWindow]" << "[attach] Ending.";
}

void MainWindow::windowCloseRequested() {
    qDebug() << "[MainWindow]" << "[windowCloseRequested] Beginning.";
    QApplication::closeAllWindows();
}

void MainWindow::showOptionsDialog() {
    qDebug() << "[MainWindow]" << "[showOptionsDialog] Beginning.";
    optionsDialog->show();
    optionsDialog->init();
    qDebug() << "[MainWindow]" << "[showOptionsDialog] Ending.";
}

void MainWindow::timerSetup() {
    qDebug() << "[MainWindow]" << "[timerSetup] Beginning.";
    mainTimer = new QTimer(this);
    mainTimer->start(1000);
    connect(
        mainTimer, SIGNAL(timeout()),
        this, SLOT(timerTimeout())
    );
    qDebug() << "[MainWindow]" << "[timerSetup] Ending.";
}

void MainWindow::onTcpPrintNotified() {
    //qDebug() << "MainWindow::onTcpPrintNotified";
    //If Server has initiated printing, wait for another full printing polling interval
    qDebug() << "[MainWindow]" << "[onTcpPrintNotified] called.";
    counterPrint = intervalPrint;
}

void MainWindow::counterSetup() {
    qDebug() << "[MainWindow]" << "[counterSetup] Beginning.";
    settings->beginGroup("printing");
    intervalPrint = settings->value("interval").toInt();
    settings->endGroup();
    intervalTcp = 10;

    if (intervalPrint < 10) intervalPrint = 10; // security measure
    counterPrint = intervalPrint;

    counterTcp = intervalTcp;
    qDebug() << "[MainWindow]" << "[counterSetup] Ending.";
}

void MainWindow::timerTimeout() {
    //qDebug() << "[MainWindow]" << "[timerTimeout] Beginning.";
    QString localprinter;
    QString printurl;
    QString mainurl;
    QDateTime datetime = QDateTime::currentDateTime();
    //qDebug() << datetime.toString("yyyyMMddHHmmss");
    //qDebug() << "MainWindow::timerTimeout(): intervalPrint is " << intervalPrint;
    counterPrint--;
    counterTcp--;

    if (counterPrint == 0) {
        counterPrint = intervalPrint;

        settings->beginGroup("printing");
        mainurl = settings->value("url").toString();
        settings->endGroup();

        // strip basename
        int pos = 0;
        QRegExp rx("(htt.*\://.*)/");
        pos = rx.indexIn(mainurl);
        mainurl = rx.cap(1);

        if (mainurl != "") {

            foreach(QString remoteprinter, remotePrinterNames) {
                settings->beginGroup(remoteprinter);
                localprinter = settings->value("localprinter").toString();
                printurl = mainurl + settings->value("url").toString() + "?" + datetime.toString("yyyyMMddHHmmss");
                settings->endGroup();

                if (localprinter == "") {
                    //qDebug() << "Not fetching anything for local printer" << remoteprinter;
                    // don't print when localprinter combo box has been left empty
                } else {
                    SalorPrinter *printer = new SalorPrinter(this, networkManager, localprinter);
                    printer->printURL(printurl);
                    // printer instance will delete itself after printing. we cannot do it here since long running network requests are involved.
                }
            }

        }
    }

    if (counterTcp == 0) {
        counterTcp = intervalTcp;

        mainurl = settings->value("url").toString();
        if (mainurl != "") {
            //notificatorStatusLabel->setText(QString::number(salorNotificator->currentState));
            if (salorNotificator->currentState == 0) {
                //QAbstractSocket::UnconnectedState
                salorNotificator->start();
                //salorNotificator->writestuff("text\n");// do nothing
            }
            if (salorNotificator->currentState == 3) {
                settings->beginGroup("printing");
                QString username = settings->value("username").toString();
                settings->endGroup();
                salorNotificator->writeToSocket("PING|" + username + "|" + QString::number(getpid()));
            }
        }
    }

    printCounterLabel->setText(QString::number(counterPrint));
    //qDebug() << "[MainWindow]" << "[timerTimeout] Ending.";
}

void MainWindow::setPrinterCounter(int value){
    counterPrint = value;
}

void MainWindow::updateNotificatorLabel(QAbstractSocket::SocketState state) {
    qDebug() << "[MainWindow]" << "[updateNotificatorLabel] Start.";
    notificatorStatusLabel->setText(QString::number(state));
}
