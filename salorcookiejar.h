#ifndef SALORCOOKIEJAR_H
#define SALORCOOKIEJAR_H

#include <QFile>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QTimer>

class SalorCookieJar : public QNetworkCookieJar {
    Q_OBJECT

public:
    SalorCookieJar(QObject* parent = 0);
    virtual ~SalorCookieJar();

    virtual bool setCookiesFromUrl(const QList<QNetworkCookie>&, const QUrl&);

    void setDiskStorageEnabled(bool);

public slots:
    void scheduleSaveToDisk();
    void loadFromDisk();
    void reset();

private slots:
    void saveToDisk();

private:
    void extractRawCookies();

    QList<QByteArray> m_rawCookies;
    bool m_storageEnabled;
    QFile m_file;
    QTimer m_timer;
};

#endif // SALORCOOKIEJAR_H
