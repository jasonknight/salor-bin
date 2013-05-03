#ifndef COMMON_INCLUDES_H
#define COMMON_INCLUDES_H

#include <stdlib.h>
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <stdio.h>   /* Standard input/output definitions */
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <QApplication>
#include <QMainWindow>
#include <QtGui>
#include <QObject>
#include <QtGlobal>
#include <QSettings>

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QSysInfo>

#include <QDebug>
#include <QDesktopServices>

#include <QScopedPointer>
#include <QDateTime>

#include <QWebView>
#include <QWebFrame>
#include <QWebPage>
#include <QtWebKit>

#include <QPrinter>
#include <QPrintDialog>
#include <QPrinterInfo>

#include <QTimer>
#include <QTextStream>
#include <QVariant>
#include <QByteArray>
#include <QMap>
#include <QProcess>
#include <QGroupBox>
#include <QVBoxLayout>

#include <QStatusBar>
#include <QBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QClipboard>
#include <QShortcut>
#include <QDesktopServices>
#include <QList>

#include <QNetworkDiskCache>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkCookie>
#include <QNetworkCookieJar>

#include <QSize>
#include <QFont>


#include <QMessageBox>


#ifdef Q_OS_LINUX
   #define LINUX
#endif
#ifdef WIN32
    #define WINDOWS
#endif


#ifdef LINUX
    #include <termios.h> /* POSIX terminal control definitions */
    #include <linux/stat.h>
#endif
#ifdef WIN32
    #include <windows.h>
#endif
#ifdef MAC
    #include <cups/cups.h>
#endif


extern char * __path;
extern size_t __size;

extern const QString PathWorking;
extern const QString PathCache;
extern const QString PathSettings;
extern const QString PathDownloads;
extern const QString PathCookies;


static QVariant _get(QString key) {
    QSettings settings(PathSettings,QSettings::IniFormat);
    QVariant val = settings.value(key);
    if (!val.isNull()) {
        return val;
    } else {
        return QVariant();
    }
}

static void _set(QString key, QString &value) {
    QSettings settings(PathSettings,QSettings::IniFormat);
    settings.setValue(key,value);
}
static void _set(QString key, int value) {
    QSettings settings(PathSettings,QSettings::IniFormat);
    settings.setValue(key,value);
}
static void _set(QString key, qreal value) {
    QSettings settings(PathSettings,QSettings::IniFormat);
    settings.setValue(key,value);
}


#define GET(name) Get##name()
#define SET(name,value) Set##name(value)
#define bug(string) qDebug() << "##string"

#endif // COMMON_INCLUDES_H
