/*
    Copyright 2013 Denis Kuplyakov <dener.kup@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 1.1
import org.kde.games.core 0.1 as KgCore
import org.kde.plasma.core 0.1 as PlasmaCore

Rectangle {
    id: popupContainer
    height: row.height + 30
    width: row.width + 30
    anchors.bottom: undefined
    anchors.top: parent.bottom
    anchors.left: parent.left
    anchors.leftMargin: 5
    anchors.bottomMargin: 5


    color: "#111111"
    opacity: 0.9
    border.color: "#111111"
    border.width: 1

    function show(message) {
        text.text = message
        timer.start();
        state = "Showing"
    }

    Timer {
        id: timer
        repeat: false
        interval: 3000
        onTriggered: state = ""
    }

    Row {
        id: row
        x: 15
        y: 15
        spacing: 10

        PlasmaCore.IconItem {
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            width: theme.iconSizes.dialog
            height: width
            source: "dialog-information"
        }


        Text {
            id: text
            anchors.verticalCenter: parent.verticalCenter
            color: "white"
            wrapMode: Text.WordWrap
        }
    }

    states: [
        State {
            name: "Showing"

            AnchorChanges {
                target: popupContainer
                anchors.bottom: popupContainer.parent.bottom
                anchors.top: undefined
            }
        }
    ]

    transitions: [
        Transition {

            AnchorAnimation {
                duration: 300
            }
        }
    ]
}
