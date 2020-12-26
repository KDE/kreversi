/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.3
import "globals.js" as Globals

/**
  * Table
  *
  * Represents table with board
  */

Item {
    id: tableContainer

    /**
      * Is board showing labels or not
      */
    property bool isBoardShowingLabels: false
    /**
      * Chips image's ID prefix at SVG theme file to use.
      */
    property string chipsImagePrefix: "chip_bw"
    /**
      * Duration of chip's turning animation
      */
    property int chipsAnimationTime: 25 * 12
    /**
      * Emitted when user clicks (row, column) cell
      * @param row row index of cell (starting from 0)
      * @param column column index of cell (starting from 0)
      */
    signal cellClicked(int row, int column)
    /**
      * Enables/disables legal marker at (row, column) cell
      * @param row row index of cell (starting from 0)
      * @param column column index of cell (starting from 0)
      * @param value @c true show legal marker
      *              @c false hide legal marker
      */
    function setLegal(row, column, value) {
        board.setLegal(row, column, value);
    }
    /**
      * Turn chip to White/Black at (row, column) cell
      * @param row row index of cell (starting from 0)
      * @param column column index of cell (starting from 0)
      * @param value @c "Black" turn chip to black
      *              @c "White turn chip to white
      *              @c "" (empty string) hide chip
      */
    function setChipState(row, column, value) {
        board.setChipState(row, column, value);
    }
    /**
      * Enables/disables hint at (row, column) cell
      * @param row row index of cell (starting from 0)
      * @param column column index of cell (starting from 0)
      * @param value @c true show hint
      *              @c false hide hint
      */
    function setHint(row, col, show) {
        board.setHint(row, col, show);
    }
    /**
      * Enables/disables last move marker at (row, column) cell
      * @param row row index of cell (starting from 0)
      * @param column column index of cell (starting from 0)
      * @param value @c true show last move marker
      *              @c false hide last move marker
      */
    function setLastMove(row, column, value) {
        board.setLastMove(row, column, value)
    }
    /**
     * Sets time to wait before starting animation
     * @param row row index of cell (starting from 0)
     * @param column column index of cell (starting from 0)
     * @param value time to wait
     */
    function setPreAnimationTime(row, column, value) {
        board.setPreAnimationTime(row, column, value);
    }
    /**
      * Shows popup with specified text
      * @param text Text to show
      */
    function showPopup(text) {
        popup.show(text, "SHOWING");
    }

    CanvasItem {
        id: table_background
        anchors.fill: parent
        spriteKey: "background"
    }

    CanvasItem {
        id: board_image
        width: Math.min(parent.width, parent.height)
        height: Math.min(parent.width, parent.height)
        anchors.horizontalCenter: parent.horizontalCenter
        spriteKey: "board"
    }

    Board {
        id: board
        width: Math.min(parent.width, parent.height)
        height: Math.min(parent.width, parent.height)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        isShowingLabels: parent.isBoardShowingLabels
        chipsImagePrefix: parent.chipsImagePrefix
        chipsAnimationTime: parent.chipsAnimationTime

        onCellClicked: tableContainer.cellClicked(row, column)
    }

    Popup {
        id: popup

        anchors.bottom: undefined
        anchors.top: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.bottomMargin: 5

        opacity: 0.9
        isReplacing: true

        states: [
            State {
                name: "SHOWING"

                AnchorChanges {
                    target: popup
                    anchors.bottom: popup.parent.bottom
                    anchors.top: undefined
                }
            }
        ]

        transitions: [
            Transition {
                from: ""
                to: "SHOWING"
                reversible: true

                AnchorAnimation {

                    duration: 300
                }
           }
        ]
    }
}
