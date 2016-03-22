#-------------------------------------------------
#
# Project created by QtCreator 2013-12-30T04:24:01
#
#-------------------------------------------------

QT       += core gui svg network

include(../base/base.pri)
include(../gui/gui.pri)
DESTDIR = $$PWD

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = livius
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    liveinfo.cpp \
    liveframe.cpp \
    chatinfo.cpp \
    tlcvclient.cpp \
    connectiondialog.cpp \
    resultsdialog.cpp \
    emailgamedialog.cpp \
    pgndialog.cpp \
    chathighlight.cpp \
    aboutdialog.cpp \
    debugconsoledialog.cpp

HEADERS  += mainwindow.h \
    liveinfo.h \
    liveframe.h \
    chatinfo.h \
    tlcvclient.h \
    connectiondialog.h \
    resultsdialog.h \
    emailgamedialog.h \
    pgndialog.h \
    chathighlight.h \
    aboutdialog.h \
    debugconsoledialog.h \
    ack.h

FORMS    += mainwindow.ui \
    liveinfo.ui \
    chatinfo.ui \
    connectiondialog.ui \
    resultsdialog.ui \
    emailgamedialog.ui \
    pgndialog.ui \
    aboutdialog.ui \
    debugconsoledialog.ui

RESOURCES += \
    livius.qrc
