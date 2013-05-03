#ifndef SALORPAGE_H
#define SALORPAGE_H

#include <QWebPage>
#include <QObject>
#include <QDebug>
#include <QNetworkRequest>
#include <QProcess>
#include <QNetworkReply>
#include "salorcapture.h"

class MainWindow;
class SalorCapture;

class SalorPage : public QWebPage
{
    Q_OBJECT
public:
    explicit SalorPage(QObject *parent = 0);
    MainWindow * main;
    int js_error_count;
    void setAttribute(QWebSettings::WebAttribute option, const QString& value);
    void setUserAgent(const QString& userAgent);
    void setAlertString(const QString& alertString);
    void setPrintAlerts(bool printAlerts);
    void setSalorCapture(SalorCapture* SalorCapture);
    QString getAlertString();

protected:
    virtual void javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID  );
    void javaScriptAlert(QWebFrame* frame, const QString& msg);
    bool javaScriptPrompt(QWebFrame* frame, const QString& msg, const QString& defaultValue, QString* result);
    bool javaScriptConfirm(QWebFrame* frame, const QString& msg);
    bool mPrintAlerts;
    QString userAgentForUrl(const QUrl& url) const;
    QString chooseFile(QWebFrame *frame, const QString& suggestedFile);
    QString mUserAgent;
    QString mAlertString;
    SalorCapture* mSalorCapture;
    
signals:
    void fileProgressUpdated(int);
    void addWidget(QWidget *);
    void removeWidget(QWidget *);
    
public slots:
    void resetJsErrors();
    void downloadFile(QNetworkRequest request);
    void downloadFile(QNetworkReply * reply);
    void updateFileProgress(qint64 read,qint64 total);
    void bubbleAddWidget(QWidget * w) { qDebug() << "Bubbling add widget"; emit addWidget(w);}
    void bubbleRemoveWidget(QWidget *w) { emit removeWidget(w);}
    
};

#endif // SALORPAGE_H
