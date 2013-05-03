#include "mainwindow.h"
#include "common_includes.h"

char * __path;
size_t __size;

#ifdef LINUX
    extern const QString PathWorking = QDir::homePath() + "/.salor-bin";
#endif

#ifdef WIN32
    extern const QString PathWorking = _getcwd(__path,__size);
#endif

#ifdef MAC
    extern const QString PathWorking = getcwd(__path,__size);
#endif

extern const QString PathCookies = PathWorking + "/cookiejar";
extern const QString PathCache = PathWorking + "/cache";
extern const QString PathSettings = PathWorking + "/settings";
extern const QString PathDownloads = PathWorking + "/downloads";

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
    QString arg;
    bool fullscreen = false;

    if (!QDir(PathWorking).exists()) {
        QDir().mkdir(PathWorking);
    }

    if (_get("salor.url").isNull() != true && _get("salor.url").toString() != "") {
        w.to_url = _get("salor.url").toString();
    } else {
        w.to_url = QString("http://documentation.red-e.eu");
    }

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
             fullscreen = false;
         }
      }
    }

    w.init();

    if (fullscreen) {
        w.showMaximized();
    } else {
        w.show();
    }

#ifdef LINUX
    signal(SIGCHLD, SIG_IGN);
#endif
    
    return a.exec();
}
