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
    property int frames_count: 12
    property int current_frame: 1
    property string image_prefix: "chip_bw_"

    signal clicked

    KgCore.KgItem {
        id: chipImage
        z: 0
        anchors.fill: parent
        visible: false
        provider: themeProvider
        spriteKey: image_prefix + current_frame
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: container.clicked()
    }

    states: [
        State {
            name: "Black"

            PropertyChanges {
                target: chipImage
                visible: true
            }

            PropertyChanges {
                target: container
                current_frame: 1
            }
        },

        State {
            name: "White"

            PropertyChanges {
                target: chipImage
                visible: true
            }

            PropertyChanges {
                target: container
                current_frame: container.frames_count
            }
        }
    ]

    Behavior on current_frame {
        NumberAnimation {
            duration: 300
            easing.type: Easing.InOutQuad
        }
    }
}
