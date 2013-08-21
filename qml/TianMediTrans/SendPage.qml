/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2011 Emanuele Colombo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

import QtQuick 1.1
import com.nokia.symbian 1.1    // Symbian components

Item {
    id: sendPage
    width: 360
    height: 360

    signal back()

    BuddyListElement {
        id: localBuddy
        visible: destinationBuddy.ip != "IP"
        anchors.top: parent.top
        anchors.topMargin: 25
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.right: parent.right
        buddyGeneric: destinationBuddy.genericAvatar
        buddyAvatar: destinationBuddy.avatar
        buddyOsLogo: destinationBuddy.osLogo
        buddyUsername: destinationBuddy.username
        buddySystem: destinationBuddy.system
        buddyIp: "-"
    }

    BuddyListElement {
        id: remoteBuddy
        visible: destinationBuddy.ip == "IP"
        anchors.top: parent.top
        anchors.topMargin: 25
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.right: parent.right
        buddyGeneric: "UnknownLogo.png"
        buddyAvatar: ""
        buddyOsLogo: ""
        buddyUsername: qsTr("Destination:")
        buddySystem: ""
        buddyIp: "-"
    }

    MTextBox {
        id: destinationText
        visible: destinationBuddy.ip == "IP"
        anchors.right: buttonSendImage.right//localBuddy.right
        anchors.bottom: localBuddy.bottom
        anchors.bottomMargin: 5
//        anchors.rightMargin: 20
        width: 225
        height: 25

        Binding {
            target: controller
            property: "remoteDestinationAddress"
            value: destinationText.text
        }
    }

    MLabel {
        id: labelAction
        anchors.left: localBuddy.left
        anchors.top: localBuddy.bottom
        anchors.topMargin: 35
        font.pixelSize: 17
        color: "white"
        text: qsTr("What do you want to do?")
    }

    MButton {
        id: buttonSendImage
        anchors.top: labelAction.bottom
        anchors.topMargin: 15
        anchors.left: localBuddy.left
        width: 300
        enabled: controller.currentTransferBuddy !== ""
        text: qsTr("Send images")
        onClicked: controller.sendSomeFiles("image")
    }

    MButton {
        id: buttonSendVideo
        anchors.top: buttonSendImage.bottom
        anchors.topMargin: 15
        anchors.left: localBuddy.left
        width: 300
        enabled: controller.currentTransferBuddy !== ""
        text: qsTr("Send video")
        onClicked: controller.sendSomeFiles("video")
    }

    MButton {
        id: buttonSendAudio
        anchors.top: buttonSendVideo.bottom
        anchors.topMargin: 15
        anchors.left: localBuddy.left
        width: 300
        enabled: controller.currentTransferBuddy !== ""
        text: qsTr("Send audio")
        onClicked: controller.sendSomeFiles("audio")
    }

    MButton {
        id: buttonSendFolder
        anchors.top: buttonSendAudio.bottom
        anchors.topMargin: 15
        anchors.left: localBuddy.left
        width: 300
        enabled: controller.currentTransferBuddy !== ""
        text: qsTr("Send a folder")
        onClicked: controller.sendFolder()
    }

    ToolBar {
        id: toolBar
        anchors.bottom: parent.bottom
        tools: ToolBarLayout {

            ToolButton {
                iconSource: "toolbar-back"
                onClicked: sendPage.back()
            }
        }
    }
}
