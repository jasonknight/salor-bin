#include <QtGui/QApplication>
#include "mainwindow.h"
#include <iostream>
#include <signal.h>

void help() {
    std::cout << "Usage:\n";
    std::cout << "\tsalor -[uh] [value]\n";
    std::cout << "Example: \n";
    std::cout << "\tsalor -u http://localhost:3000\n";
    std::cout << "\tRunning salor with no arguments will load http://salor\n";
    std::cout << "\tOther arguments could be: salor -u http://salortrainer\n";
    exit(0);
    return;
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.to_url = QString("http://salor-retail/orders/new");
    if (_get("salor.url").isNull() != true && _get("salor.url").toString() != "") {
        w.to_url = _get("salor.url").toString();
    } else {
        w.to_url = QString("http://sr.red-e.eu");
    }
    QString arg;
    bool fs = true;
    for (int i = 1; i < argc; i++) {
      arg = QString(argv[i]);
      if (arg == "-h") {
        help();
      }
      if (i + 1 != argc) { // Check that we haven't finished parsing already
         if (arg == "-u") {
           qDebug() << "Setting to_url to: " << argv[i + 1];
             w.to_url = QString(argv[i + 1]);
         } else if (arg == "-h") {
             help();
         } else if (arg == "-w") {
             fs = false;
         }
      }
    }
    //QPixmap pixmap(":/salor-splash.png");
    //QSplashScreen sp(pixmap);
   // sp.show();
   // sp.showMessage("Loading salor");

   // sp.showMessage("initializing");
    w.init();
    //sp.showMessage("initialization completed, waiting for load");
    //while (w.shown == false) {
    //    a.processEvents();
    //}
    if (fs) {
      #ifdef LINUX
        w.showFullScreen();
      #else
        w.showMaximized();
      #endif
    } else {
      w.show();
    }
    //sp.finish(&w);
#ifdef Q_OS_LINUX
    signal(SIGCHLD, SIG_IGN);
#endif
    return a.exec();
}

