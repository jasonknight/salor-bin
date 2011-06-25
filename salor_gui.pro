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
    salor_settings.cpp \
    salor_page.cpp \
    salor_web_plugin.cpp \
    salor_plugin_factory.cpp

HEADERS  += mainwindow.h \
    salor_settings.h \
    salor_page.h \
    salor_plugin_factory.h \
    salor_web_plugin.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
