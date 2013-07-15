#include "drawerobserver.h"
#include "salorjsapi.h"
#include "common_includes.h"

DrawerObserver::DrawerObserver() :
    QObject()
{
    mPath = "";
    mdrawerClosed = false;
    mdrawerOpened = false;
}

void DrawerObserver::openDevice() {
    qDebug() << "[DrawerObserver]" << "[openDevice] Attempting to open file descriptior with open() at " << mPath;
#ifdef LINUX
     struct termios options;

     mFiledescriptor = open(mPath.toLatin1(), O_RDWR | O_NDELAY);
     if (mFiledescriptor < 0) {
         qDebug() << "DrawerObserverThread::open(): Unable to open port for drawer" << mPath << "mFiledescriptor is" << mFiledescriptor;
     } else {
         qDebug() << "DrawerObserverThread::open(): Successfully opened port for drawer" << mPath << "mFiledescriptor is" << mFiledescriptor;
         //QString("Opened file descriptor 0x%1 for cash drawer.").arg(uchar(mFiledescriptor),0,16);

         // flushing is to be done after opening. This prevents first read and write to be spam'ish.
         tcflush(mFiledescriptor, TCIOFLUSH);

         int n = fcntl(mFiledescriptor, F_GETFL, 0);
         fcntl(mFiledescriptor, F_SETFL, n & ~O_NDELAY);

         if (tcgetattr(mFiledescriptor, &options) != 0)
         {
            qDebug() << "tcGETattr() 1 failed";
         }

         //int n = fcntl(mFiledescriptor, F_GETFL, 0);
         //fcntl(mFiledescriptor, F_SETFL, n & ~O_NDELAY);

         // Get the current options for the port
         //tcgetattr(mFiledescriptor, &options);

         cfsetospeed(&options, (speed_t)B9600);
         cfsetispeed(&options, (speed_t)B9600); // Set the baud rates

         // null CSIZE and set to 8 bits
         options.c_cflag = (options.c_cflag & ~CSIZE) | CS8;

         // Enable the receiver and set local mode...
         options.c_cflag |= CLOCAL | CREAD;

         // set Parity flags to 0
         options.c_cflag &= ~(PARENB | PARODD);

         // turn off hardware handshake
         options.c_cflag &= ~CRTSCTS;

         // turn off 2 stop bits
         options.c_cflag &= ~CSTOPB;

         // dunno what this does
         options.c_iflag=IGNBRK;

         // turn off software handshake
         options.c_iflag &= ~(IXON|IXOFF|IXANY);

         options.c_lflag=0;
         options.c_oflag=0;

         options.c_cc[VTIME]=1;
         options.c_cc[VMIN]=60;

         if (tcsetattr(mFiledescriptor, TCSANOW, &options)!=0)
         {
            qDebug() << "tcSETattr() 1 failed";
         }

         // ---- no idea
         int mcs=0;
         ioctl(mFiledescriptor, TIOCMGET, &mcs);
         mcs |= TIOCM_RTS;
         ioctl(mFiledescriptor, TIOCMSET, &mcs);

         if (tcgetattr(mFiledescriptor, &options)!=0)
         {
            qDebug() << "tcGETattr() 4 failed";
         }

         // hardware handshake off
         options.c_cflag &= ~CRTSCTS;

         if (tcsetattr(mFiledescriptor, TCSANOW, &options)!=0)
         {
            qDebug() << "tcSETattr() 2 failed";
         }
     }
#endif
#ifdef WIN32
    // TODO: How to read from system printer???
#endif
     qDebug() << "[DrawerObserver]" << "[openDevice] Ending.";
}

void DrawerObserver::closeDevice() {
    qDebug() << "[DrawerObserver]" << "[closeDevice] Beginning.";
    close(mFiledescriptor);
}

void DrawerObserver::readData(int fd) {
    char buf[10];

    QByteArray closedCode = "\x14";
    QByteArray openedCode = "\x10";

    int bytesRead = ::read(mFiledescriptor, buf, 9);

    QByteArray bytes;
    bytes = QByteArray(buf);

    qDebug() << "[DrawerObserver] [readData]" << bytesRead << "bytes read:" << bytes.toHex();

    if (bytes.indexOf(openedCode) != -1) {
        mdrawerOpened = true;
        qDebug() << "[DrawerObserver] [readData] Open Drawer detected";
        return;
    }

    if (mdrawerOpened == false && bytes.indexOf(closedCode) != -1) {
        qDebug() << "[DrawerObserver] [readData] Closed Drawer detected. But it was not yet open, so doing nothing.";
        return;
    }

    if (mdrawerOpened == true && bytes.indexOf(closedCode) != -1) {
        qDebug() << "[DrawerObserver] [readData] Closed Drawer detected after it was open. Stopping.";
        mdrawerClosed = true;
        emit drawerCloseDetected();
        stop();
    }
}

void DrawerObserver::stop() {
    m_notifier->setEnabled(false);
    m_notifier->deleteLater();
    closeDevice();
}


void DrawerObserver::start() {
    int count;
    qDebug() << "[DrawerObserver]" << "[start] Beginning.";
    openDevice();
    char *opencode = "\x1D\x61\x01";
    count = ::write(mFiledescriptor, opencode, 3);
    qDebug() << "Wrote "  << count << " bytes to enable printer feedback.";
    m_notifier = new QSocketNotifier(mFiledescriptor, QSocketNotifier::Read, this);
    connect(m_notifier, SIGNAL(activated(int)), this, SLOT(readData(int)));
}
