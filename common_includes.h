// Uncomment one of these before you compile
//#define LINUX
//#define ANDROID
//#define WINDOWS
#define MAC
#ifndef COMMON_INCLUDES_H
#define COMMON_INCLUDES_H

// Linux based includes

#ifdef LINUX
#include <termios.h> /* POSIX terminal control definitions */
#include <linux/stat.h>
#endif

#ifdef WINDOWS

#endif

#ifdef MAC
    #include    <cups/cups.h>
#endif

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

static QVariant _get(QString key) {
    QSettings settings("JolieRouge", "Salor");
    QVariant val = settings.value(key);
    if (!val.isNull()) {
        return val;
    } else {
        return QVariant();
    }
}

static void _set(QString key, QString &value) {
    QSettings settings("JolieRouge", "Salor");
    settings.setValue(key,value);
}
static void _set(QString key, int value) {
    QSettings settings("JolieRouge", "Salor");
    settings.setValue(key,value);
}
static void _set(QString key, qreal value) {
    QSettings settings("JolieRouge", "Salor");
    settings.setValue(key,value);
}

#endif // COMMON_INCLUDES_H
