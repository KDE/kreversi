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

Item {
    id: cellContainer
    signal clicked
    property bool isLastMove: false
    property bool isLegal: false
    property bool isHint: false
    property string chipImagePrefix: boardContainer.chipsImagePrefix
    property int chipAnimationTime: boardContainer.chipsAnimationTime
    property string chipState: ""

    KgCore.KgItem {
        id: cellLegalImage
        z: 1
        anchors.fill: parent
        visible: isLegal
        provider: themeProvider
        spriteKey: "move_hint"
    }

    Rectangle {
        id: cellLastMoveMarker;
        z: 1
        visible: isLastMove
        anchors.fill: parent
        color: "#AAAAAA"
    }

    Chip {
        id: reversiChip
        z: 2
        anchors.fill: parent

        state: chipState + (isHint ? "_blinking" : "")

        onClicked: cellContainer.clicked()
    }
}
