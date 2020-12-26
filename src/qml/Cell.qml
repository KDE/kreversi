/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.3

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
