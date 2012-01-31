#ifndef SALORPROCESS_H
#define SALORPROCESS_H

#include <QObject>
#include <QProcess>
class SalorProcess : public QObject
{
    Q_OBJECT
public:
    explicit SalorProcess(QObject *parent = 0);
    //void run(QString app, QString args);
    void run(QString app, QStringList args);
signals:

public slots:
    void 	error ( QProcess::ProcessError error );
    void 	finished ( int exitCode, QProcess::ExitStatus exitStatus );
    void 	readyReadStandardError ();
    void 	readyReadStandardOutput ();
    void 	started ();
    void 	stateChanged ( QProcess::ProcessState newState );
private:
    QProcess *proc;

};

#endif // SALORPROCESS_H
