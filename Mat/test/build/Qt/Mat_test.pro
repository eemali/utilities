#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = Mat_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp

QMAKE_CXXFLAGS += -std=c++0x

DESTDIR = "$$PWD"

HEADERS += \    
    "$$PWD/../../../include/Mat.h"
SOURCES += \
	"$$PWD/../../../src/Mat.cpp" \
	"$$PWD/../../src/Mat_test.cpp"
