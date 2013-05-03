#ifndef SALORPROCESS_H
#define SALORPROCESS_H

#include <QObject>
#include "common_includes.h"

class SalorProcess : public QObject
{
    Q_OBJECT
public:
    explicit SalorProcess(QObject *parent = 0);
    void run(QString app, QStringList args, int ttl);


private:
    QProcess *proc;
    qint64 pid;
    QString name;
    
signals:
    
public slots:
    void 	error ( QProcess::ProcessError error );
    void 	finished ( int exitCode, QProcess::ExitStatus exitStatus );
    void 	readyReadStandardError ();
    void 	readyReadStandardOutput ();
    void 	started ();
    void 	stateChanged ( QProcess::ProcessState newState );
    void    dieMotherfucker();
    
};

#endif // SALORPROCESS_H
