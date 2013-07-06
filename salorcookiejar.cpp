#include "salorcookiejar.h"
#include "common_includes.h"

SalorCookieJar::SalorCookieJar(QObject *parent) :
    QNetworkCookieJar(parent)
{
    m_timer.setInterval(1000);
    m_file.setFileName(PathCookies);
    qDebug() << "SalorCookieJar(): constructor with m_file" << m_file.fileName();
}

SalorCookieJar::~SalorCookieJar()
{
    qDebug() << "~SalorCookieJar(): destructing";
    if (!m_readonlyMode) {
        extractRawCookies();
        saveToDisk();
    }
}

void SalorCookieJar::setup() {
    loadFromDisk();

    m_readonlyMode = customerScreenId != "";
    if (m_readonlyMode) {
        qDebug() << "SalorCookieJar::setup(): readonly mode: never writing any cookies, but loading them regularly from the cookiejar file";
        connect(&m_timer, SIGNAL(timeout()), this, SLOT(loadFromDisk()));
    } else {
        qDebug() << "SalorCookieJar::setup(): normal mode";
    }
    m_timer.start();
}

bool SalorCookieJar::setCookiesFromUrl(const QList<QNetworkCookie>& cookieList, const QUrl& url)
{
    qDebug() << "SalorCookieJar::setCookiesFromUrl()";
    bool status = QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
    if (status && !m_readonlyMode) {
        extractRawCookies();
        saveToDisk();
    }
    return status;
}

void SalorCookieJar::extractRawCookies()
{
    qDebug() << "SalorCookieJar::extractRawCookies()";
    QList<QNetworkCookie> cookies = allCookies();
    m_rawCookies.clear();

    foreach (const QNetworkCookie &cookie, cookies) {
      m_rawCookies.append(cookie.toRawForm());
    }
}

void SalorCookieJar::saveToDisk()
{
    qDebug() << "SalorCookieJar::saveToDisk()";
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
    qDebug() << "SalorCookieJar::loadFromDisk()";
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
    //qDebug() << "Loaded" << QString::number(cookies.length()) << "Cookies";
    setAllCookies(cookies);
}

void SalorCookieJar::reset()
{
    qDebug() << "SalorCookieJar::reset()";
    setAllCookies(QList<QNetworkCookie>());
    extractRawCookies();
    saveToDisk();
}
