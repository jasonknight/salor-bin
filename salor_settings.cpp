#include "salor_settings.h"
#include <QtDebug>
#include <QDir>
#include <QDesktopServices>
#include <QSettings>
#include <QVariant>
#include <qwebview.h>
#include <qwebframe.h>
#include <qwebpage.h>
SalorSettings* SalorSettings::_self = NULL;
QString SalorSettings::appName = "";

SalorSettings::SalorSettings(QObject *parent) :
    QObject(parent) {
}
SalorSettings* SalorSettings::getSelf() {
    if (_self == NULL) {
        _self = new SalorSettings();
        _self->init();
    }
    return _self;
}
void SalorSettings::init() {
    //Ideally, this should be read once, and from then on, it should be using the file
    //we'll be creating with the code below.
    this->iniFile = "salor.ini"; //Use app's executable name for ini file.
    //Default ini file has fallback:
    QString defIniFile = QFile::exists(this->iniFile) ? this->iniFile : "salor.ini";
    qDebug() << "Default ini file: " << defIniFile;
    QSettings s(defIniFile, QSettings::IniFormat);
    this->cwd = QDir::currentPath();
    this->dataDirectory = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    if (s.value("useLocalStorage").toBool() != true) {
        qDebug() << "Using app ini file, not local...";
        goto finish;
    }
    //On the off chance DataLocation is empty...
    if (this->dataDirectory.isEmpty()) {
        qDebug() << "Step 1 Error: dataDirectory does not exist...";
        this->dataDirectory = QDir::homePath() + "/" + appName;
    }
    this->dataDirectory = this->dataDirectory + "/" + appName;
    //Making sure data location exists.
    if (!QFile::exists(this->dataDirectory)) {
        QDir d;
        d.mkpath(this->dataDirectory);
    }
    //Add path for local ini file, file and path always named after app name.
    this->iniFile = this->dataDirectory + "/" + this->iniFile;
    if (!QFile::exists(this->iniFile)) {
        QFile afile(defIniFile);
        QFile ufile(this->iniFile);
        if (ufile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream to(&ufile);
            if (afile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QString contents = afile.readAll();
                to << contents << endl;
                ufile.close();
                afile.close();
            } else {
                ufile.close();
                this->iniFile = QFile::exists(defIniFile) ? defIniFile : ":/salor.ini";
            }
        } else {
            this->iniFile = QFile::exists(defIniFile) ? defIniFile : ":/salor.ini";
        }
    }
    finish:
    qDebug() << "Local ini file: " << this->iniFile;
    this->iniSettings = new QSettings(this->iniFile, QSettings::IniFormat);
    this->iniSettings->setValue("cwd",this->cwd);
}
void SalorSettings::setIniFile(QString fname) {
    this->iniSettings = new QSettings(fname, QSettings::IniFormat);
}

QVariant SalorSettings::getValue(QString key) {
    QVariant value = this->iniSettings->value(key);
    QString tv = value.toString();
    QString tv2 = this->parseIniValuesInQString(tv);
    if (tv != tv2) {
        value = QVariant(tv2);
    }
    return value;
}
bool SalorSettings::setValue(QString key, QString value) {
    this->iniSettings->setValue(key, value);
    return true;
}
QString SalorSettings::parseIniValuesInQString(QString text) {
    //This is where we get the ability to use ini vars in our
    //pages without much effort. just put: ${{ini_value_name}}
    QRegExp r("\\$\\{\\{([a-zA-Z_0-9]+)\\}\\}");
    int pos = 0;
    QVariant v;
    while ((pos = r.indexIn(text,pos)) != -1) {
        v = getValue(r.cap(1));
        if (!v.isNull()) {
            text.replace("${{"+r.cap(1)+"}}", v.toString());
        }
        pos += r.matchedLength();
    }
    return text;
}
void SalorSettings::attach(QWebView *view){
    QWebPage *page = view->page();
    frame = page->mainFrame();
    frame->addToJavaScriptWindowObject(QString("SalorSettings"),this);
}
