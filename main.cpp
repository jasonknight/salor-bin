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


// these global variables must be included in a C file once, otherwise "ld error: undefined reference to"
const QString PathCookies = PathWorking + "/cookiejar";
const QString PathCache = PathWorking + "/cache";
#ifdef LINUX
const QString PathSettings = "/etc/salor-bin/salor-bin.ini";
const QString PathLog = "/var/log/salor-bin/salor-bin.log";
#endif
#ifdef WIN32
const QString PathSettings = PathWorking + "/salor-bin.ini";
const QString PathLog = PathWorking + "/salor-bin.log";
#endif
#ifdef MAC
const QString PathSettings = PathWorking + "/salor-bin.ini";
const QString PathLog = PathWorking + "/salor-bin.log";
#endif
const QString PathDownloads = PathWorking + "/downloads";
QStringList remotePrinterNames;
QStringList localPrinterNames;
QSettings *settings = new QSettings(PathSettings, QSettings::IniFormat);


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

void myMessageOutput(QtMsgType type, const char *msg)
 {
    QFile f(PathLog);
    f.open(QIODevice::Append);
    QDataStream out(&f);
    out << msg << "\n";
    f.close();

     //in this function, you can write the message to any stream!
     switch (type) {
     case QtDebugMsg:
         fprintf(stderr, "Debug: %s\n", msg);
         break;
     case QtWarningMsg:
         fprintf(stderr, "Warning: %s\n", msg);
         break;
     case QtCriticalMsg:
         fprintf(stderr, "Critical: %s\n", msg);
         break;
     case QtFatalMsg:
         fprintf(stderr, "Fatal: %s\n", msg);
         abort();
     }
 }


int main(int argc, char *argv[])
{
    qInstallMsgHandler(myMessageOutput);
    QApplication a(argc, argv);
    MainWindow w;
    QString arg;
    bool fullscreen = false;
    QVariant url;

    // go to default URL if none specified in .ini file
    url = _get("url");
    if (url.isNull() != true && url.toString() != "") {
        w.to_url = url.toString();
    } else {
        w.to_url = "http://documentation.red-e.eu";
    }

    for (int i = 1; i < argc; i++) {
        arg = QString(argv[i]);

        if (arg == "-u") {
            qDebug() << "Setting to_url to: " << argv[i + 1];
            w.to_url = QString(argv[i + 1]);
        } else if (arg == "-h") {
            help();
        } else if (arg == "-w") {
            qDebug() << "[main.cpp]" << " Setting fullscreen to true";
            fullscreen = true;
        } else if (arg == "--customerscreenid") {
            // acts as a customer screen
            w.customerScreenId = QString(argv[i + 1]);
        }
    }

    qDebug() << "[main.cpp]" << " Initializing w.";
    w.init();

    qDebug() << "[main.cpp]" << " Initialization complete.";
    if (fullscreen == true || settings->value("kiosk").toString() == "true") {
        qDebug() << "[main.cpp]" << " Showing fullscreen.";
        w.showFullScreen();
    } else {
        qDebug() << "[main.cpp]" << " Just showing.";
        w.show();
    }

    qDebug() << "[main.cpp]" << " Display Complete.";

#ifdef LINUX
    signal(SIGCHLD, SIG_IGN);
    qDebug() << "[main.cpp]" << " Linux Signal Handler attached.";
#endif
    qDebug() << "[main.cpp]" << " calling a.exec().";
    return a.exec();
}
