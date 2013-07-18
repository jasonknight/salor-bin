#include "serialport.h"
#include "common_includes.h"

Serialport::Serialport(QString path, int baudrate) :
    QObject()
{
    m_path = path;

    switch(baudrate) {
    case 50:
        m_baudrate = B50;
        break;
    case 75:
        m_baudrate = B75;
        break;
    case 110:
        m_baudrate = B110;
        break;
    case 134:
        m_baudrate = B134;
        break;
    case 150:
        m_baudrate = B150;
        break;
    case 200:
        m_baudrate = B200;
        break;
    case 300:
        m_baudrate = B300;
        break;
    case 600:
        m_baudrate = B600;
        break;
    case 1200:
        m_baudrate = B1200;
        break;
    case 1800:
        m_baudrate = B1800;
        break;
    case 2400:
        m_baudrate = B2400;
        break;
    case 4800:
        m_baudrate = B4800;
        break;
    case 9600:
        m_baudrate = B9600;
        break;
    case 19200:
        m_baudrate = B19200;
        break;
    case 38400:
        m_baudrate = B38400;
        break;
    case 57600:
        m_baudrate = B57600;
        break;
    case 1152000:
        m_baudrate = B115200;
        break;
    case 230400:
        m_baudrate = B230400;
        break;
    case 460800:
        m_baudrate = B460800;
        break;
    case 500000:
        m_baudrate = B500000;
        break;
    case 576000:
        m_baudrate = B576000;
        break;
    case 921600:
        m_baudrate = B921600;
        break;
    case 1000000:
        m_baudrate = B1000000;
        break;
    default:
        m_baudrate = B9600;
        qDebug() << "[Serialport]" << "[initialize]" << "Invalid baudrate" << baudrate << ". Defaulting to 9600.";
        break;
    }
}

int Serialport::open() {
    qDebug() << "[Serialport]" << "[open]" << "Attempting to open file descriptior with open() at " << m_path;
#ifdef LINUX

    struct termios options;

    m_fd = ::open(m_path.toLatin1(), O_RDWR);
    if (m_fd < 0) {
        qDebug() << "[Serialport]" << "[open]" << "Unable to open port for drawer" << m_path << "m_fd is" << m_fd << "Error is" << strerror(errno);
        return m_fd;
    }

    qDebug() << "[Serialport]" << "[open]" << "Successfully opened port for drawer" << m_path << "m_fd is" << m_fd;

    tcflush(m_fd, TCIOFLUSH);

    // populate the options struct with current terminal settings
    if (tcgetattr(m_fd, &options) < 0)
    {
        qDebug() << "[Serialport]" << "[open]" << "tcGETattr() 1 failed with Error:" << strerror(errno);

    }

    // ====== cflag -- control modes begin ====
    // null CSIZE and set to 8 bits
    options.c_cflag = (options.c_cflag & ~CSIZE) | CS8;

    // ignore modem control lines (DTS, RTS, CTS, etc.)
    options.c_cflag |= CLOCAL;

    // enable the receiver
    options.c_cflag |= CREAD;

    // disable parity bit
    options.c_cflag &= ~(PARENB | PARODD);

    // turn off hardware handshake.
    options.c_cflag &= ~CRTSCTS;

    // turn off 2 stop bits. the default is 1
    options.c_cflag &= ~CSTOPB;
    // ====== cflag end ==========


    // ====== iflag -- input modes begin ====
    // POSIX way of setting input baudrate in the options struct, instead of modifying the struct directly
    cfsetispeed(&options, m_baudrate);

    // Ignore BREAK condition on input. break seems to be a submission
    // of many zeros over a larger period of time. BREAK causes the
    // input and output queues to be flushed, and if the terminal is
    // the controlling terminal of a foreground process group, it
    // will cause a SIGINT to be sent to this foreground process group.
    options.c_iflag=IGNBRK;

    // Ignore framing errors and parity errors.
    options.c_iflag=IGNPAR;

    // disable XON/XOFF software flow control for input, ouput,
    options.c_iflag &= ~(IXON|IXOFF|IXANY);
    // ====== iflag end ==========


    // ====== oflag -- output modes begin ====
    // clear this bytes. it seems to not contain any relevant settings for us.
    options.c_oflag = 0;

    //POSIX way of setting input baudrate in the options struct, instead of modifying the struct directly
    cfsetospeed(&options, m_baudrate);
    // ====== oflag end ==========


    // ====== lflag -- local modes begin ====
    options.c_lflag = 0;
    // ====== lflag end ==========


    // ====== c_cc array --  terminal special characters ===========
    // options.c_cc[VTIME]=1;
    // options.c_cc[VMIN]=60;
    // ====== c_cc end ==========

    // itermediate save
    if ( tcsetattr(m_fd, TCSANOW, &options ) < 0)
    {
        qDebug() << "[Serialport]" << "[open]" << "tcSETattr() 1 failed with Error:" << strerror(errno);
    }

    // --------------------------------
    // get the status of modem bits.
    int mcs = 0;
    if ( ioctl(m_fd, TIOCMGET, &mcs) < 0 ) {
        qDebug() << "[Serialport]" << "[open]" << "ioctl TIOCMGET failed with Error:" << strerror(errno);
    }

    mcs |= TIOCM_RTS; // request to send pin on
    mcs |= TIOCM_CTS; // clear to send pin on
    mcs |= TIOCM_DTR; // data terminal ready pin on

    // set the status of our modified modem bits.
    if ( ioctl(m_fd, TIOCMSET, &mcs) < 0 ) {
        qDebug() << "[Serialport]" << "[open]" << "ioctl TOPCMSET failed with Error:" << strerror(errno);
    }
    // --------------------------------

    if (tcsetattr(m_fd, TCSANOW, &options) < 0 )
    {
        qDebug()  << "[Serialport]" << "[open]" << "tcSETattr() 2 failed with Error:" << strerror(errno);
    }

#endif
#ifdef WIN32
    // TODO
#endif
    qDebug() << "[Serialport]" << "[open]" << "Finished.";
}


// POSIX, modern way of setting O_NONBLOCK. This breaks WRITE() to USB nodes. Nonblock is only useful for READ().
void Serialport::setNonblock() {
    qDebug() << "[Serialport]" << "[setNonblock]";
    if ( fcntl(m_fd, F_SETFL, O_NONBLOCK) < 0 ) {
        qDebug() << "[Serialport]" << "[setNonblock]" << "fcntl() F_SETFL O_NONBLOCK failed with Error:" << strerror(errno);
    }
}

int Serialport::close() {
    qDebug() << "[Serialport]" << "[close]" << "Closing file descriptor" << m_fd;
#ifdef LINUX
    if ( ::close(m_fd) < 0 ) {
        qDebug() << "[Serialport]" << "[close]" << "close() failed with Error:" << strerror(errno);
    }
#endif
#ifdef WIN32
    // TODO
#endif
    qDebug() << "[Serialport]" << "[close]" << "Finished";
}

int Serialport::write(QByteArray bytes) {
    ssize_t written_bytes;
    size_t length;
    const char *constdata;

    length = bytes.size();
    constdata = bytes.constData();

    written_bytes = ::write(m_fd, constdata, length);
    if ( written_bytes < 0 ) {
        qDebug() << "[Serialport]" << "[write]" << "Writing failed with Error:" << strerror(errno);
    }
    qDebug() << "[Serialport]" << "[write]" << "Wrote" << written_bytes << "bytes out of" << bytes.size() << "bytes which were in hex" << bytes.left(10).toHex();
    return written_bytes;
}


QByteArray Serialport::read() {
    char buf[7] = {"\0"};
    int count;
    QByteArray bytes;

    count = ::read(m_fd, &buf, 7);
    if ( count < 0 ) {
        qDebug() << "[Serialport]" << "[read]" << "read() failed with errno" << errno << ", Error:" << strerror(errno);
        return "Error";
    }
    bytes = QByteArray(buf, 7);
    qDebug() << "[Serialport]" << "[read]" << "Read" << count << "bytes which were" << QString(bytes) << "or in hex" << bytes.toHex();
    return bytes;
}
