#-------------------------------------------------
#
# Project created by QtCreator 2010-08-10T18:30:38
#
#-------------------------------------------------

QT       += core gui

TARGET = Example
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    ecwin7.cpp

HEADERS  += mainwindow.h \
    ecwin7.h

FORMS    += mainwindow.ui

RC_FILE  = example.rc

# -- NEEDED BY EcWin7 LIBRARY
LIBS += libole32
