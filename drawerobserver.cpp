#include "drawerobserver.h"
#include "salorjsapi.h"
#include "common_includes.h"

DrawerObserver::DrawerObserver(QString path, int baudrate) :
    QObject()
{
    mPath = path;
    mSerialport = NULL;
    mdrawerClosed = false;
    mdrawerOpened = false;
    mBaudrate = baudrate;
    m_notifier = NULL;
}

void DrawerObserver::openDevice() {
    if (mSerialport) {
        qDebug() << "[DrawerObserver]" << "[openDevice]" << "A Serialport is already existing. Must call DrawerObserver::closeDevice before you can open this one again.";
        return;
    } else {
        mSerialport = new Serialport(mPath, mBaudrate);
        mSerialport->open();
    }
}

void DrawerObserver::closeDevice() {
    if (mSerialport) {
        qDebug() << "[DrawerObserver]" << "[closeDevice]" << "Closing existing Serialport.";
        mSerialport->close();
        mSerialport->deleteLater();
        mSerialport = 0;
    } else {
        qDebug() << "[DrawerObserver]" << "[closeDevice]" << "Serialport has already been closed. Must call DrawerObserver::openDevice.";
    }
}

void DrawerObserver::startWithLoop() {
    qDebug() << "[DrawerObserver]" << "[startWithLoop]";

    int count;
    int close_after_seconds = 30;
    int i = 0;
    int j = 0;
    bool drawer_was_open = false;
    QByteArray readbuf;
    QByteArray closedCode = "\x14";
    QByteArray openedCode = "\x10";

    openDevice();
    doStop = false;
    drawerClosed = false;

    QByteArray opencode = "\x1D\x61\x01";
    count = mSerialport->write(opencode);
    qDebug() << "[DrawerObserver]" << "[startWithLoop]" << "Wrote "  << count << " bytes to enable printer feedback.";


    mSerialport->setNonblock();

    while (i < (2 * close_after_seconds) && !doStop) {
        i += 1;

        readbuf = mSerialport->read();

        if (drawer_was_open) {
            if (readbuf.indexOf(closedCode) != -1) {
                drawerClosed = true;
                qDebug() << "[DrawerObserver]" << "[startWithLoop]" << "Closed drawer detected.";
            }
            qDebug() << "[DrawerObserver]" << "[startWithLoop]" << (5*close_after_seconds-i) << "Waiting for Drawer close." << readbuf.size() << "bytes read, in hex: " << readbuf.toHex();
        } else {
            if (readbuf.indexOf(openedCode) != -1) {
                drawer_was_open = true;
                qDebug() << "[DrawerObserver]" << "[startWithLoop]" << "Open drawer detected.";
            }
            qDebug() << "[DrawerObserver]" << "[startWithLoop]" << (5*close_after_seconds-i) << "Waiting for Drawer open." << readbuf.size() << "bytes read, in hex: " << readbuf.toHex();
        }

        if (doStop || drawerClosed) {
            qDebug() << "[DrawerObserver]" << "[startWithLoop]" << "Finishing thread by halting this loop.";
            break;
        }
        usleep(250000);
    }
    closeDevice();
    qDebug() << "[DrawerObserver]" << "[startWithLoop]" << "Finished.";
}


void DrawerObserver::startWithNotifier() {
    int count;
    QByteArray opencode = "\x1D\x61\x01";

    openDevice();
    count = mSerialport->write(opencode);
    qDebug() << "[DrawerObserver]" << "[startWithNotifier]" << "Wrote "  << count << " bytes to enable printer feedback.";

#ifdef LINUX
    m_notifier = new QSocketNotifier(mSerialport->m_fd, QSocketNotifier::Read, this);
    connect(m_notifier, SIGNAL(activated(int)), this, SLOT(readData(int)));
#endif
#ifdef WINDOWS
    // Windows does not support QSocketNotifier on a filedescriptor of type HANDLE void*
#endif

}


void DrawerObserver::stopWithNotifier() {
    qDebug() << "[DrawerObserver]" << "[stopWithNotifier]";
    m_notifier->setEnabled(false);
    m_notifier->deleteLater();
    m_notifier = NULL;
    closeDevice();
}


void DrawerObserver::readData(int fd) {
    QByteArray closedCode = "\x14";
    QByteArray openedCode = "\x10";
    QByteArray printerFeedback;

    printerFeedback = mSerialport->read();

    qDebug() << "[DrawerObserver] [readData]" << printerFeedback.size() << "bytes read:" << printerFeedback.toHex();

    if (printerFeedback.indexOf(openedCode) != -1) {
        mdrawerOpened = true;
        qDebug() << "[DrawerObserver] [readData] Open Drawer detected";
        return;
    }

    if (mdrawerOpened == false && printerFeedback.indexOf(closedCode) != -1) {
        qDebug() << "[DrawerObserver] [readData] Closed Drawer detected. But it was not yet open, so doing nothing.";
        return;
    }

    if (mdrawerOpened == true && printerFeedback.indexOf(closedCode) != -1) {
        qDebug() << "[DrawerObserver] [readData] Closed Drawer detected after it was open. Stopping.";
        mdrawerClosed = true;
        stopWithNotifier();
        emit drawerCloseDetected();
    }
}
