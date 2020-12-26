/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.3
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
