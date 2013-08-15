#include "salorcapture.h"
#include "salorpage.h"
#include "salorprocess.h"
#include "common_includes.h"

SalorCapture::SalorCapture(SalorPage* page,
                           const QString& output,
                           int delay,
                           const QString& scriptProp,
                           const QString& scriptCode) {

    qDebug() << "[SalorCapture]" << "[Initializer]" << "Called.";

    mPage = page;
    mOutput = output;
    mDelay = delay;
    mSawInitialLayout = false;
    mSawDocumentComplete = false;
    mScriptProp = scriptProp;
    mScriptCode = scriptCode;
    mScriptObj = new QObject();
    mPage->setSalorCapture(this);
}

void SalorCapture::InitialLayoutCompleted() {
    qDebug() << "[SalorCapture]" << "[InitialLayoutCompleted]" << "Called.";
    /*
    mSawInitialLayout = true;

    if (mSawInitialLayout && mSawDocumentComplete) {
        TryDelayedRender();
    }
    */
}

void SalorCapture::DocumentComplete(bool /*ok*/) {
    qDebug() << "[SalorCapture]" << "[DocumentComplete]" << "Called. Calling saveSnapshot().";
    saveSnapshot();
    /*
    return;
    mSawDocumentComplete = true;

    if (mSawInitialLayout && mSawDocumentComplete)
    TryDelayedRender();
    */
}

void SalorCapture::JavaScriptWindowObjectCleared() {
    qDebug() << "[SalorCapture]" << "[JavaScriptWindowObjectCleared]" << "Called.";
}

void SalorCapture::TryDelayedRender() {
    qDebug() << "[SalorCapture]" << "[TryDelayedRender]" << "Called.";
    if (!mPage->getAlertString().isEmpty())
        return;

    if (mDelay > 0) {
        QTimer::singleShot(mDelay, this, SLOT(Delayed()));
        return;
    }

    saveSnapshot();
}

void SalorCapture::Timeout() {
    qDebug() << "[SalorCapture]" << "[Timeout]" << "Called.";
    saveSnapshot();
}

void SalorCapture::Delayed() {
    qDebug() << "[SalorCapture]" << "[Delayed]" << "Called.";
    saveSnapshot();
}

void SalorCapture::saveSnapshot() {
    qDebug() << "[SalorCapture]" << "[saveSnapshot]" << "Called.";
    QWebFrame *mainFrame = mPage->mainFrame();

    QSize size(800,480);
    //mainFrame->contentsSize(size);

    mPage->setViewportSize(size); //mainFrame->contentsSize()
    QImage image(size, QImage::Format_ARGB32); // mPage->viewportSize()

    qDebug() << "[SalorCapture]" << "[saveSnapshot]" << "Painting.";
    QPainter painter;
    painter.begin(&image);
    mainFrame->render(&painter);
    painter.end();

    qDebug() << "[SalorCapture]" << "[saveSnapshot]" << "Saving image to" << mOutput;
    image.save(mOutput, "bmp");

    qDebug() << "[SalorCapture]" << "[saveSnapshot]" << "Instantiating SalorProcess for running poledancer";
    SalorProcess *sp = new SalorProcess(this);
    sp->run("poledancer", QStringList() << "-dlo" <<  mOutput, 2000);
    delete sp;
    emit done();
}

void SalorCapture::DocumentPrint() {
    qDebug() << "[SalorCapture]" << "[DocumentPrint]" << "Called";
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
