TRANSLATIONS = TianMediTran_vi.ts
# the app version
    VERSION = 1.1.0
#Changes log:
#    Fix UI croping on Nokia E6

# Define the preprocessor macro to get the application version in our application.
    DEFINES += APP_VERSION=\"$$VERSION\"
# This removes extra logging from release builds
release {
    DEFINES += QT_NO_DEBUG_OUTPUT
}
#
# Basic Qt configuration
#
CONFIG += qt-components mobility
MOBILITY += systeminfo gallery

# Allow network access on Symbian
symbian: {
    TARGET.CAPABILITY += NetworkServices ReadUserData WriteUserData ReadDeviceData WriteDeviceData
    LIBS += -lbafl -lmgfetch
}

# Add more folders to ship with the application, here
qml_sources.source = qml/TianMediTrans
qml_sources.target = qml
DEPLOYMENTFOLDERS = qml_sources

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

QT += network

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
#CONFIG += qdeclarative-boostable

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    platform.cpp \
    duktoprotocol.cpp \
    buddylistitemmodel.cpp \
    controller.cpp \
    settings.cpp \
    destinationbuddy.cpp \
    ipaddressitemmodel.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

OTHER_FILES +=

HEADERS += \
    platform.h \
    peer.h \
    duktoprotocol.h \
    buddylistitemmodel.h \
    controller.h \
    settings.h \
    destinationbuddy.h \
    ipaddressitemmodel.h

RESOURCES +=
