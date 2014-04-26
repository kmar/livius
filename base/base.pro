#-------------------------------------------------
#
# Project created by QtCreator 2013-12-20T11:30:04
#
#-------------------------------------------------

QT       += core

TARGET = base
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$PWD

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    chess/zobrist.cpp \
    chess/tables.cpp \
    chess/movegen.cpp \
    chess/move.cpp \
    chess/magic.cpp \
    chess/board.cpp \
    pgn/pgn.cpp \
    chess/chess.cpp \
    config/token.cpp \
    config/config.cpp \
    core/utils.cpp \
    core/thread.cpp \
    core/timer.cpp \
    core/apppath.cpp \
    pgn/pgnhighlight.cpp

HEADERS += \
    chess/zobrist.h \
    chess/tables.h \
    chess/movegen.h \
    chess/move.h \
    chess/magic.h \
    chess/chtypes.h \
    chess/chess.h \
    chess/board.h \
    pgn/pgn.h \
    sig/slotbase.h \
    sig/slot11.h \
    sig/slot.h \
    sig/signalbase.h \
    sig/signal11.h \
    sig/signal.h \
    sig/connection.h \
    config/token.h \
    config/config.h \
    core/utils.h \
    core/types.h \
    core/thread.h \
    core/prng.h \
    core/timer.h \
    core/apppath.h \
    pgn/pgnhighlight.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
