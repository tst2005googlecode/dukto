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
    id: settingsPage
    width: 360
    height: 360
    property alias buddyName: txtName.text

    signal back()

    MLabel {
        id: labelPath
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.leftMargin: 17
        font.pixelSize: 16
        text: qsTr("Save received file in:")
        color: "white"
    }

    MTextBox {
        id: textPath
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: labelPath.bottom
        anchors.leftMargin: 17
        anchors.rightMargin: 17
        anchors.topMargin: 8
        height: 30
        clip: true
        text: controller.currentPath
        readOnly: true
    }

    MButton {
        id: buttonPath
        anchors.right: parent.right
        anchors.rightMargin: 17
        anchors.top: textPath.bottom
        anchors.topMargin: 10
        text: qsTr("Change folder")
        onClicked: controller.changeDestinationFolder()
    }

    MLabel {
        id: lbName
        anchors.left: parent.left
        anchors.leftMargin: 17
        anchors.top: buttonPath.bottom
        anchors.topMargin: 10
        font.pixelSize: 16
        text: qsTr("Appear to your buddy as:")
        color: "white"
    }

    MTextBox {
        id: txtName
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: lbName.bottom
        anchors.leftMargin: 17
        anchors.rightMargin: 17
        anchors.topMargin: 8
        height: 30
        clip: true

        onTextChanged: if(text != controller.buddyName) controller.buddyName = text;
    }
    ToolBar {
        id: toolBar
        anchors.bottom: parent.bottom
        tools: ToolBarLayout {

            ToolButton {
                iconSource: "toolbar-back"
                onClicked: settingsPage.back()
            }
        }
    }
}
