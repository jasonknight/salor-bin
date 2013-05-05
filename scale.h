#ifndef SCALE_H
#define SCALE_H

#include "common_includes.h"

class Scale
{
public:
    Scale(const QString path, int protocol);
    char * read();


protected:
    QString mPath;
    int mFiledescriptor;
    int mProtocol;
    char *doSamsungSpain();
    void open();
    void close();
};

#endif // SCALE_H
