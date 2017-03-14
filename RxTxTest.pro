QT += core serialport
QT -= gui

CONFIG += c++11

TARGET = RxTxTest
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    qrxtxtest.cpp

HEADERS += \
    qrxtxtest.h
