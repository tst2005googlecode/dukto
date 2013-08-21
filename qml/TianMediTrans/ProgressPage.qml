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
    id: progressPage
    width: 360
    height: 360

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        height: 200
        color: "black"

        MLabel {
            id: boxTitle
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 15
            anchors.topMargin: 5
            font.pixelSize: 30
            text: controller.currentTransferSending ? qsTr("Sending data") : qsTr("Receiving data")
        }

        MLabel {
            id: boxSender
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 17
            anchors.right: progressBar.right
            anchors.topMargin: 45
            elide: "ElideRight"
            font.pixelSize: 16
            text: (controller.currentTransferSending ? qsTr("to ") : qsTr("from ")) + controller.currentTransferBuddy
        }

        Rectangle {
            id: progressBar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: boxSender.bottom
            anchors.leftMargin: 17
            anchors.rightMargin: 17
            anchors.topMargin: 25
            color: "gray"
            height: 40

            Rectangle {
                id: progressFiller
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                color: "lightblue"
                width: parent.width * controller.currentTransferProgress / 100;
            }
        }

        MButton {
            id: abortButton
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: 17
            anchors.bottomMargin: 17
            width: 75
            text: qsTr("Abort")
            onClicked: controller.abortTransfer()
            visible: controller.currentTransferSending
        }

        MLabel {
            id: statusText
            anchors.left: parent.left
            anchors.right: abortButton.left
            anchors.verticalCenter: abortButton.verticalCenter
            anchors.leftMargin: 17
            anchors.rightMargin: 17            
            font.pixelSize: 17
            elide: "ElideRight"
            text: controller.currentTransferStats
        }
    }
}
