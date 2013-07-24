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
import ColorScheme 1.0

Rectangle {
    id: popupContainer

    height: row.height + 30
    width: row.width + 30

    color: colorScheme.background
    opacity: 0.9
    border.color: colorScheme.border
    border.width: 1

    function show(message) {
        text.text = message
        timer.start();
        state = "SHOWING"
    }

    ColorScheme {
        id: colorScheme
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

        Image {
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            width: visible ? 32 : 0
            height: visible ? 32 : 0

            property string iconName: "dialog-information"
            source: "image://icon/"+iconName
        }


        Text {
            id: text
            anchors.verticalCenter: parent.verticalCenter
            color: colorScheme.foreground
            wrapMode: Text.WordWrap
        }
    }
}
