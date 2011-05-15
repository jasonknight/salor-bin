#ifndef SALOR_PAGE_H
#define SALOR_PAGE_H
#include <QWebPage>
#include <QObject>
class SalorPage:protected QWebPage
{
    Q_OBJECT
public:
    SalorPage(QObject* parent = 0);
protected:
    virtual void javaScriptConsoleMessage ( const QString & message, int lineNumber, const QString & sourceID  );
};
#endif // SALOR_PAGE_H
