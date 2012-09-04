
#include "salor_web_plugin.h"

//begin constructor
SalorWebPlugin::SalorWebPlugin(QString name,
                             QString description,
                             QList<QWebPluginFactory::MimeType>mimeTypes,
                             /* This should be a pointer to function that returns the QObject* for the plugin.
                                SalorPluginFactory::create will just pass the params to this function and return it's
                                return value... */
                             QObject* (*create)(const QString &mimeType,
                                                const QUrl &url, const QStringList &argumentNames,
                                                const QStringList &argumentValues))
              : QWebPluginFactory::Plugin()
{
    this->name = name;
    this->description = description;
    this->mimeTypes = mimeTypes;
    this->create = create;
}
