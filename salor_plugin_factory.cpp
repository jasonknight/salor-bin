#include "salor_plugin_factory.h"
#include <QTreeView>
#include <QDebug>
SalorPluginFactory::SalorPluginFactory(QObject* parent) : QWebPluginFactory(parent)
{
    //manager = new QNetworkAccessManager(this);
    qDebug() << "Factory has been created...";
}
QObject* SalorPluginFactory::create(const QString &mimeType,
                                   const QUrl &url, const QStringList &argumentNames,
                                   const QStringList &argumentValues) const{
    qDebug() << "create called";
    QWebPluginFactory::MimeType m;
    for (int i = 0; i < this->pluginList.count(); i++) {
        SalorWebPlugin pli = this->pluginList.at(i);
        for (int j = 0; j < pli.mimeTypes.count(); i++) {
            m = pli.mimeTypes.at(j);
            if (m.name == mimeType) {
                if (pli.create) {
                    return pli.create(mimeType,url,argumentNames,argumentValues);
                }
            }
        }
    }
    return NULL;
}
QList<QWebPluginFactory::Plugin> SalorPluginFactory::plugins() const {
       QList<QWebPluginFactory::Plugin> pl;
       for (int i = 0; i < this->pluginList.count(); ++i)
           pl << this->pluginList.at(i);
       return pl;
}
void SalorPluginFactory::append(SalorWebPlugin plugin) {
    this->pluginList << plugin;
}

