#ifndef SALOR_PLUGIN_FACTORY_H
#define SALOR_PLUGIN_FACTORY_H
#include <QWebPluginFactory>
#include <QObject>
#include <QNetworkAccessManager>
#include <salor_web_plugin.h>
class SalorPluginFactory : public QWebPluginFactory
{
    Q_OBJECT
public:
    SalorPluginFactory(QObject *parent = 0);
    QObject *create(const QString &mimeType,
                const QUrl &url, const QStringList &argumentNames,
                const QStringList &argumentValues) const;
    QList<QWebPluginFactory::Plugin> plugins() const;
    QList<SalorWebPlugin> pluginList;
public slots:
    void append(SalorWebPlugin plugin);
private:
   QNetworkAccessManager *manager;
};
#endif // SALOR_PLUGIN_FACTORY_H
