#-------------------------------------------------
#
# Project created by QtCreator 2010-08-05T16:39:13
#
#-------------------------------------------------

QT       += core gui

TARGET = Dukto
TEMPLATE = app


SOURCES += main.cpp \
    peer.cpp \
    oslib.cpp \
    mainwindow.cpp \
    listwidgetpeeritem.cpp \
    duktoprotocol.cpp \
    dialogsendip.cpp \
    buddywidget.cpp

HEADERS  += \
    peer.h \
    oslib.h \
    mainwindow.h \
    listwidgetpeeritem.h \
    duktoprotocol.h \
    dialogsendip.h \
    buddywidget.h

FORMS    += \
    mainwindow.ui \
    dialogsendip.ui \
    buddywidget.ui

RESOURCES += \
    resources.qrc

CONFIG += mobility
MOBILITY = systeminfo bearer

symbian {
    TARGET.UID3 = 0xe3a6f8fa
    TARGET.CAPABILITY = "NetworkServices"
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000

    MMP_RULES += "START BITMAP Dukto_icons.mbm" \
"HEADER" \
"SOURCEPATH .\icons" \
"SOURCE c24,1 7050.bmp 7050m.bmp 3939.bmp 3939m.bmp 8670.bmp 8670m.bmp" \
"TARGETPATH \resource\apps" \
"END"

    moreFiles.pkg_postrules = "\"C:\\Qt\NokiaQtSDK\\Symbian\\SDK\\epoc32\\data\\z\\resource\\apps\\Dukto_icons.mbm\" -\"!:\\resource\\apps\\Dukto_icons.mbm\""
    DEPLOYMENT += moreFiles
}

