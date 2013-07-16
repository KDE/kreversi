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
import org.kde.games.core 0.1 as KgCore

Item {
    id: container
    property int frames_count: 12
    property int current_frame: 1
    property string image_prefix: "chip_bw_"
    property int animation_time: 300

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

    Timer {
        id: blinking_timer
        interval: 500
        running: false
        repeat: true
        onTriggered: chipImage.visible = !chipImage.visible
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

            PropertyChanges {
                target: blinking_timer
                running: false
            }
        },

        State {
            name: "Black_blinking"

            PropertyChanges {
                target: chipImage
                visible: true
            }

            PropertyChanges {
                target: container
                current_frame: 1
            }

            PropertyChanges {
                target: blinking_timer
                running: true
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
                current_frame: frames_count
            }

            PropertyChanges {
                target: blinking_timer
                running: false
            }
        },

        State {
            name: "White_blinking"

            PropertyChanges {
                target: chipImage
                visible: true
            }

            PropertyChanges {
                target: container
                current_frame: frames_count
            }

            PropertyChanges {
                target: blinking_timer
                running: true
            }
        }
    ]

    Behavior on current_frame {
        NumberAnimation {
            duration: animation_time
            easing.type: Easing.InOutQuad
        }
    }
}
