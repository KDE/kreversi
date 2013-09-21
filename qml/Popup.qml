/*
    Copyright 2012 Viranch Mehta <viranch.mehta@gmail.com>
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
    id: popup

    // constants
    property int marginOnSides: 15
    property int marginIconText: 10

    // API properties
    property int timeout: 3000
    property alias sharpness: popup.radius
    property string iconName: "dialog-information"
    property alias showIcon: icon.visible
    property alias hideOnMouseClick: mouseArea.enabled
    property bool useCustomTextColor: false
    property color textColor: "white"
    property bool useCustomBackgroundColor: false
    property color backgroundColor: "black"
    property bool useCustomBorderColor: false
    property color borderColor: "black"
    property int borderWidth: 1
    property bool isReplacing: false

    // API signals
    signal linkActivated(string link)
    signal hidden()

    // private properties
    height: row.height + 2 * marginOnSides
    width: row.width + 2 * marginOnSides

    color: useCustomBackgroundColor ? backgroundColor : colorScheme.background
    border.color: useCustomBorderColor ? borderColor : colorScheme.border
    border.width: borderWidth

    function show(message, showing_state) {
        if (!timer.running || isReplacing) {
            text.text = message
            timer.restart()
            state = showing_state
        }
    }

    ColorScheme {
        id: colorScheme
    }

    Timer {
        id: timer
        repeat: false
        interval: timeout
        onTriggered: {
            state = ""
            hidden()
        }
    }

    Row {
        id: row
        x: marginOnSides
        y: marginOnSides
        spacing: marginIconText

        Image {
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            width: visible ? 32 : 0
            height: visible ? 32 : 0

            source: "image://icon/" + iconName
        }


        Text {
            id: text
            anchors.verticalCenter: parent.verticalCenter
            color: colorScheme.foreground
            wrapMode: Text.WordWrap

            onLinkActivated: popup.linkActivated(link);
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            popup.state = ""
            timer.stop()
            hidden()
        }
    }
}
