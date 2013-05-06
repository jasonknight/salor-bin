#include "salorpage.h"
#include "downloader.h"
#include "salorcapture.h"
#include "common_includes.h"

SalorPage::SalorPage(QObject *parent) :
    QWebPage(parent)
{
    //mwindow = parent;
    js_error_count = 0;
}


void SalorPage::resetJsErrors() {
    //qDebug() << "resetting js errors";
    js_error_count = 0;
}

void SalorPage::updateFileProgress(qint64 read,qint64 total) {
       int p = (int)((read / total) * 100);
       emit fileProgressUpdated(p);
}

void SalorPage::downloadFile(QNetworkRequest request) {
    qDebug() << "Other download called" << request.url().toString();
    QString default_file_name = QFileInfo(request.url().toString()).fileName();
    QString file_name = QFileDialog::getSaveFileName((QWidget*)this,tr("Save File"),default_file_name);
    if (file_name.isEmpty())
        return;
    QNetworkRequest new_request = request;
    new_request.setAttribute(QNetworkRequest::User,file_name);
    QNetworkAccessManager * mng = this->networkAccessManager();
    QNetworkReply *reply = mng->get(new_request);

}
void SalorPage::downloadFile(QNetworkReply *reply) {
    qDebug() << "SalorPage::downloadFile:" << reply->url().toString();
    QString default_file_name = QFileInfo(reply->url().toString()).fileName();
    qDebug() << "    " << default_file_name;
    QString file_name = QFileDialog::getSaveFileName(0, tr("Save File"),default_file_name);
    qDebug() << "    " << file_name;
    if (file_name.isEmpty())
        return;
    Downloader * d = new Downloader;
    d->file_name = file_name;
    connect(d,SIGNAL(fileProgressUpdated(int)),this->main,SLOT(setProgress(int)));
    connect(d,SIGNAL(finished()),d,SLOT(deleteLater()));
    connect(d,SIGNAL(addWidget(QWidget*)),this,SLOT(bubbleAddWidget(QWidget*)));
    connect(d,SIGNAL(removeWidget(QWidget*)),this,SLOT(bubbleRemoveWidget(QWidget*)));
    qDebug() << "    Calling setReply";
    d->setReply(reply);
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),d,SLOT(updateFileProgress(qint64,qint64)));
    d->main = this->main;
}

void SalorPage::javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID  ) {
    //QMessageBox::critical(0, QObject::tr("Critical Script Error"), QString() + "A Javascript error Occurred: '" + message + "'\nat line " + QString::number(lineNumber) + "\nin " + sourceID);
    if (this->js_error_count >= 1000) {
        return;
    }
    QString err = QString() + "A Javascript error Occurred: " + message + "'\nat line " + QString::number(lineNumber) + "\nin " + sourceID;

    this->js_error_count++;
    if (this->js_error_count > 30) {
        this->js_error_count = 1000;
        //this->setAttribute(QWebSettings::JavascriptEnabled,"off");
        //QMessageBox::critical(0, QObject::tr("Critical Script Error"), "Scripting is being stopped, there were more than 100 errors. please call tech support NOW to get this resolved. The errors have been logged.");
        return;
    }
    if (this->js_error_count == 10) {
        //QMessageBox::critical(0, QObject::tr("Critical Script Error"), "There have been too many errors, please call tech support now to get this resolved. The errors have been logged.");
    } else {
       // emit generalSnap(err);
    }

    qDebug() <<  "A Javascript error Occurred: " << err;
}

QString SalorPage::chooseFile(QWebFrame* /*frame*/, const QString& /*suggestedFile*/) {
  QString file_name = QFileDialog::getOpenFileName();
  if (!file_name.isEmpty()) {
    return file_name;
  }
  qDebug() << "Returing null from SalorPage::chooseFile";
  return QString::null;
}

bool SalorPage::javaScriptConfirm(QWebFrame* /*frame*/, const QString& /*msg*/) {
  return true;
}

bool SalorPage::javaScriptPrompt(QWebFrame* /*frame*/,
                           const QString& /*msg*/,
                           const QString& /*defaultValue*/,
                           QString* /*result*/) {
  return true;
}


void SalorPage::javaScriptAlert(QWebFrame* /*frame*/, const QString& msg) {
   qDebug() << "[alert]" << msg;

  if (mAlertString == msg) {
    // TODO: mSalorCapture
    //QTimer::singleShot(10, mSalorCapture, SLOT(Delayed()));
  }
}

QString SalorPage::userAgentForUrl(const QUrl& url) const {

  if (!mUserAgent.isNull())
    return mUserAgent;

  return QWebPage::userAgentForUrl(url);
}

void SalorPage::setUserAgent(const QString& userAgent) {
  mUserAgent = userAgent;
}

void SalorPage::setAlertString(const QString& alertString) {
  mAlertString = alertString;
}

QString SalorPage::getAlertString() {
  return mAlertString;
}

void SalorPage::setPrintAlerts(bool printAlerts) {
  mPrintAlerts = printAlerts;
}

void SalorPage::setAttribute(QWebSettings::WebAttribute option,
                       const QString& value) {

  if (value == "on")
    settings()->setAttribute(option, true);
  else if (value == "off")
    settings()->setAttribute(option, false);
  else
    (void)0; // TODO: ...
}


void SalorPage::setSalorCapture(SalorCapture* SalorCapture) {
  mSalorCapture = SalorCapture;
}
