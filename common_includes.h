// Uncomment one of these before you compile


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
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <QSettings>
    #include <QClipboard>
    #include <QVariant>
    #include <QDir>
    #include <QDebug>
    #include <QDesktopServices>
    #include <QDirIterator>
    #include <QScopedPointer>
    #ifdef Q_WS_X11
        #define LINUX
    #endif
    //#define ANDROID
    #ifdef Q_WS_WIN
        #define WINDOWS
    #endif
    #ifdef Q_WS_MACX
        #define MAC
    #endif
    extern char * __path;
    extern size_t __size;
    // Linux based includes

    #ifdef LINUX
        #include <termios.h> /* POSIX terminal control definitions */
        #include <linux/stat.h>
    #endif

    #ifdef WINDOWS
        #include <windows.h>
    #endif

    #ifdef MAC
        #include <cups/cups.h>
    #endif
    extern const QString PathWorking;
    extern const QString PathCache;
    extern const QString PathSettings;
    extern const QString PathDownloads;


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

    // Global Variables


    // Macro Definitions

    #define PROP(type,name) \
       public: \
        type Get##name() { return prop_##name;} \
        void Set##name(type value) { prop_##name = value; }; \
       private: \
        type prop_##name;
    #define GET(name) Get##name()
    #define SET(name,value) Set##name(value)
    #define bug(string) \
        qDebug() << "##string"
#endif // COMMON_INCLUDES_H
