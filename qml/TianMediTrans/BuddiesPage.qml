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
    id: buddiesPage
    width: 360
    height: 360
    clip: true

    signal showSettings()
    signal showIP()
    signal showAbout()

    ListView {
        id: buddiesList
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: toolBar.top
        spacing: 10
        anchors.leftMargin: 25
        anchors.rightMargin: 0
        model: buddiesListData // EsempioDati {}

        Component {
             id: contactDelegate
             BuddyListElement {
                 buddyIp: ip
                 buddyAvatar: avatar
                 buddyGeneric: generic
                 buddyUsername: username
                 buddySystem: system
                 buddyOsLogo: oslogo
                 buddyShowBack: showback
             }
         }

        delegate: contactDelegate
    }
    ToolBar {
        id: toolBar
        anchors.bottom: parent.bottom
        tools: ToolBarLayout {

            ToolButton {
                iconSource: "toolbar_close.svg"
                onClicked: Qt.quit()
            }
            ToolButton {
                iconSource: "toolbar-share"
                onClicked: buddiesPage.showIP()
            }
            ToolButton {
                iconSource: "toolbar_about.svg"
                onClicked: buddiesPage.showAbout()
            }
            ToolButton {
                iconSource: "toolbar-settings"
                onClicked: buddiesPage.showSettings()
            }
        }
    }
}
