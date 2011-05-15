#ifndef SALOR_WEB_PLUGIN_H
#define SALOR_WEB_PLUGIN_H
#include <QWebPluginFactory>
class SalorWebPlugin : public QWebPluginFactory::Plugin
{
public:
    SalorWebPlugin(QString name, QString description, QList<QWebPluginFactory::MimeType> mimeTypes,QObject* (*create)(const QString &mimeType,
                                                                                                  const QUrl &url, const QStringList &argumentNames,
                                                                                                  const QStringList &argumentValues));
    QObject* (*create)(const QString &mimeType,
                       const QUrl &url, const QStringList &argumentNames,
                       const QStringList &argumentValues);
};
#endif // SALOR_WEB_PLUGIN_H
