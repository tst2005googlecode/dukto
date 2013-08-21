// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.symbian 1.1    // Symbian components

Rectangle {
    id: mainPage
    property bool isNokiaE6: controller.isNokiaE6
    width: (isNokiaE6 ? 640 : 360)
    height: (isNokiaE6 ? 480 : 640)
    //for Nokia E6 testing
    //    width: 640
    //    height: 480
    color: "black"

    Component.onCompleted: {
        sbar.title = "Tian Media Transfer";
    }

    Connections {
        target: controller
        onTransferStart: inner.state = "progress"
        onReceiveCompleted: {
            inner.state = ""
        }
        onGotoSendPage: inner.state = "send";
        onGotoMessagePage: inner.state = "message";
        onHideAllOverlays: inner.state = "";
    }

    Item {
        id: inner
        width: parent.width
        height: parent.height - sbar.height
        anchors.top: sbar.bottom
        anchors.left: parent.left

        BuddiesPage {
            id: buddiesPage
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.bottom: parent.bottom
            width: parent.width
            x: 0
            onShowSettings: {
                if(settingsPage.buddyName != controller.buddyName) settingsPage.buddyName = controller.buddyName;
                inner.state = "settings";
            }
            onShowIP: {
                controller.refreshIpList();
                inner.state = "ip";
            }
            onShowAbout: inner.state = "about"
        }

        Rectangle {
            id: disabler
            color: "gray"
            anchors.fill: parent
            opacity: 0
            MouseArea {
                anchors.fill: parent;
            }
        }

        AboutPage {
            id: aboutPage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width
            x: -50
            opacity: 0
            onBack: parent.state = ""
        }

        IpPage {
            id: ipPage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width
            x: -50
            opacity: 0
            onBack: parent.state = ""
        }

        ProgressPage {
            id: progressPage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width
            x: -50
            opacity: 0
        }

        SettingsPage {
            id: settingsPage
            width: parent.width
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            x: -50
            opacity: 0
            onBack: parent.state = ""
        }

        SendPage {
            id: sendPage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width
            height: parent.height
            x: -50
            opacity: 0
            onBack: parent.state = ""
        }

        MessagePage {
            id: messagePage
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width
            x: -50
            opacity: 0
            onBack: parent.state = backState
        }

        states: [
            State {
                name: "about"
                PropertyChanges {
                    target: sbar
                    title: qsTr("About")
                }
                PropertyChanges {
                    target: buddiesPage
                    opacity: 0
                }
                PropertyChanges {
                    target: aboutPage
                    opacity: 1
                    x: 0
                }
            },State {
                name: "ip"
                PropertyChanges {
                    target: sbar
                    title: qsTr("Your IP addresses")
                }
                PropertyChanges {
                    target: disabler
                    opacity: 0.5
                }
                PropertyChanges {
                    target: ipPage
                    opacity: 1
                    x: 0
                }
            },
            State {
                name: "progress"
                PropertyChanges {
                    target: sbar
                    title: controller.currentTransferSending ? qsTr("Sending data") : qsTr("Receiving data")
                }
                PropertyChanges {
                    target: disabler
                    opacity: 0.5
                }
                PropertyChanges {
                    target: progressPage
                    opacity: 1
                    x: 0
                }
            },
            State {
                name: "settings"
                PropertyChanges {
                    target: sbar
                    title: qsTr("Settings")
                }
                PropertyChanges {
                    target: buddiesPage
                    opacity: 0
                }
                PropertyChanges {
                    target: settingsPage
                    opacity: 1
                    x: 0
                }
            },
            State {
                name: "send"
                PropertyChanges {
                    target: sbar
                    title: qsTr("Send data to")
                }
                PropertyChanges {
                    target: buddiesPage
                    opacity: 0
                }
                PropertyChanges {
                    target: sendPage
                    opacity: 1
                    x: 0
                }
            },
            State {
                name: "message"
                PropertyChanges {
                    target: sbar
                    title: controller.messagePageTitle
                }
                PropertyChanges {
                    target: disabler
                    opacity: 0.5
                }
                PropertyChanges {
                    target: messagePage
                    opacity: 1
                    x: 0
                }
            }
        ]

        transitions: [
            Transition {
                NumberAnimation { property: "x"; easing.type: Easing.OutCubic; duration: 500 }
                NumberAnimation { property: "opacity"; easing.type: Easing.OutCubic; duration: 500 }
            }
        ]
    }

    Binding {
        target: controller
        property: "uiState"
        value: inner.state
    }

    StatusBar {
        id: sbar
        anchors.top: parent.top
        property alias title: statusPaneTitle.text

        Rectangle
        {
            anchors.left: parent.left
            anchors.leftMargin: 2
            anchors.verticalCenter: parent.verticalCenter
            width: sbar.width - 140
            height: parent.height
            clip: true;
            color: "#00000000"

            Text{
                id: statusPaneTitle
                anchors.verticalCenter: parent.verticalCenter
                maximumLineCount: 1
                x: 0
                color: "white"
            }

            Rectangle{
                width: 25
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                rotation: -90
                gradient: Gradient{
                    GradientStop { position: 0.0; color: "#00000000" }
                    GradientStop { position: 1.0; color: "#ff000000" }
                }
            }
        }
    }
}
