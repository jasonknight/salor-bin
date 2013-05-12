#include "optionsdialog.h"
#include "ui_optionsdialog.h"
#include "mainwindow.h"
#include "salorprinter.h"
#include "downloader.h"
#include "common_includes.h"
#include <QPrinterInfo>
#include <QList>

OptionsDialog::OptionsDialog(QWidget *parent, QNetworkAccessManager *nm) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    networkManager = nm;
    ui->setupUi(this);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::init() {
    auth_tried = false;
    ui->urlEditInput->setText(settings->value("url").toString());
    settings->beginGroup("printing");
    ui->printUrlInput->setText(settings->value("url").toString());
    ui->printUsernameInput->setText(settings->value("username").toString());
    settings->endGroup();
    ui->kioskCheckBox->setChecked(settings->value("kiosk").toString() == "true");
    setupPrinterCombos();
}

void OptionsDialog::setupPrinterCombos() {
    emit setPrinterNames();
    signalMapper = new QSignalMapper(this);

    QString labeltext;
    QString localprintername;
    int i = 0;
    foreach(QString remoteprinter, remotePrinterNames) {
      //qDebug() << "Dynamically setting up combobox for printer" << remoteprinter;
      QComboBox * combobox = new QComboBox();
      settings->beginGroup(remoteprinter);
      labeltext = settings->value("name").toString();
      localprintername = settings->value("localprinter").toString();
      QLabel * label = new QLabel(labeltext);
      settings->endGroup();
      combobox->addItems(localPrinterNames);
      connect(combobox, SIGNAL(currentIndexChanged(const QString &)), signalMapper, SLOT(map()));
      signalMapper->setMapping(combobox, remoteprinter);

      //select current value
      //qDebug() << "finding index for " << localprintername;
      int index = combobox->findText(localprintername);
      //qDebug() << "index is " << index;
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
    //qDebug() << "emitting clear cache";
    // TODO: This does not seem to clear the .cache directory
    emit clearCache();
}

void OptionsDialog::on_updateSettingsButton_clicked()
{
    //qDebug() << "update button clicked";

    QLayoutItem * child;
    while ((child = ui->printerGrid->takeAt(0)) != 0) {
        // this works because the remaining children will be re-enumerated
        delete child->widget();
    }

    QString url_firstpart = settings->value("url").toString();
    settings->beginGroup("printing");
    QString url = url_firstpart + settings->value("url").toString();
    QString username = settings->value("username").toString();
    settings->endGroup();

    qDebug() << "Starting request to " << url;
    QNetworkRequest request(QUrl::fromUserInput(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    QUrl params;
    params.addQueryItem("id",username);

    QByteArray data;
    data = params.encodedQuery();

    //QSslConfiguration c = request.sslConfiguration();
    //c.setPeerVerifyMode(QSslSocket::VerifyNone);
    //request.setSslConfiguration(c);

    QNetworkReply *reply = networkManager->post(request,data);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onError(QNetworkReply::NetworkError))
    );
    connect(reply,SIGNAL(finished()),
            this,SLOT(onPrintInfoFetched()));
}

void OptionsDialog::onError(QNetworkReply::NetworkError error) {
    qDebug() << "OptionsDialog::onError()";
    //QAuthenticator *auth = qobject_cast<QAuthenticator *>(sender());
    //auth->setUser(settings->value("username").toString());
    //auth->setPassword(settings->value("password").toString());
    //auth->setUser("username222");
    //auth->setPassword("password222");
    //if(auth_tried == true) {
      // problem with the authenticationRequired signal is that it will cache wrong auth username/password, which leads to an endless loop with the server is immediately asking again for authentication. so, we kill it at the second attempt.
      //delete networkManagerSettings;
    //}
    //auth_tried = true;
}

// this just writes stuff to the .ini file
void OptionsDialog::onPrintInfoFetched() {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    auth_tried = false;

    QRegExp rxPrinters("^printer.*");
    QStringList groupsBefore = (*settings).childGroups().filter(rxPrinters);
    //qDebug() << "filtered:" << groups;

    QStringList groupsAfter;

    qDebug() << "Attempting to read reply";
    QString body = reply->readAll();
    qDebug() << "Reply is: " << body;
    if (body.indexOf("html") == -1) {
        // don't parse html errors from the server
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
                    QString printernumber = key.mid(10, 4);
                    QString printergroup = "printer" + printernumber;
                    groupsAfter << printergroup;
                    settings->beginGroup(printergroup);
                    settings->setValue("url", val);
                    settings->endGroup();
                } else if (key.indexOf("printername") != -1) {
                    QString printernumber = key.mid(11, 4);
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
    }

    qDebug() << groupsBefore;
    qDebug() << groupsAfter;
    foreach(QString grpb, groupsBefore) {
        qDebug() << "checking for" << grpb;
        if (groupsAfter.contains(grpb) == false) {
            qDebug() << "removing";
            settings->remove(grpb);
        }
    }
    setupPrinterCombos();
    reply->deleteLater();
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
        SalorPrinter *printer = new SalorPrinter(this, networkManager, settings->value("localprinter").toString());
        printer->print("*** OK *** \n\n\n");
        // sp will delete itself after printing
        settings->endGroup();
    }
}

void OptionsDialog::on_kioskCheckBox_clicked(bool checked)
{
    settings->setValue("kiosk", checked);
}

void OptionsDialog::on_pushButton_clicked()
{
    Downloader * d = new Downloader;
    d->file_name = PathWorking + "/salor-bin.exe";
    //connect(d,SIGNAL(fileProgressUpdated(int)), main, SLOT(setProgress(int)));
    connect(d,SIGNAL(finished()),d,SLOT(deleteLater()));
    //connect(d,SIGNAL(addWidget(QWidget*)),this,SLOT(bubbleAddWidget(QWidget*)));
    //connect(d,SIGNAL(removeWidget(QWidget*)),this,SLOT(bubbleRemoveWidget(QWidget*)));
    qDebug() << "UPGRADING";
    QNetworkRequest request = QNetworkRequest(QUrl("http://resources.red-e.eu/tools/salor-bin/salor-bin.exe"));
    QNetworkReply *reply = networkManager->get(request);
    d->setReply(reply);
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),d,SLOT(updateFileProgress(qint64,qint64)));
    //d->main = main;
}
