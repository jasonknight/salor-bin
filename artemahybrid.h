#ifndef ARTEMAHYBRID_H
#define ARTEMAHYBRID_H

#include <QThread>

class ArtemaHybrid : public QThread
{
    Q_OBJECT
public:
    explicit ArtemaHybrid(QObject *parent = 0);
    void run();

    // member variables
    int m_state;
    QString m_terminal_state;
    bool m_ecr_ready;
    bool checkParity(char b);
    char encodeParity(char b);
    bool running;
    bool m_ready;
    QString m_data_to_send;

signals:
    void stateChange(int,QString);
    void pollingTimeOut();
    void receiveTimeOut();
    void repeatTimeOut();
    void sendTimeOut();
    void dataSent();
public slots:
    void send(char b);
    void send(QString buffer);
    void newState(int s,QString ts) {
        if (s != this->m_state || ts != this->m_terminal_state) {
            this->m_state = s;
            this->m_terminal_state = ts;
            emit stateChanged(s,ts);
        }
    }
};

#endif // ARTEMAHYBRID_H
