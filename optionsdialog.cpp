#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "mainwindow.h"
#include "salorprinter.h"
#include "common_includes.h"
#include <QPrinterInfo>
#include <QList>

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    auth_tried = false;

    ui->urlEditInput->setText(settings->value("url").toString());
    settings->beginGroup("printing");
    ui->printUrlInput->setText(settings->value("url").toString());
    ui->printUsernameInput->setText(settings->value("username").toString());
    settings->endGroup();
    ui->kioskCheckBox->setChecked(settings->value("kiosk").toString() == "true");
    sp = new SalorPrinter;
    setupPrinterCombos();
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::setupPrinterCombos() {
    sp->setPrinterNames();
    signalMapper = new QSignalMapper(this);

    QString labeltext;
    QString localprintername;
    int i = 0;
    foreach(QString remoteprinter, remotePrinterNames) {
      qDebug() << "Dynamically setting up combobox for printer" << remoteprinter;
      QComboBox * combobox = new QComboBox();
      settings->beginGroup(remoteprinter);
      labeltext = settings->value("name").toString();
      localprintername = settings->value("localprinter").toString();
      qDebug() << labeltext;
      QLabel * label = new QLabel(labeltext);
      settings->endGroup();
      combobox->addItems(localPrinterNames);
      connect(combobox, SIGNAL(currentIndexChanged(const QString &)), signalMapper, SLOT(map()));
      signalMapper->setMapping(combobox, remoteprinter);

      //select current value
      qDebug() << "finding index for " << localprintername;
      int index = combobox->findText(localprintername);
      qDebug() << "index is " << index;
      if (index != -1) {
          combobox->setCurrentIndex(index);
      }

      //add stuff to ui
      ui->printerGrid->addWidget(label, i, 0);
      ui->printerGrid->addWidget(combobox, i, 1);

      localPrinterInputWidgetMap[remoteprinter] = combobox;
      i++;
    }
    connect(signalMapper, SIGNAL(mapped(const QString &)), this, SLOT(localPrinterInputWidgetChanged(const QString &)));
}

void OptionsDialog::localPrinterInputWidgetChanged(QString remoteprinter) {
    QString localprinter = localPrinterInputWidgetMap[remoteprinter]->currentText();
    qDebug() << "QComboBox changed:" << remoteprinter << localprinter;
    settings->beginGroup(remoteprinter);
    settings->setValue("localprinter", localprinter);
    settings->endGroup();
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

void OptionsDialog::on_updateSettingsButton_clicked()
{
    qDebug() << "update button clicked";

    QLayoutItem * child;
    while ((child = ui->printerGrid->takeAt(0)) != 0) {
        // this works because the remaining children will be re-enumerated
        delete child->widget();
    }

    settings->beginGroup("printing");
    QString url = settings->value("url").toString();
    QString username = settings->value("username").toString();
    settings->endGroup();

    QNetworkRequest request(QUrl::fromUserInput(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    networkManagerSettings = new QNetworkAccessManager(this);
    connect(networkManagerSettings, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
            this, SLOT(on_authenticationRequired(QNetworkReply*, QAuthenticator*))
    );
    connect(networkManagerSettings,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(on_printInfoFetched(QNetworkReply*)));

    QUrl params;
    params.addQueryItem("id",username);

    QByteArray data;
    data = params.encodedQuery();

    QSslConfiguration c = request.sslConfiguration();
    c.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(c);

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

// this just writes stuff to the .ini file
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
    setupPrinterCombos();
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


void OptionsDialog::on_printNowButton_clicked()
{
    emit setPrinterCounter(1);
}

void OptionsDialog::on_printTestButton_clicked()
{
    foreach(QString remoteprinter, remotePrinterNames) {
        settings->beginGroup(remoteprinter);
        sp->print(settings->value("localprinter").toString(), "OK\n\n\n");
        settings->endGroup();
    }
}

void OptionsDialog::on_kioskCheckBox_clicked(bool checked)
{
    settings->setValue("kiosk", checked);
}
