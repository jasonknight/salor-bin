#include "downloader.h"
#include <QtGlobal>
#include <QSize>
#include <QFont>
Downloader::Downloader(QObject *parent) :
    QObject(parent)
{
}
void Downloader::setReply(QNetworkReply *rep) {
    this->reply = rep;
    box = new QGroupBox;
    bar = new QProgressBar;
    QVBoxLayout * vbox = new QVBoxLayout;
    box->setFixedHeight(55);
    QFont f;
    f.setFamily("sans-serif");
    f.setPixelSize(9);
    box->setFont(f);
     vbox->addWidget(bar);


     box->setTitle(QFileInfo(this->file_name).fileName());
     box->setLayout(vbox);
     qDebug() << "Emitting Add Widget";
     emit addWidget(box);
    connect(this->reply,SIGNAL(finished()),this,SLOT(writeFile()));
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateFileProgress(qint64,qint64)));

}
void Downloader::writeFile() {
    qDebug() << "Write File called";
    QByteArray ba = reply->readAll();
    QFile f(file_name);
    if (f.open(QIODevice::WriteOnly)) {
        QTextStream out(&f);
        out << ba;
        f.close();
        qDebug() << "save completed.";

    } else {
        qDebug() << "Failed to save file" << file_name;
    }
    emit removeWidget(box);
    emit finished();

}
void Downloader::updateFileProgress(qint64 read,qint64 total) {
    qDebug() << "Updating file progress" << QString::number(read) << "  " << QString::number(total);
    this->bar->setMaximum((qint32)total);
    this->bar->setValue((qint32)read);
}
