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
    w.to_url = QString("http://salor/orders/new");
    QSettings settings("JolieRouge", "Salor");
    int pid = settings.value("pid").toInt();
    int m_pid = getppid();
    qDebug() << "PID is: " << QString::number(pid);
    qDebug() << "PID is: " << QString::number(m_pid);
    if (pid != 0 && pid != m_pid) {
        qDebug() << "PIDS don't match" << QString::number(m_pid) << " != " << QString::number(pid);
        #ifdef LINUX
            kill(pid,9);
        #endif

    }
    settings.setValue("pid",m_pid);
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
    if (fs) {
      w.showFullScreen();
    } else {
      w.show();
    }
    sp.finish(&w);
    signal(SIGCHLD, SIG_IGN);

    return a.exec();
}

