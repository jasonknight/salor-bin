#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <common_includes.h>

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();
    bool _ready;
    
private:
    Ui::OptionsDialog *ui;
    QSettings *settings;
    bool auth_tried;
    QNetworkAccessManager * networkManagerSettings;
    QSignalMapper *signalMapper;
    QMap<QString, QComboBox *> localPrinterInputWidgetMap;
    void setupPrinterCombos();

signals:
    void navigateToUrl(QString);
    void clearCache();
    void sendJS(QString &js);
    void startPrintTimer();



public slots:
    void on_goButton_clicked();
    void on_clearCacheButton_clicked();
    void on_authenticationRequired(QNetworkReply * reply, QAuthenticator * auth);
    void on_printInfoFetched(QNetworkReply * rep);
    void on_printoutFetched(QNetworkReply * rep);

private slots:
    void on_urlEditInput_textChanged(QString value);
    void on_updateSettingsButton_clicked();
    void on_printUrlInput_textChanged(const QString &arg1);
    void on_printUsernameInput_textChanged(const QString &arg1);
    void localPrinterInputWidgetChanged(QString text);
};

#endif // OPTIONSDIALOG_H
