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

/**
  * Cell
  *
  * Represents board's single cell.
  */

Item {
    id: cellContainer
    /**
      * Emitted when user clicks this cell
      */
    signal clicked
    /**
      * Is cell showing last move marker or not
      */
    property bool isLastMove: false
    /**
      * Is cell showing legal move marker or not
      */
    property bool isLegal: false
    /**
      * Is cell showing hint move marker or not
      */
    property bool isHint: false
    /**
      * Chips image's ID prefix at SVG theme file to use.
      */
    property string chipImagePrefix: "chip_bw"
    /**
      * Duration of chip's turning animation
      */
    property int chipAnimationTime: 25 * 12
    /**
     * How long to wait before start of animation
     */
    property int chipPreAnimationTime: 0
    /**
      * Chips state:
      * "Black" turns chip to black side
      * "White" turns chip to white side
      * "" (empty string) hides chip
      */
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

        imagePrefix: parent.chipImagePrefix + "_"
        animationTime: parent.chipAnimationTime
        preAnimationTime: parent.chipPreAnimationTime
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: cellContainer.clicked()
    }
}
