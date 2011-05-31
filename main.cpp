#include <QtGui/QApplication>
#include "mainwindow.h"
#include "salor_settings.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    SalorSettings::getSelf()->application = &a;
    MainWindow w;
    w.init();
    w.showMaximized();
    w.linkClicked(QUrl(
                SalorSettings::getSelf()->getValue("start").toString()
              )
         );

    return a.exec();
}
