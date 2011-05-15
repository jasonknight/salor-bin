#include "salor_page.h"
#include <QDebug>
#include <QMessageBox>
#include "salor_plugin_factory.h"
SalorPage::SalorPage(QObject* parent):QWebPage(parent)
{
    //SalorPluginFactory* wpf = new SalorPluginFactory(this);

    //this->setPluginFactory(wpf);
}
void SalorPage::javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID  ) {
    QMessageBox::critical(0, QObject::tr("Critical Script Error"), QString() + "A Javascript error Occurred:\n" + message + "\nat line " + QString(lineNumber) + "\nin " + sourceID);
}
