QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HTYMap
TEMPLATE = app

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    tileitem.cpp

HEADERS += \
        mainwindow.h \
    tileitem.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    res.qrc