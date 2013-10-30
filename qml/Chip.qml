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

Item {
    id: chipContainer
    property int framesCount: 12
    property int currentFrame: 1
    property string imagePrefix: "chip_bw_"
    property int animationTime: 25 * 12
    property int preAnimationTicks: 0

    signal clicked

    CanvasItem {
        id: chipImage
        anchors.fill: parent
        visible: false
        spriteKey: imagePrefix + currentFrame
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: chipContainer.clicked()
    }

    Timer {
        id: blinkingTimer
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
                target: chipContainer
                currentFrame: 1
            }

            PropertyChanges {
                target: blinkingTimer
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
                target: chipContainer
                currentFrame: 1
            }

            PropertyChanges {
                target: blinkingTimer
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
                target: chipContainer
                currentFrame: framesCount
            }

            PropertyChanges {
                target: blinkingTimer
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
                target: chipContainer
                currentFrame: framesCount
            }

            PropertyChanges {
                target: blinkingTimer
                running: true
            }
        }
    ]

    Behavior on currentFrame {
        SequentialAnimation {
            PauseAnimation {
                duration: preAnimationTicks * animationTime
            }
            
            NumberAnimation {
                duration: animationTime
                easing.type: Easing.InOutQuad
            }
        }
    }
}
