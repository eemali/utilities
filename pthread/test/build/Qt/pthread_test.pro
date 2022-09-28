#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = pthread_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

DESTDIR = "$$PWD"

SOURCES += "$$PWD/../../src/pthread_test.cpp"
