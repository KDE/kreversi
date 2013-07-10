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

import QtQuick 1.0
import org.kde.games.core 0.1 as KgCore
import "globals.js" as Globals

Item {
    id: container

    signal cellClicked(int row, int column)

    function setHint(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).setHint(value)
    }

    function setLegal(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).setLegal(value)
    }

    function setChipState(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).setChipState(
                    value)
    }

    function setLabels(show) {
        board_labels.visible = show;
    }

    function setChipsPrefix(prefix) {
        for (var i = 0; i < Globals.ROW_COUNT; i++)
            for (var j = 0; j < Globals.COLUMN_COUNT; j++)
                cells.itemAt(i * Globals.COLUMN_COUNT + j).setChipPrefix(
                            prefix)
    }

    function setLastMove(row, column, value) {
        cells.itemAt(row * Globals.COLUMN_COUNT + column).setLastMove(value)
    }

    function setAnimationTime(value) {
        for (var i = 0; i < Globals.ROW_COUNT; i++)
            for (var j = 0; j < Globals.COLUMN_COUNT; j++)
                cells.itemAt(i * Globals.COLUMN_COUNT + j).setAnimationTime(
                            value)
    }

    KgCore.KgItem {
        id: board_background
        z: 0
        anchors.fill: parent
        provider: themeProvider
        spriteKey: "board"
    }

    KgCore.KgItem {
        id: board_labels
        z: 0
        anchors.fill: parent
        visible: false
        provider: themeProvider
        spriteKey: "board_numbers"
    }

    Item {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        x: Globals.GRID_OFFSET_X_PERCENT * container.width
        y: Globals.GRID_OFFSET_Y_PERCENT * container.height

        width: Globals.GRID_WIDTH_PERCENT * container.width
        height: Globals.GRID_HEIGHT_PERCENT * container.height

        Repeater {
            id: cells
            model: Globals.ROW_COUNT * Globals.COLUMN_COUNT

            Cell {
                x: (index % Globals.COLUMN_COUNT)
                   * Globals.GRID_WIDTH_PERCENT
                   * container.width
                   / Globals.COLUMN_COUNT;
                y: Math.floor(index / Globals.COLUMN_COUNT)
                   * Globals.GRID_HEIGHT_PERCENT
                   * container.height
                   / Globals.ROW_COUNT;

                width: Globals.GRID_WIDTH_PERCENT * container.width
                       / Globals.COLUMN_COUNT
                height: Globals.GRID_HEIGHT_PERCENT * container.height
                        / Globals.ROW_COUNT

                onClicked: container.cellClicked(index / Globals.COLUMN_COUNT,
                                                 index % Globals.COLUMN_COUNT)
            }
        }
    }
}
