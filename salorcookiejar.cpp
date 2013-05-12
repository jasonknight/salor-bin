#include "salorcookiejar.h"
#include "common_includes.h"

SalorCookieJar::SalorCookieJar(QObject *parent) :
    QNetworkCookieJar(parent)
{
    m_timer.setInterval(10);
    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(saveToDisk()));
    m_file.setFileName(PathCookies);
    qDebug() << m_file.fileName();
    loadFromDisk();
}

SalorCookieJar::~SalorCookieJar()
{
    if (m_storageEnabled) {
        extractRawCookies();
        saveToDisk();
    } else {
        qDebug() << "~SalorCookieJar(): Not writing Cookies to disk";
    }
}

bool SalorCookieJar::setCookiesFromUrl(const QList<QNetworkCookie>& cookieList, const QUrl& url)
{
    //qDebug() << "SalorCookieJar::setCookiesFromUrl()";
    bool status = QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
    if (status && m_storageEnabled)
        scheduleSaveToDisk();
    return status;
}

void SalorCookieJar::setDiskStorageEnabled(bool enabled)
{
    //qDebug() << "SalorCookieJar::setDiskStorageEnabled()";
    m_storageEnabled = enabled;

    if (enabled && allCookies().isEmpty())
        loadFromDisk();

    // When disabling, save current cookies.
    if (!enabled && !allCookies().isEmpty())
        scheduleSaveToDisk();
}

void SalorCookieJar::scheduleSaveToDisk()
{
    // We extract the raw cookies here because the user may
    // enable/disable/clear cookies while the timer is running.
    //qDebug() << "SalorCookieJar::scheduleSaveToDisk()";
    extractRawCookies();
    m_timer.start();
}

void SalorCookieJar::extractRawCookies()
{
    //qDebug() << "SalorCookieJar::extractRawCookies()";
    QList<QNetworkCookie> cookies = allCookies();
    m_rawCookies.clear();

    foreach (const QNetworkCookie &cookie, cookies) {
      m_rawCookies.append(cookie.toRawForm());
    }
}

void SalorCookieJar::saveToDisk()
{
    m_timer.stop();
    //qDebug() << "SalorCookieJar::saveToDisk()";

    if (m_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&m_file);
        foreach (const QByteArray &cookie, m_rawCookies)
            out << cookie + "\n";
        m_file.close();
    } else {
         qDebug() << "SalorCookieJar::saveToDisk(): ERROR";
    }
}

void SalorCookieJar::loadFromDisk()
{
    //qDebug() << "SalorCookieJar::loadFromDisk()";
    if (!m_file.exists()) {
        qDebug() << "SalorCookieJar::loadFromDisk(): File does not exist.";
        return;
    }

    QList<QNetworkCookie> cookies;
    if (m_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&m_file);
        QString txt = in.readAll();
        QStringList lines = txt.split("\n");
       foreach (QString c, lines) {
        cookies.append(QNetworkCookie::parseCookies(c.toUtf8()));
       }
        m_file.close();
    } else {
         qDebug() << "SalorCookieJar::loadFromDisk(): ERROR";
    }
    //qDebug() << "Setting cookies..." << QString::number(cookies.length());
    setAllCookies(cookies);
}

void SalorCookieJar::reset()
{
    //qDebug() << "SalorCookieJar::reset()";
    setAllCookies(QList<QNetworkCookie>());
    if (m_storageEnabled)
        scheduleSaveToDisk();
}
