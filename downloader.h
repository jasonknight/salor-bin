#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include "common_includes.h"
#include <QNetworkReply>
#include "mainwindow.h"
#include <QWidget>
#include <QProgressBar>
#include <QGroupBox>
#include <QVBoxLayout>
class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = 0);
    void setReply(QNetworkReply * rep);
    QString file_name;
    MainWindow * main;
    QGroupBox * box;
    QProgressBar * bar;
signals:
    void fileProgressUpdated(int);
    void finished();
    void addWidget(QWidget *);
    void removeWidget(QWidget *);
public slots:
    void updateFileProgress(qint64 read,qint64 total);
    void writeFile();
private:
    QNetworkReply * reply;

};

#endif // DOWNLOADER_H
