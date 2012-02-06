#include "cutecredit.h"
#include <QDebug>
#include <QFile>
CuteCredit::CuteCredit(QObject *parent) :
    QThread(parent)
{
        this->running = true;
}
void CuteCredit::run() {
    FILE * fp;
    char readbuf[512];
    while (this->running) {

        QFile f("/tmp/CUTE_CREDIT_OUT");
        if (!f.exists())
            continue;
        qDebug() << "running";
        fp = fopen("/tmp/CUTE_CREDIT_OUT","r");
        fgets(readbuf,512,fp);
        fclose(fp);
        emit dataRead(QString(readbuf));
    }
    qDebug() << "Ending CuteCredit THread";
}
void CuteCredit::writeData(QString data) {
    qDebug() << "CuteCredit Thread received: " << data;
            ;
    FILE * fp;
    fp = fopen("/tmp/CUTE_CREDIT_IN","w");
    fputs(data.toAscii().data(),fp);
    fclose(fp);
}
void CuteCredit::stopRunning() {
    this->running = false;
}
