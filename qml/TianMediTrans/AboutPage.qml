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
    id: aboutPage
    width: 360
    height: 360

    signal back()

    Rectangle {
        id: recLogo
        anchors.top: parent.top
        anchors.topMargin: 15
        anchors.left: parent.left
        anchors.leftMargin: 27
        width: 128
        height: 128
        color: "transparent"//"lightblue"
        Image {
            source: "TianMediTranIcon.png"
            anchors.fill: parent
        }
    }

    MLabel {
        id: l1
        anchors.top: recLogo.bottom
        anchors.topMargin: 10
        anchors.left: recLogo.left
        font.pixelSize: 30
        text: ("Tian Media Transfer")
    }
    MLabel {
        id: l2
        anchors.top: l1.bottom
        anchors.topMargin: 10
        anchors.left: recLogo.left
        font.pixelSize: 16
        text: qsTr("Based on Dukto R6\nCreated by Trong Pham/Mr. Tian")
    }
    MLabel {
        id: l3
        anchors.top: l2.bottom
        anchors.topMargin: 10
        anchors.left: recLogo.left
        text: qsTr("Website: <a href=\"http://tianpham.blogspot.com/\">tianpham.blogspot.com/</a>")
        onLinkActivated: Qt.openUrlExternally(link)
    }
    MLabel {
        id: l4
        anchors.top: l3.bottom
        anchors.topMargin: 10
        anchors.left: recLogo.left
        anchors.right: parent.right
        text: qsTr("How to use: Connect two devices you want to transfer data to a WIFI connection. Then run Tian Media Transfer/Dukto R6 on these two, they will recognize each other and be ready to transfer data.")
        wrapMode: Text.WordWrap
        onLinkActivated: Qt.openUrlExternally(link)
    }

    ToolBar {
        id: toolBar
        anchors.bottom: parent.bottom
        tools: ToolBarLayout {

            ToolButton {
                iconSource: "toolbar-back"
                onClicked: aboutPage.back()
            }
        }
    }
}
