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
import "globals.js" as Globals

/**
  * Table
  *
  * Represents table with board
  */

Item {
    id: tableContainer
    anchors.fill: parent

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
