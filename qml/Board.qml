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

Item {
    id: boardContainer
    property bool isShowingLabels: false
    property string chipsImagePrefix: "chip_bw"
    property int chipsAnimationTime: 25 * 12

    signal cellClicked(int row, int column)

    function setHint(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).isHint = value
    }

    function setLegal(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).isLegal = value
    }

    function setChipState(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).chipState = value
    }

    function setLastMove(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).isLastMove = value
    }
    
    function setPreAnimationTicks(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).chipPreAnimationTicks = value
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
