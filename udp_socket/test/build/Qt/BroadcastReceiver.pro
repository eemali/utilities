#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = BroadcastReceiver
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

win32:LIBS += -lws2_32

DESTDIR = "$$PWD"

HEADERS += \    
    "$$PWD/../../../include/socket.h"
    
SOURCES += \
    "$$PWD/../../../../get_time_of_day/src/gettimeofday.cpp" \
    "$$PWD/../../../src/socket.cpp" \
    "$$PWD/../../src/BroadcastReceiver.cpp"
