#ifndef SALOR_PAGE_H
#define SALOR_PAGE_H
#include <QWebPage>
#include <QObject>
#include <QDebug>
class SalorCapture;
class SalorPage:public QWebPage
{
    Q_OBJECT
public:
    SalorPage(QObject* parent = 0);
    void setAttribute(QWebSettings::WebAttribute option, const QString& value);
    void setUserAgent(const QString& userAgent);
    void setAlertString(const QString& alertString);
    void setPrintAlerts(bool printAlerts);
    void setSalorCapture(SalorCapture* SalorCapture);
    int js_error_count;
    QString getAlertString();
public slots:
    void resetJsErrors() {
        //qDebug() << "resetting js errors";
        this->js_error_count = 0;
    }

signals:
    void generalSnap(QString);
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
};
#endif // SALOR_PAGE_H

#ifndef SALOR_CAPT_H
#define SALOR_CAPT_H
class SalorCapture : public QObject {
  Q_OBJECT

public:
  SalorCapture( SalorPage* page,
                const QString& output,
                int delay,
                const QString& scriptProp,
                const QString& scriptCode
               );

public slots:
  void DocumentComplete(bool ok);
  void DocumentPrint();
  void InitialLayoutCompleted();
  void JavaScriptWindowObjectCleared();
  void Timeout();
  void Delayed();

private:
  void TryDelayedRender();
  void saveSnapshot();
  bool mSawInitialLayout;
  bool mSawDocumentComplete;

protected:
  int          mDelay;
  SalorPage*   mPage;
  QObject*     mScriptObj;
  QString      mScriptProp;
  QString      mScriptCode;
  QString      mOutput;
};
#endif // SALOR_CAPT_H
