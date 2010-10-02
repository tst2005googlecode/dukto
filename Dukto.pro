#-------------------------------------------------
#
# Project created by QtCreator 2010-09-20T18:24:43
#
#-------------------------------------------------

QT       += core gui

TARGET = Dukto
TEMPLATE = app


SOURCES += \
    peer.cpp \
    oslib.cpp \
    mainwindow.cpp \
    main.cpp \
    listwidgetpeeritem.cpp \
    duktoprotocol.cpp \
    dialogsendip.cpp \
    buddywidget.cpp \
    dialogtext.cpp \
    symbiannative.cpp

HEADERS  += \
    peer.h \
    oslib.h \
    mainwindow.h \
    listwidgetpeeritem.h \
    duktoprotocol.h \
    dialogsendip.h \
    buddywidget.h \
    dialogtext.h \
    symbiannative.h

FORMS    += \
    mainwindow.ui \
    dialogsendip.ui \
    buddywidget.ui \
    dialogtext.ui

CONFIG += mobility
MOBILITY = systeminfo bearer

ICON = icons/tinyicon.svg

symbian {
    TARGET.UID3 = 0xec2c0684
    TARGET.CAPABILITY += "NetworkServices"
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
    LIBS = -lcone -lbafl -letext -lestor
}

RESOURCES += \
    resources.qrc
