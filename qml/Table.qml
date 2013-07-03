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
    anchors.fill: parent
    id: container

    signal cellClicked(int row, int column)

    function setLegal(row, column, value) {
        board.setLegal(row, column, value);
    }

    function setChipState(row, column, value) {
        board.setChipState(row, column, value);
    }

    function setLabels(show) {
        board.setLabels(show);
    }

    function setHint(row, col, show) {
        board.setHint(row, col, show);
    }

    function setChipsPrefix(prefix) {
        board.setChipsPrefix(prefix);
    }

    function setLastMove(row, column, value) {
        board.setLastMove(row, column, value)
    }

    function setAnimationTime(value) {
        board.setAnimationTime(value)
    }

    function showPopup(text) {}

    KgCore.KgItem {
        id: table_background
        z: 0
        anchors.fill: parent
        provider: themeProvider
        spriteKey: "background"
    }

    Board {
        z: 1
        id: board
        width: Math.min(parent.width, parent.height)
        height: Math.min(parent.width, parent.height)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        onCellClicked: container.cellClicked(row, column)
    }
}
