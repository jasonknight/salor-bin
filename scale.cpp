#include "scale.h"
#include "common_includes.h"
#include <unistd.h>

Scale::Scale(QString path, int protocol)
{
    mPath = path;
    mProtocol = protocol;
    mFiledescriptor = 5;
    qDebug() << "Scale::Scale() initialize: path" << path << "protocol";
}

char * Scale::read() {
    char * weight;
    open();
    if(mFiledescriptor == -1) {
        return("XXX");
    }
    switch (mProtocol) {
      case 0:
        weight = doSamsungSpain();
        break;
      default:
        qDebug() << "Scale::read(): This protocol number is not implemented.";
    }
    close();
    //return "10.3";
    return weight;
}

void Scale::open() {
#ifdef LINUX
     struct termios options;

     mFiledescriptor = ::open(mPath.toLatin1().data(), O_RDWR | O_NOCTTY | O_NDELAY);
     if (mFiledescriptor == -1) {
         qDebug() << "Scale::open(): Unable to open port for scale" << mPath;
     } else {
         qDebug() << "Opened port for scale" << mPath;
         tcgetattr(mFiledescriptor, &options); // Get the current options for the port...
         cfsetispeed(&options, B9600); // Set the baud rates
         cfsetospeed(&options, B9600);
         options.c_cflag |= (CLOCAL | CREAD); // Enable the receiver and set local mode...
         tcsetattr(mFiledescriptor, TCSANOW, &options); // Set the new options for the port...
     }
#endif
}

void Scale::close() {
    ::close(mFiledescriptor);
}


char * Scale::doSamsungSpain() {
    char buffer[10];
    int count;

    qDebug() << "Scale::samsungSpain(): filedescriptor" << mFiledescriptor;

    ::write(mFiledescriptor, "$", 1);
    usleep(100000); // sleep 100ms until bytes are in the buffer. 50ms works too.
    count = ::read(mFiledescriptor, buffer, 7);
    qDebug() << "Scale::samsungSpain(): read" << QString(*buffer);
    //int i;
    //for (i=0;i<count;i++) { printf("%X|",*(buffer+i)); } // debug
    return buffer;
}
