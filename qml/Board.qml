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
  * Board
  *
  * Represents board with chips.
  */

Item {
    id: boardContainer

    /**
      * Is board showing labels or not
      */
    property bool isShowingLabels: false
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
      * Enables/disables hint at (row, column) cell
      * @param row row index of cell (starting from 0)
      * @param column column index of cell (starting from 0)
      * @param value @c true show hint
      *              @c false hide hint
      */
    function setHint(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).isHint = value
    }

    /**
      * Enables/disables legal marker at (row, column) cell
      * @param row row index of cell (starting from 0)
      * @param column column index of cell (starting from 0)
      * @param value @c true show legal marker
      *              @c false hide legal marker
      */
    function setLegal(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).isLegal = value
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
        cells.itemAt(row * Globals.COLUMN_COUNT + column).chipState = value
    }

    /**
      * Enables/disables last move marker at (row, column) cell
      * @param row row index of cell (starting from 0)
      * @param column column index of cell (starting from 0)
      * @param value @c true show last move marker
      *              @c false hide last move marker
      */
    function setLastMove(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).isLastMove = value
    }
    
    /**
     * Sets time to wait before starting animation
     * @param row row index of cell (starting from 0)
     * @param column column index of cell (starting from 0)
     * @param value time to wait
     */
    function setPreAnimationTime(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).chipPreAnimationTime = value
    }

    CanvasItem {
        id: boardBackground
        anchors.fill: parent
        spriteKey: "board"
    }

    CanvasItem {
        id: boardLabels
        anchors.fill: parent
        visible: isShowingLabels
        spriteKey: "board_numbers"
    }

    Item {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        x: Globals.GRID_OFFSET_X_PERCENT * boardContainer.width
        y: Globals.GRID_OFFSET_Y_PERCENT * boardContainer.height

        width: Globals.GRID_WIDTH_PERCENT * boardContainer.width
        height: Globals.GRID_HEIGHT_PERCENT * boardContainer.height

        Repeater {
            id: cells
            model: Globals.ROW_COUNT * Globals.COLUMN_COUNT

            Cell {
                x: (index % Globals.COLUMN_COUNT)
                   * Globals.GRID_WIDTH_PERCENT
                   * boardContainer.width
                   / Globals.COLUMN_COUNT;
                y: Math.floor(index / Globals.COLUMN_COUNT)
                   * Globals.GRID_HEIGHT_PERCENT
                   * boardContainer.height
                   / Globals.ROW_COUNT;

                width: Globals.GRID_WIDTH_PERCENT * boardContainer.width
                       / Globals.COLUMN_COUNT
                height: Globals.GRID_HEIGHT_PERCENT * boardContainer.height
                        / Globals.ROW_COUNT

                chipImagePrefix: boardContainer.chipsImagePrefix
                chipAnimationTime: boardContainer.chipsAnimationTime

                onClicked: boardContainer.cellClicked(index / Globals.COLUMN_COUNT,
                                                 index % Globals.COLUMN_COUNT)
            }
        }
    }
}
