#include "cutecredit.h"

CuteCredit::CuteCredit(QObject *parent) :
    QThread(parent)
{
        this->running = true;
}
void CuteCredit::run() {
    FILE * fp;
    char readbuf[512];
    while (this->running) {
        fp = fopen("/tmp/CUTE_CREDIT_OUT","r");
        fgets(readbuf,512,fp);
        fclose(fp);
        emit dataRead(QString(readbuf));
    }
}
void CuteCredit::writeData(QString data) {
    FILE * fp;
    fp = fopen("/tmp/CUTE_CREDIT_IN","w");
    fputs(data.toAscii().data(),fp);
    fclose(fp);
}
void CuteCredit::stopRunning() {
    this->running = false;
}
