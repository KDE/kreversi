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
    id: tableContainer
    anchors.fill: parent

    property bool isBoardShowingLabels: false
    property string chipsImagePrefix: "chip_bw"
    property int chipsAnimationTime: 25 * 12

    signal cellClicked(int row, int column)

    function setLegal(row, column, value) {
        board.setLegal(row, column, value);
    }

    function setChipState(row, column, value) {
        board.setChipState(row, column, value);
    }

    function setHint(row, col, show) {
        board.setHint(row, col, show);
    }

    function setLastMove(row, column, value) {
        board.setLastMove(row, column, value)
    }

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
