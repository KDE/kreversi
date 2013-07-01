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

    function setHint(new_state) {
        if (new_state)
            reversi_chip.state = "Black_blinking"
        else {
            if (reversi_chip.state == "Black_blinking")
                reversi_chip.state = "Black";
            else if (reversi_chip.state == "White_blinking")
                reversi_chip.state = "White";
        }
    }

    function setLegal(new_state) {
        cell_legal_image.visible = new_state
    }

    function setChipPrefix(prefix) {
        reversi_chip.image_prefix = prefix + "_";
    }

    function setLastMove(value) {
        cell_last_move_marker.visible = value;
    }

    KgCore.KgItem {
        id: cell_legal_image
        z: 1
        anchors.fill: parent
        visible: false
        provider: themeProvider
        spriteKey: "move_hint"
    }

    Rectangle {
        id: cell_last_move_marker;
        z: 1
        visible: true
        anchors.fill: parent
        color: "#AAAAAA"
    }

    ReversiChip {
        id: reversi_chip
        z: 2
        anchors.fill: parent
        onClicked: container.clicked()
    }
}
