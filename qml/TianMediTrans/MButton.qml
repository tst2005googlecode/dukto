// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: rectangle1
    width: 150
    height: 30
    color: "transparent" //"dimgray"
    radius: 0
    border.color: enabled?"lightblue":"gray"
    border.width: 1
    property alias text: txtText.text
    property alias textColor: txtText.color
    Rectangle {
        id: recPressedColor
        color: "lightblue"
        anchors.fill: parent
        opacity: 0
    }

    Text {
        id: txtText
        color: parent.enabled?"white":"gray"
        text: "label"
        anchors.fill: parent
        font.pixelSize: 14
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
    }
    signal clicked();
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: parent.clicked();
    }
    states: [
        State {
            name: "PRESSED"
            when: mouseArea.pressed
            PropertyChanges {
                target: recPressedColor
                opacity: 0.6
            }
        }
    ]
    transitions: [
        Transition {
            to: "*"
            NumberAnimation { target: recPressedColor; property: "opacity"; duration: 100 }
        }
    ]
}
