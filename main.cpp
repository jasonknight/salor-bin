#include <QtGui/QApplication>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap pixmap(":/salor-splash.png");
    QSplashScreen sp(pixmap);
    sp.show();
    sp.showMessage("Loading salor");
    MainWindow w;
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
