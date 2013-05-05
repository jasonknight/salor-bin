#include "salorcapture.h"
#include "salorpage.h"
#include "salorprocess.h"
#include "common_includes.h"

SalorCapture::SalorCapture(SalorPage* page,
                           const QString& output,
                           int delay,
                           const QString& scriptProp,
                           const QString& scriptCode) {
  mPage = page;
  mOutput = output;
  mDelay = delay;
  mSawInitialLayout = false;
  mSawDocumentComplete = false;
  mScriptProp = scriptProp;
  mScriptCode = scriptCode;
  mScriptObj = new QObject();

  qDebug() << "In SalorCapture";
  mPage->setSalorCapture(this);

  qDebug() << "setSalorCapture done";
}

void SalorCapture::InitialLayoutCompleted() {
    qDebug() << "in InitialLayoutCompleted";
  mSawInitialLayout = true;

  if (mSawInitialLayout && mSawDocumentComplete)
    TryDelayedRender();
}

void SalorCapture::DocumentComplete(bool /*ok*/) {
  qDebug() << "In DocumentComplete";

  saveSnapshot();
  return;
  mSawDocumentComplete = true;

  if (mSawInitialLayout && mSawDocumentComplete)
    TryDelayedRender();
}

void SalorCapture::JavaScriptWindowObjectCleared() {

}

void SalorCapture::TryDelayedRender() {
    qDebug() << "TryDelayRender called";
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
    qDebug() << "saveSnapshot was called";
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
    qDebug() << "Saving to: " << mOutput;
    image.save(mOutput, "bmp");
    SalorProcess *sp = new SalorProcess(this);
    qDebug() << "Running poledancer";
    sp->run("poledancer",QStringList() << "-dlo" <<  mOutput,2000);

    delete sp;
    emit done();
}

void SalorCapture::DocumentPrint() {
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
    emit done();
}

//void SalorCapture::done() {}
