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

Item {
    id: cellContainer
    signal clicked
    property bool isLastMove: false
    property bool isLegal: false
    property bool isHint: false
    property string chipImagePrefix: boardContainer.chipsImagePrefix
    property int chipAnimationTime: boardContainer.chipsAnimationTime
    property string chipState: ""

    CanvasItem {
        id: cellLegalImage
        anchors.fill: parent
        visible: isLegal
        spriteKey: "move_hint"
    }

    Rectangle {
        id: cellLastMoveMarker;
        visible: isLastMove
        anchors.fill: parent
        color: "#AAAAAA"
    }

    Chip {
        id: reversiChip
        anchors.fill: parent

        state: chipState + (isHint ? "_blinking" : "")

        onClicked: cellContainer.clicked()
    }
}
