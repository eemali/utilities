#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = xt_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += \
   "$$PWD/../../../install/nlohmann_json/include" \
   "$$PWD/../../../install/xsimd/include" \
   "$$PWD/../../../install/xtensor/include" \
   "$$PWD/../../../install/xtensor-io/include" \
   "$$PWD/../../../install/xtl/include"   

DESTDIR = "$$PWD"

SOURCES += \
   "$$PWD/../../src/xtensor_test.cpp"
