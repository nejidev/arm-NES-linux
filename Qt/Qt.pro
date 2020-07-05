#-------------------------------------------------
#
# Project created by QtCreator 2020-07-04T15:03:41
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt
TEMPLATE = app


SOURCES += main.cpp \
NesEmulateWindow.cpp \
NesScreenWidget.cpp \
NesThread.cpp \
../K6502.cpp \
../InfoNES.cpp \
../InfoNES_Mapper.cpp \
../InfoNES_pAPU.cpp \
    NesSoundDevice.cpp

HEADERS  += NesEmulateWindow.h \
NesScreenWidget.h \
NesThread.h \
    NesSoundDevice.h

FORMS    += NesEmulateWindow.ui

#VC char is default signed char GCC char is default unsigned char
QMAKE_CXXFLAGS += -O2 -fsigned-char

