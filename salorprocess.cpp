#include "salorprocess.h"
#include "common_includes.h"

SalorProcess::SalorProcess(QObject *parent) :
    QObject(parent)
{
}

void SalorProcess::run(QString app, QStringList args, int ttl) {
    this->proc = new QProcess(this);
    //this->connect(this->proc,SIGNAL(started()),SLOT(started()));
    this->connect(this->proc,SIGNAL(finished(int,QProcess::ExitStatus)),SLOT(finished(int,QProcess::ExitStatus)));
    //this->connect(this->proc,SIGNAL(readyReadStandardError()),SLOT(readyReadStandardError()));
    //this->connect(this->proc,SIGNAL(readyReadStandardOutput()),SLOT(readyReadStandardOutput()));
    //this->connect(this->proc,SIGNAL(error(QProcess::ProcessError)),SLOT(error(QProcess::ProcessError)));
    this->proc->startDetached(app,QStringList()<<args, QString("/"));
    qDebug() << app << " started at: " << QDateTime::currentDateTime();
    //QTimer::singleShot(ttl,this,SLOT(dieMotherfucker()));
}

void SalorProcess::started() {
    qDebug() << "\n\nProcess has started\n\n";
}
void SalorProcess::finished(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "\n\nProcess has finished\n\n";
    //this->proc->close();
}

void 	SalorProcess::error ( QProcess::ProcessError error ) {
    qDebug() << "\n\nProcess has error\n\n";
    this->proc->terminate();
}

void 	SalorProcess::readyReadStandardError () {
    qDebug() << "\n\nProcess has error to read" << this->proc->readAllStandardError();
    this->proc->waitForFinished(100);
    qDebug() << "Terminating";
    this->proc->terminate();
}

void 	SalorProcess::readyReadStandardOutput () {
    qDebug() << "\n\nProcess has output to read" << this->proc->readAllStandardOutput();

}
void 	SalorProcess::stateChanged ( QProcess::ProcessState newState ) {
    qDebug() << "\n\nProcess has new state\n\n";
    this->proc->readAllStandardError();
    this->proc->readAllStandardOutput();
}

void    SalorProcess::dieMotherfucker() {
    qDebug() << this->name << " killed at: " << QDateTime::currentDateTime();
    #ifdef LINUX
        ::kill(this->pid,SIGKILL);
    #endif
    this->deleteLater();
}
