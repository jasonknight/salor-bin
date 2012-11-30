#ifndef SALORPROCESS_H
#define SALORPROCESS_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <sys/types.h>
#include <signal.h>
class SalorProcess : public QObject
{
    Q_OBJECT
public:
    explicit SalorProcess(QObject *parent = 0);
    //void run(QString app, QString args);
    void run(QString app, QStringList args, int ttl);
signals:

public slots:
    void 	error ( QProcess::ProcessError error );
    void 	finished ( int exitCode, QProcess::ExitStatus exitStatus );
    void 	readyReadStandardError ();
    void 	readyReadStandardOutput ();
    void 	started ();
    void 	stateChanged ( QProcess::ProcessState newState );
    void    dieMotherfucker();
private:
    QProcess *proc;
    qint64 pid;
    QString name;

};

#endif // SALORPROCESS_H
