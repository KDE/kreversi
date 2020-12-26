/*
    SPDX-FileCopyrightText: 2007 Dmitry Suzdalev <dimsuz@gmail.com>
    SPDX-FileCopyrightText: 2012 Viranch Mehta <viranch.mehta@gmail.com
    SPDX-FileCopyrightText: 2013 Denis Kuplyakov <dener.kup@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick 2.3
import ColorScheme 1.0

/**
  * Popup
  *
  * Represents popup. It's analog of KGamePopupItem.
  *
  * TODO: move it to libkdegames
  */

Rectangle {
    id: popup

    // constants

    /**
      * Margin on the sides of message box
      */
    property int marginOnSides: 15
    /**
      * Space between pixmap and text
      */
    property int marginIconText: 10

    // API properties

    // TODO: If item is hovered with mouse it will hide only after user moves the mouse away
    /**
      * The amount of time the item will stay visible on screen before it goes away.
      * By default item is shown for 2000 msec.
      */
    property int timeout: 2000
    /**
      * The popup angles sharpness
      */
    property alias sharpness: popup.radius
    /**
      * Name of the icon to show at popup
      */
    property string iconName: "dialog-information"
    /**
      * Sets whether to hide this popup item on mouse click.
      *
      * By default a mouse click will cause an item to hide
      */
    property alias hideOnMouseClick: mouseArea.enabled
    /**
      * Sets default color for unformatted text.
      * By default system-default color is used.
      */
    property alias textColor: text.color
    /**
      * Sets default color for background.
      * By default system-default color is used.
      */
    property alias backgroundColor: popup.color
    /**
      * Sets whether to use custom border color or system-default one.
      * By default system-default color is used.
      */
    property bool useCustomBorderColor: false
    /**
      * Border's color.
      * By default system-default color is used.
      */
    property color borderColor: popup.border.color
    /**
      * Border's width in pixels.
      * By default it's equals 1
      */
    property int borderWidth: 1
    /**
      * Sets whether new message is replacing already showing one or ignored
      */
    property bool isReplacing: false

    // API signals
    /**
      * Emitted when user clicks on a link in item.
      */
    signal linkActivated(string link)
    /**
      * Emitted when popup hides.
      */
    signal hidden()

    // private properties
    height: row.height + 2 * marginOnSides
    width: row.width + 2 * marginOnSides

    color: colorScheme.background
    border.color: useCustomBorderColor ? black : colorScheme.border
    border.width: borderWidth

    function show(message, showing_state) {
        if (!timer.running || isReplacing) {
            text.text = message
            timer.restart()
            state = showing_state
        }
    }
    
    ColorScheme {
        id: colorScheme
    }

    Timer {
        id: timer
        repeat: false
        interval: timeout
        onTriggered: {
            state = ""
            hidden()
        }
    }

    Row {
        id: row
        anchors.centerIn: parent
        spacing: marginIconText

        Image {
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            width: visible ? 32 : 0
            height: width

            visible: iconName != ""

            source: "image://icon/" + iconName
        }


        Text {
            id: text
            anchors.verticalCenter: parent.verticalCenter
            color: colorScheme.foreground
            wrapMode: Text.WordWrap

            onLinkActivated: popup.linkActivated(link);
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            popup.state = ""
            timer.stop()
            hidden()
        }
    }
}
