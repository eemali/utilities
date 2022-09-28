#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = get_time_of_day_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

DESTDIR = "$$PWD"

HEADERS += \    
    "$$PWD/../../../include/gettimeofday.h"
    
SOURCES += \
    "$$PWD/../../../src/gettimeofday.cpp" \
    "$$PWD/../../src/get_time_of_day_test.cpp"
