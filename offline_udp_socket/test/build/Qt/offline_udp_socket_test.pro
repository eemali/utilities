#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = offline_udp_socket_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

DESTDIR = "$$PWD"

HEADERS += \    
    "$$PWD/../../../include/cOfflineUDPSocket.h"
    
SOURCES += \
    "$$PWD/../../../src/cOfflineUDPSocket.cpp" \
    "$$PWD/../../src/offline_udp_socket_test.cpp"
