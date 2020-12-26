/*
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.3
import "globals.js" as Globals

/**
  * Chip
  *
  * Represents chip on cell.
  * Set state to:
  *     "Black" to show black chip
  *     "White" to show white chip
  *     "" (empty string) to hide chip
  */

Item {
    id: chipContainer

    /**
     * Total frames in animation
     */
    property int framesCount: 12
    /**
      * Current chip's frame
      */
    property int currentFrame: 1
    /**
      * Chips image's ID prefix at SVG theme file to use.
      * Should already contain '_' at end of it.
      */
    property string imagePrefix: "chip_bw_"
    /**
      * Duration of chip's turning animation
      */
    property int animationTime: 25 * 12
    /**
     * How long to wait before start of animation
     */
    property int preAnimationTime: 0

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
                duration: preAnimationTime
            }
            
            NumberAnimation {
                duration: animationTime
                easing.type: Easing.InOutQuad
            }

        }
    }
    
    transitions: [
    Transition {
        from: ""
        to: "*"
        reversible: false
        
        NumberAnimation {
            property: "currentFrame"
            duration: 0
        }
    }
    ]
}
