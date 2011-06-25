#include <QtGui/QApplication>
#include "mainwindow.h"
#include "salor_settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "Loading Splash Screen";
    QPixmap pixmap(":/salor-splash.png");
    QSplashScreen sp(pixmap);
    sp.show();
    sp.showMessage("Loading salor");
    SalorSettings::getSelf()->application = &a;
    MainWindow w;
    sp.showMessage("initializing");
    w.init();
    sp.showMessage("initialization completing, waiting for load");
    while (w.shown == false) {
        a.processEvents();
    }
    w.showMaximized();
    sp.finish(&w);
    return a.exec();
}
