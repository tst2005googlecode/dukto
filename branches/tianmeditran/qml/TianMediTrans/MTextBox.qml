// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: rectangle1
    width: 100
    height: textInput.height + 10
    color: "transparent" //"gray"
    border.color: "white"
    property alias text: textInput.text
    property alias textColor: textInput.color
    property alias label: mLabel.text
    property alias font: textInput.font
    property alias selectByMouse: textInput.selectByMouse
    property alias readOnly: textInput.readOnly

    MLabel {
        id: mLabel
        visible: (parent.text == "")
        opacity: 0.5
        color: parent.textColor
        text: qsTr("Input...")
        anchors.rightMargin: 5
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.left: parent.left
        font.pixelSize: parent.font.pixelSize
    }

    TextInput {
        id: textInput
        color: parent.enabled?"white":"gray"
        font.pixelSize: 14
        anchors.rightMargin: 5
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.left: parent.left
        focus: parent.focus
    }
    states: [
        State {
            name: "ENTER"
            when: textInput.focus
            PropertyChanges {
                target: rectangle1
                border.color: "lightblue"
            }
        }
    ]
    transitions: [
        Transition {
            to: "*"
            ColorAnimation { target: rectangle1; duration: 100 }
        }
    ]
}
