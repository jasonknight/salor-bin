#include <QtGui/QApplication>
#include "mainwindow.h"
#include <iostream>
void help() {
    std::cout << "Usage:\n";
    std::cout << "\tsalor -[uh] [value]";
    std::cout << "Example: \n";
    std::cout << "\tsalor -u http://localhost:3000";
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    qDebug() << "Here I am";
    w.to_url = QString("http://salor");
    for (int i = 1; i < argc; i++) {
        if (i + 1 != argc) { // Check that we haven't finished parsing already
           qDebug() << "Arg is: " << argv[i];
           if (argv[i] == "-u") {
               w.to_url = QString(argv[i + 1]);
           } else if (argv[i] == "-h") {
               help();
               w.close();
               a.exit(0);
           }
       }
       std::cout << argv[i] << " ";
    }
    QPixmap pixmap(":/salor-splash.png");
    QSplashScreen sp(pixmap);
    sp.show();
    sp.showMessage("Loading salor");

    sp.showMessage("initializing");
    w.init();
    sp.showMessage("initialization completed, waiting for load");
    while (w.shown == false) {
        a.processEvents();
    }
    w.showMaximized();
    sp.finish(&w);
    return a.exec();
}

