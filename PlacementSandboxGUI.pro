#-------------------------------------------------
#
# Project created by QtCreator 2015-09-14T17:08:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PlacementSandboxGUI
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    mycanvas.cpp \
    flute_new.cpp

HEADERS  += mainwindow.h \
    mycanvas.h \
    flute.h

FORMS    += mainwindow.ui
