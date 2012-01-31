#include "salor_page.h"
#include <QDebug>
#include <QMessageBox>
#include "salor_plugin_factory.h"
#include <QTimer>
#include <QtWebKit>
#include <QtGui>
#include <QTimer>
#include <QByteArray>
#include <QNetworkRequest>
#include <QProcess>
#include "salorprocess.h"

SalorPage::SalorPage(QObject* parent):QWebPage(parent)
{
    //SalorPluginFactory* wpf = new SalorPluginFactory(this);

    //this->setPluginFactory(wpf);
}
void SalorPage::javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID  ) {
    QMessageBox::critical(0, QObject::tr("Critical Script Error"), QString() + "A Javascript error Occurred: '" + message + "'\nat line " + QString::number(lineNumber) + "\nin " + sourceID);
    qDebug() <<  "A Javascript error Occurred: " << message << "\nat line " << QString::number(lineNumber) << "\nin " << sourceID;
}
QString SalorPage::chooseFile(QWebFrame* /*frame*/, const QString& /*suggestedFile*/) {
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
    QTimer::singleShot(10, mSalorCapture, SLOT(Delayed()));
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

void SalorPage::setSalorCapture(SalorCapture* SalorCapture) {
  mSalorCapture = SalorCapture;
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

/* Salor Capt Definitions */

SalorCapture::SalorCapture(SalorPage* page, const QString& output, int delay,
                   const QString& scriptProp, const QString& scriptCode) {
  mPage = page;
  mOutput = output;
  mDelay = delay;
  mSawInitialLayout = false;
  mSawDocumentComplete = false;
  mScriptProp = scriptProp;
  mScriptCode = scriptCode;
  mScriptObj = new QObject();

 // qDebug() << "In SalorCapture";
  mPage->setSalorCapture(this);

  //qDebug() << "setSalorCapture done";
}

void SalorCapture::InitialLayoutCompleted() {
    qDebug() << "in InitialLayoutCompleted";
  mSawInitialLayout = true;

  if (mSawInitialLayout && mSawDocumentComplete)
    TryDelayedRender();
}

void SalorCapture::DocumentComplete(bool /*ok*/) {
  //qDebug() << "In DocumentComplete";

  saveSnapshot();
  return;
  mSawDocumentComplete = true;

  if (mSawInitialLayout && mSawDocumentComplete)
    TryDelayedRender();
}

void SalorCapture::JavaScriptWindowObjectCleared() {

}

void SalorCapture::TryDelayedRender() {
   // qDebug() << "TryDelayRender called";
  if (!mPage->getAlertString().isEmpty())
    return;

  if (mDelay > 0) {
    QTimer::singleShot(mDelay, this, SLOT(Delayed()));
    return;
  }

  saveSnapshot();
}

void SalorCapture::Timeout() {
  saveSnapshot();
}

void SalorCapture::Delayed() {
  saveSnapshot();
}
void SalorCapture::saveSnapshot() {
    //qDebug() << "saveSnapshot was called";
    QWebFrame *mainFrame = mPage->mainFrame();

    QSize size(800,480);
    //mainFrame->contentsSize(size);

    mPage->setViewportSize(size); //mainFrame->contentsSize()
    QImage image(size, QImage::Format_ARGB32); // mPage->viewportSize()

    QPainter painter;
    painter.begin(&image);
    mainFrame->render(&painter);
    painter.end();

    // Here is where we hook in.
    //qDebug() << "Saving to: " << mOutput;
    image.save(mOutput, "bmp");
    SalorProcess *sp = new SalorProcess(this);
    sp->run("pole-dancer","-dlo " + mOutput);
    emit deleteLater();
    //display_link_write_image(mOutput.toAscii());
}
void SalorCapture::DocumentPrint(bool ok) {
    qDebug() << "DocumentPrint was called";
    QWebFrame *mainFrame = mPage->mainFrame();

    QSize size(800,480);
    //mainFrame->contentsSize(size);

    mPage->setViewportSize(size); //mainFrame->contentsSize()
    QPrinter printer;
    printer.setPageSize(QPrinter::A4);
    printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);
    printer.setColorMode(QPrinter::Color);
    QPrintDialog* dialog = new QPrintDialog(&printer, 0);
    if (dialog->exec() == QDialog::Accepted)
    {
         mainFrame->print(&printer);
    }
    emit deleteLater();
}
