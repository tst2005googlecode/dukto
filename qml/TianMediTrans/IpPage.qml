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
    id: ipPage
    width: 360
    height: 360

    signal back()

    Item {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        height: 200

        Rectangle {
            id: backRecangle
            color: "black"
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
        }

        MLabel {
            id: boxTitle
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 15
            anchors.topMargin: 5
            font.pixelSize: 30
            text: qsTr("Your IP addresses")
        }

        ListView {
            anchors.top: boxTitle.bottom
            anchors.left: boxTitle.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.topMargin: 20
            anchors.bottomMargin: 10
            clip: true

            model: ipAddressesData /*ListModel {
                ListElement { ip: "192.168.92.32" }
                ListElement { ip: "182.23.2.1" }
                ListElement { ip: "169.0.3.2" }
            } */

            Component {
                 id: ipDelegate
                 MLabel {
                     text: ip
                     font.pixelSize: 17
                 }
            }

            delegate: ipDelegate
        }
    }

    ToolBar {
        id: toolBar
        anchors.bottom: parent.bottom
        tools: ToolBarLayout {

            ToolButton {
                iconSource: "toolbar-back"
                onClicked: ipPage.back()
            }
        }
    }
}
