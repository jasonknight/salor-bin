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
    cashdrawer.cpp \
    salorprinter.cpp \
    salorcookiejar.cpp \
    salorprocess.cpp \
    salorjsapi.cpp \
    cutecredit.cpp \
    optionsdialog.cpp \
    network.cpp

HEADERS  += mainwindow.h \
    salor_page.h \
    salor_customer_screen.h \
    cashdrawer.h \
    scales.h \
    salorprinter.h \
    salorcookiejar.h \
    salorprocess.h \
    salorjsapi.h \
    cutecredit.h \
    common_includes.h \
    optionsdialog.h \
    network.h \
    paths.h

FORMS    += mainwindow.ui \
    optionsdialog.ui

RESOURCES += \
    resources.qrc
macx {
    LIBS += -lcups
}
win32 {
    LIBS += -lwinspool -lgdi32 -luser32 -lcomdlg32 -lkernel32
}
