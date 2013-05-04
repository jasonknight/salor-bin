#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "mainwindow.h"
#include "common_includes.h"
#include <QPrinterInfo>
#include <QList>

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);

    auth_tried = false;

    signalMapper = new QSignalMapper(this);
    settings = new QSettings(PathSettings, QSettings::IniFormat);

    ui->urlEditInput->setText(_get(QString("url")).toString());
    settings->beginGroup("printing");
    ui->printUrlInput->setText(settings->value("url").toString());
    ui->printUsernameInput->setText(settings->value("username").toString());
    settings->endGroup();

    _ready = false;
#ifdef MAC
    int i;
    cups_dest_t *dests, *dest;
    int num_dests = cupsGetDests(&dests);
    for (i = num_dests, dest = dests; i > 0; i--, dest++) {
        //qDebug() << "Adding" << list.at(i).printerName();
        QString name = dest->name;
        ui->printerComboBox->addItem(name,QVariant(name));
     }

#endif
#ifdef LINUX
    // populate combo box for local printers
    QStringList filters;
    filters << "ttyS*" << "ttyUSB*" << "usb";
    QDir * devs = new QDir("/dev", "*", QDir::Name, QDir::System);
    (*devs).setNameFilters(filters);
    ui->localPrinters1Combo->addItems(devs->entryList());

    // populate combo box for remote printers
    QStringList groups = (*settings).childGroups();
    QStringList remoteprinters;
    //qDebug() << "GROUPS" << groups;
    for (int i = 0; i < groups.size(); i++)
    {
        QString group = groups[i];
        if (group.indexOf("printer") != -1) {
            settings->beginGroup(group);
            remoteprinters << settings->value("name").toString();
            settings->endGroup();
            QLineEdit * inputfield = new QLineEdit();
            connect(inputfield, SIGNAL(textChanged(const QString &)), signalMapper, SLOT(map()));
            signalMapper->setMapping(inputfield, group);
            ui->myGrid->addWidget(inputfield,i,0);
        }
    }
    connect(signalMapper, SIGNAL(mapped(const QString &)),         this, SLOT(myTextChanged(const QString &)));
    ui->remotePrinters1Combo->addItems(remoteprinters);
#endif
#ifdef WIN32
    QList<QPrinterInfo> printer_list = QPrinterInfo::availablePrinters();
    for (int i = 0; i < printer_list.length(); i++) {
        QPrinterInfo info = printer_list.at(i);
        QString name = info.printerName();
        qDebug() << "Adding " << name << " to combo box";
        ui->localPrinters1Combo->addItem(name,QVariant(name));
    }
    /*LPBYTE pPrinterEnum;
    DWORD pcbNeeded, pcbReturned;
    PRINTER_INFO_2 *piTwo = NULL;
    EnumPrinters(PRINTER_ENUM_LOCAL,NULL,2,NULL,0,&pcbNeeded,&pcbReturned);
    pPrinterEnum = new BYTE[pcbNeeded];
    if (!EnumPrinters(PRINTER_ENUM_LOCAL,NULL,2,pPrinterEnum,pcbNeeded,&pcbNeeded,&pcbReturned)) {
        bug(could not enumerate printers);
    } else {
        piTwo = ((PRINTER_INFO_2*)pPrinterEnum);
        for (int i = 0; i < pcbReturned; i++) {
            QString name = QString::fromWCharArray(piTwo[i].pPrinterName);
            ui->printerComboBox->addItem(name,QVariant(name));
        }
    }*/
#endif
    settings->beginGroup("printer1");
    int index = ui->localPrinters1Combo->findText(settings->value("name").toString());
    settings->endGroup();
    //qDebug() << "index is: " << index << "get is:" << _get("salor.thermal_printer");
    if (index != -1) {
        ui->localPrinters1Combo->setCurrentIndex(index);
    }
    connect(ui->localPrinters1Combo,
            SIGNAL(currentIndexChanged(QString)),
            this,
            SLOT(on_localPrinters1Combo_currentIndexChanged(QString))
            );
    _ready = true;
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::myTextChanged(QString text) {
    qDebug() << "TEXT CHANGED" << text;
}

void OptionsDialog::on_urlEditInput_textChanged(QString value)
{
    _set(QString("url"),value);
}

void OptionsDialog::on_goButton_clicked()
{
    QString url = _get("url").toString();
    emit navigateToUrl(url);
}

void OptionsDialog::on_clearCacheButton_clicked() {
    qDebug() << "emitting clear cache";
    // TODO: This does not seem to clear the .cache directory
    emit clearCache();
}

void OptionsDialog::on_localPrinters1Combo_currentIndexChanged(QString name)
{
    if(this->_ready == false)
        return;

    settings->beginGroup("printer1");
    settings->setValue("name", name);
    settings->endGroup();
    //QClipboard *clipboard = QApplication::clipboard();
    //clipboard->setText(name);
}


void OptionsDialog::on_updateSettingsButton_clicked()
{
    qDebug() << "update button clicked";
    settings->beginGroup("printing");
    QString url = settings->value("url").toString();
    QString username = settings->value("username").toString();
    settings->endGroup();

    QNetworkRequest request(QUrl::fromUserInput(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    networkManagerSettings = new QNetworkAccessManager(this);
    connect(networkManagerSettings,
              SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
              this,
              SLOT(on_authenticationRequired(QNetworkReply*, QAuthenticator*))
    );
    QByteArray data;
    QUrl params;
    QSslConfiguration c = request.sslConfiguration();
    c.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(c);
    params.addQueryItem("id",username);
    data = params.encodedQuery();
    connect(networkManagerSettings,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(on_printInfoFetched(QNetworkReply*)));
    networkManagerSettings->post(request,data);
}

void OptionsDialog::on_authenticationRequired(QNetworkReply *reply, QAuthenticator *auth) {
    qDebug() << "Authentication required";
    //auth->setUser(settings->value("username").toString());
    //auth->setPassword(settings->value("password").toString());
    auth->setUser("username222");
    auth->setPassword("password222");
    if(auth_tried == true) {
      // problem with the authenticationRequired signal is that it will cache wrong auth username/password, which leads to an endless loop with the server is immediately asking again for authentication. so, we kill it at the second attempt.
      delete networkManagerSettings;
    }
    auth_tried = true;
}

void OptionsDialog::on_printInfoFetched(QNetworkReply *rep) {
    auth_tried = false;
    qDebug() << "Attempting to read reply";
    QString body = rep->readAll();
    qDebug() << "Reply is: " << body;
    QStringList lines = body.split("\n");
    QRegExp rx("([\\w\-]+)\\:(.+)");
    foreach(QString line,lines) {
        qDebug() << "line is" << line;
        int pos = 0;
        while( ( pos = rx.indexIn(line,pos) ) != -1 ) {
            QString key = rx.cap(1);
            QString val = rx.cap(2);
            qDebug() << "Key is: " << key << " value is: " << val;
            if (key.indexOf("printerurl") != -1) {
                QString printernumber = key.mid(10, 1);
                QString printergroup = "printer" + printernumber;
                settings->beginGroup(printergroup);
                settings->setValue("url", val);
                settings->endGroup();
            } else if (key.indexOf("printername") != -1) {
                QString printernumber = key.mid(11, 1);
                QString printergroup = "printer" + printernumber;
                settings->beginGroup(printergroup);
                settings->setValue("name", val);
                settings->endGroup();
            } else {
                // other print settings
                settings->beginGroup("printing");
                settings->setValue(key,val);
                settings->endGroup();
            }
            pos += rx.matchedLength();
        }
    }
    //qDebug() << "Setting up timer";
    //emit startPrintTimer();
    /*settings->beginGroup("printer-info");
        timer->stop();
        int interval = settings->value("interval").toInt();
        if (interval) {
             qDebug() << "Printer Info Interval is " << interval;
            timer->start(interval * 1000);
        } else {
            qDebug() << "interval is not good " << interval;
        }
    settings->endGroup();*/

}

void OptionsDialog::on_printoutFetched(QNetworkReply *rep) {
    qDebug() << "Printout Fetched";
    QByteArray ba = rep->readAll();
    qDebug() << ba;
    QString path = settings->value("printer-name").toString();
    if (ba.length() > 1) {
        qDebug() << "PRINTING";
        //SalorPrinter::printURL(QString path, QString url, QString confirm_url)
        //this->print(path,ba);
    }
}

void OptionsDialog::on_printUrlInput_textChanged(const QString &arg1)
{
    settings->beginGroup("printing");
    settings->setValue("url", arg1);
    settings->endGroup();
}

void OptionsDialog::on_printUsernameInput_textChanged(const QString &arg1)
{
    settings->beginGroup("printing");
    settings->setValue("username", arg1);
    settings->endGroup();
}
