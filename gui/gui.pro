#-------------------------------------------------
#
# Project created by QtCreator 2013-12-30T04:08:57
#
#-------------------------------------------------

QT       += core gui svg

TARGET = gui
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$PWD
include(../base/base.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    pieceset.cpp \
    chessboard.cpp

HEADERS += \
    pieceset.h \
    chessboard.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES +=
