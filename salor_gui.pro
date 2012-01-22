#-------------------------------------------------
#
# Project created by QtCreator 2011-05-15T23:01:02
#
#-------------------------------------------------

QT       += core gui webkit network

TARGET = salor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    salor_page.cpp \
    salor_customer_screen.cpp \
    display_link.cpp \
    cashdrawer.cpp \
    paylife.cpp \
    salorprinter.cpp

HEADERS  += mainwindow.h \
    salor_page.h \
    salor_customer_screen.h \
    display_link.h \
    cashdrawer.h \
    scales.h \
    paylife.h \
    paylife_structs.h \
    salorprinter.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
INCLUDEPATH += /usr/include/opencv
LIBS += /usr/lib/libdlo.a -ldlo -L/usr/lib/ -lusb
