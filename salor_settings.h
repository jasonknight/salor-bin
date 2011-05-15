#ifndef SALORSETTINGS_H
#define SALORSETTINGS_H

#include <qobject.h>
#include <qsettings.h>
#include <qwebframe.h>
#include <qdebug.h>
#include <qapplication.h>
class QWebView;
class SalorSettings : public QObject
{
Q_OBJECT
    static SalorSettings* _self;

public:
    SalorSettings(QObject *parent = 0);
    ~SalorSettings() { delete iniSettings;}
    void init();
    //class variables
    static SalorSettings* getSelf ();
    static void appSetup(QString name) { appName = name; qDebug() << "AppName is:" << name; }

    //instance variables
    QString dataDirectory;
    QString cwd;
    QString sqliteDatabaseLocation;
    void attach(QWebView *view);
    static QString appName;
    QSettings* iniSettings;
    QString iniFile;
    QApplication* application;
signals:

public slots:
    void setIniFile(QString fname);
    QVariant getValue(QString key);
    bool setValue(QString key, QString value);
    QString parseIniValuesInQString(QString text);
private:
    QWebFrame *frame;
};

#endif // SALORSETTINGS_H
