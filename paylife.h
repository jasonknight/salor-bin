#ifndef PAYLIFE_H
#define PAYLIFE_H

#include <QThread>

class PayLife : public QThread
{
    Q_OBJECT
public:
    explicit PayLife(QObject *parent = 0);
    QString addy;
    QString data;
    QString error;
    int descriptor;
    int errorNumber;
    void run();
    
signals:
    void payLifeConfirmed();
    void payLifeError();
public slots:
  void sendPayLifeData(QString data);
};

#endif // PAYLIFE_H
