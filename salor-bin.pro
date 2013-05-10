#-------------------------------------------------
#
# Project created by QtCreator 2013-05-02T11:10:26
#
#-------------------------------------------------

QT       += core gui webkit network

# Don't commit a change to the following line into a repository.
LIBS += -lwinspool

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = salor-bin
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    salorpage.cpp \
    salornetwork.cpp \
    salorjsapi.cpp \
    salorprocess.cpp \
    salorprinter.cpp \
    optionsdialog.cpp \
    salorcookiejar.cpp \
    downloader.cpp \
    cutecredit.cpp \
    customerscreen.cpp \
    salorcapture.cpp \
    drawerobserver.cpp \
    scale.cpp \
    salornotificator.cpp

HEADERS  += mainwindow.h \
    salorpage.h \
    salornetwork.h \
    common_includes.h \
    salorjsapi.h \
    salorprocess.h \
    salorprinter.h \
    optionsdialog.h \
    salorcookiejar.h \
    downloader.h \
    cutecredit.h \
    customerscreen.h \
    salorcapture.h \
    drawerobserver.h \
    scale.h \
    salornotificator.h

FORMS    += mainwindow.ui \
    optionsdialog.ui
