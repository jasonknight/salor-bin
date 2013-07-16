#include "serialport.h"
#include "common_includes.h"

Serialport::Serialport(QString path) :
    QObject()
{
    m_path = path;
}

int Serialport::open() {
    qDebug() << "[Serialport]" << "[open]" << "Attempting to open file descriptior with open() at " << m_path;
#ifdef LINUX

    struct termios options;

    m_fd = ::open(m_path.toLatin1(), O_RDWR | O_NDELAY);
    if (m_fd < 0) {
        qDebug() << "[Serialport]" << "[open]" << "Unable to open port for drawer" << m_path << "m_fd is" << m_fd;
        return m_fd;
    }

    qDebug() << "[Serialport]" << "[open]" << "Successfully opened port for drawer" << m_path << "m_fd is" << m_fd;

    // flushing is to be done after opening. This prevents first read and write to be spam'ish.
    tcflush(m_fd, TCIOFLUSH);

    // populate the options struct with current terminal settings
    if (tcgetattr(m_fd, &options) != 0)
    {
        qDebug() << "tcGETattr() 1 failed";
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
    cfsetispeed(&options, (speed_t)B9600);

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
    cfsetospeed(&options, (speed_t)B9600);
    // ====== oflag end ==========


    // ====== lflag -- local modes begin ====
    options.c_lflag = 0;
    // ====== lflag end ==========


    // ====== c_cc array --  terminal special characters ===========
    // options.c_cc[VTIME]=1;
    // options.c_cc[VMIN]=60;
    // ====== c_cc end ==========

    // itermediate save
    if (tcsetattr(m_fd, TCSANOW, &options) != 0)
    {
        qDebug() << "tcSETattr() 1 failed";
    }

    // --------------------------------
    // get the status of modem bits.
    int mcs=0;
    ioctl(m_fd, TIOCMGET, &mcs);

    mcs |= TIOCM_RTS; // request to send pin on
    mcs |= TIOCM_CTS; // clear to send pin on
    mcs |= TIOCM_DTR; // data terminal ready pin on

    // set the status of our modified modem bits.
    ioctl(m_fd, TIOCMSET, &mcs);
    // --------------------------------

    /*
    if (tcgetattr(mFiledescriptor, &options) !=0 )
    {
        qDebug() << "tcGETattr() failed";
    }
    */

    if (tcsetattr(m_fd, TCSANOW, &options) !=0 )
    {
        qDebug() << "tcSETattr() 2 failed";
    }

#endif
#ifdef WIN32
    // TODO
#endif
    qDebug() << "[Serialport]" << "[open]" << "Done.";
}

int Serialport::close() {
    qDebug() << "[Serialport]" << "[close]" << "Closing file descriptor" << m_fd;
#ifdef LINUX
    return ::close(m_fd);
#endif
#ifdef WIN32
    // TODO
#endif
    qDebug() << "[Serialport]" << "[close]" << "Done";
}

int Serialport::write(QByteArray bytes) {
    int length = bytes.size();
    int written_bytes;
    written_bytes = ::write(m_fd, bytes.data(), length);
    qDebug() << "[Serialport]" << "[write]" << "Wrote" << length << "bytes which were" << QString(bytes) << "or in hex" << bytes.toHex();
    return written_bytes;
}

// this reads the amount of bytes in the serial hardware buffer, maximum 2024 bytes
QByteArray Serialport::read() {
    char buf[1024] = {"\0"};
    int count;
    QByteArray bytes;

    count = ::read(m_fd, &buf, 1000);
    bytes = QByteArray(buf);
    qDebug() << "[Serialport]" << "[read]" << "Read" << count << "bytes which were" << QString(bytes) << "or in hex" << bytes.toHex();
    return bytes;
}
