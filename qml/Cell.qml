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

Item {
    id: container
    signal clicked

    function setChipState(new_state) {
        reversi_chip.state = new_state
    }

    function getChipState() {
        return reversi_chip.state
    }

    function toggleHint() {
        cell_hint_image.visible = !cell_hint_image.visible
    }

    function toggleLegalImage() {
        cell_legal_image.visible = !cell_legal_image.visible
    }

    KgCore.KgItem {
        id: cell_hint_image
        z: 0
        anchors.fill: parent
        visible: false
        provider: themeProvider
        spriteKey: "board_numbers"
    }

    KgCore.KgItem {
        id: cell_legal_image
        z: 1
        anchors.fill: parent
        visible: false
        provider: themeProvider
        spriteKey: "move_hint"
    }

    ReversiChip {
        id: reversi_chip
        z: 2
        anchors.fill: parent
        onClicked: container.clicked()
    }
}
