#ifndef WEBCAM_H
#define WEBCAM_H

#include <QThread>

class WebCam : public QThread
{
    Q_OBJECT
public:
    explicit WebCam(QObject *parent = 0);
    int addy;
    QString filePath;
    QString error;
    int id;
    int errorNumber;
    void run();
signals:
    void imageSaved(int id, QString filePath);
    void WebCamError();
public slots:

};

#endif // WEBCAM_H
