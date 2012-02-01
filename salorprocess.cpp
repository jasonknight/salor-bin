#include "salorprocess.h"
#include <QDebug>
SalorProcess::SalorProcess(QObject *parent) :
    QObject(parent)
{
}
void SalorProcess::run(QString app, QStringList args) {
    this->proc = new QProcess(this);
    this->connect(this->proc,SIGNAL(started()),SLOT(started()));
    this->connect(this->proc,SIGNAL(finished(int,QProcess::ExitStatus)),SLOT(finished(int,QProcess::ExitStatus)));
    this->connect(this->proc,SIGNAL(readyReadStandardError()),SLOT(readyReadStandardError()));
    this->connect(this->proc,SIGNAL(readyReadStandardOutput()),SLOT(readyReadStandardOutput()));
    this->connect(this->proc,SIGNAL(error(QProcess::ProcessError)),SLOT(error(QProcess::ProcessError)));
    this->proc->start(app,QStringList()<<args);
}
void SalorProcess::started() {
    qDebug() << "\n\nProcess has started\n\n";
}
void SalorProcess::finished(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "\n\nProcess has finished\n\n";
    this->proc->close();
    emit deleteLater();
}
void 	SalorProcess::error ( QProcess::ProcessError error ) {
    qDebug() << "\n\nProcess has error\n\n";
    this->proc->close();
    emit deleteLater();
}
void 	SalorProcess::readyReadStandardError () {
    qDebug() << "\n\nProcess has error to read\n\n" << this->proc->readAllStandardError();
}
void 	SalorProcess::readyReadStandardOutput () {
    qDebug() << "\n\nProcess has output to read\n" << this->proc->readAllStandardOutput();
}
void 	SalorProcess::stateChanged ( QProcess::ProcessState newState ) {
    qDebug() << "\n\nProcess has new state\n\n";
}
