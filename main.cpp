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
#ifdef LINUX
const QString PathCookies = "/var/lib/salor-bin/cookiejar";
const QString PathCache = "/var/cache/salor-bin";
const QString PathSettings = "/etc/salor-bin/salor-bin.ini";
const QString PathLog = "/var/log/salor-bin/salor-bin.log";
#endif
#ifdef WIN32
const QString PathCookies = PathWorking + "/cookiejar";
const QString PathCache = PathWorking + "/cache";
const QString PathSettings = PathWorking + "/salor-bin.ini";
const QString PathLog = PathWorking + "/salor-bin.log";
#endif
#ifdef MAC
const QString PathCookies = PathWorking + "/cookiejar";
const QString PathCache = PathWorking + "/cache";
const QString PathSettings = PathWorking + "/salor-bin.ini";
const QString PathLog = PathWorking + "/salor-bin.log";
#endif

const QString PathDownloads = PathWorking + "/downloads";
QStringList remotePrinterNames;
QStringList localPrinterNames;
QSettings *settings = new QSettings(PathSettings, QSettings::IniFormat);


void help() {
    return;
}

void myMessageOutput(QtMsgType type, const char *msg)
 {
    QFile f(PathLog);
    f.open(QIODevice::Append);
    QDataStream out(&f);

    QDateTime datetime = QDateTime::currentDateTime();
    QString timestamp = datetime.toString("yyyyMMddHHmmss");

    out << timestamp << msg << "\n";

    /*
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
     */

     f.close();
 }

void purgeLogfile() {
    qDebug() << "Truncating Logfile";
    QFile f(PathLog);
    f.open(QFile::WriteOnly|QFile::Truncate);
    f.close();
}


int main(int argc, char *argv[])
{
    qInstallMsgHandler(myMessageOutput);
    QApplication a(argc, argv);
    MainWindow w;
    QString arg;
    bool fullscreen = false;
    QVariant url;

#ifdef WINDOWS
    purgeLogfile();
#endif

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
