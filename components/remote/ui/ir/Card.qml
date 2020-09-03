/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

import QtQuick 2.11
import QtQuick.Controls 2.5
import Style 1.0

import ButtonHandler 1.0

import Entity.Remote 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: card
    width: parent.width; height: parent.height
    color: Style.color.dark
    radius: Style.cornerRadius

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"

    states: [
        State {
            name: "open"
        },
        State {
            name: "closed"
            PropertyChanges {target: topItem; opacity: 0; }
            PropertyChanges {target: pagesSwipeView; opacity: 0; }
            PropertyChanges {target: tooltips; opacity: 0; }
            PropertyChanges {target: indicator; opacity: 0; }
        }
    ]

    transitions: [
        Transition {
            to: "closed"
            PropertyAnimation { target: topItem; properties: "opacity"; easing.type: Easing.OutExpo; duration: 200 }
            PropertyAnimation { target: pagesSwipeView; properties: "opacity"; easing.type: Easing.OutExpo; duration: 200 }
            PropertyAnimation { target: tooltips; properties: "opacity"; easing.type: Easing.OutExpo; duration: 200 }
            PropertyAnimation { target: indicator; properties: "opacity"; easing.type: Easing.OutExpo; duration: 200 }
        }
    ]

    Component.onCompleted: {
        card.state = "open";
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BUTTON REPEAT TIMER
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Timer {
        id: volumeUpTimer
        interval: 250
        repeat: true
        running: false
        triggeredOnStart: true
        onTriggered: obj.volumeUp()
    }

    Timer {
        id: volumeDownTimer
        interval: 250
        repeat: true
        running: false
        triggeredOnStart: true
        onTriggered: obj.volumeDown()
    }

    Timer {
        id: channelUpTimer
        interval: 500
        repeat: true
        running: false
        triggeredOnStart: true
        onTriggered: obj.channelUp()
    }

    Timer {
        id: channelDownTimer
        interval: 500
        repeat: true
        running: false
        triggeredOnStart: true
        onTriggered: obj.channelDown()
    }

    Timer {
        id: cursorUpTimer
        interval: 250
        repeat: true
        running: false
        triggeredOnStart: true
        onTriggered: obj.cursorUp()
    }

    Timer {
        id: cursorDownTimer
        interval: 250
        repeat: true
        running: false
        triggeredOnStart: true
        onTriggered: obj.cursorDown()
    }

    Timer {
        id: cursorLeftTimer
        interval: 250
        repeat: true
        running: false
        triggeredOnStart: true
        onTriggered: obj.cursorLeft()
    }

    Timer {
        id: cursorRightTimer
        interval: 250
        repeat: true
        running: false
        triggeredOnStart: true
        onTriggered: obj.cursorRight()
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: ButtonHandler

        onButtonReleased: {
            switch (button) {
            case ButtonHandler.VOLUME_UP:
                print("volume up released")
                volumeUpTimer.stop()
                break;
            case ButtonHandler.VOLUME_DOWN:
                print("volume down released")
                volumeDownTimer.stop()
                break;
            case ButtonHandler.CHANNEL_UP:
                print("channel up released")
                channelUpTimer.stop()
                break;
            case ButtonHandler.CHANNEL_DOWN:
                print("channel down released")
                channelDownTimer.stop()
                break;
            case ButtonHandler.DPAD_UP:
                print("dpad up released")
                cursorUpTimer.stop()
                break;
            case ButtonHandler.DPAD_DOWN:
                print("dpad down released")
                cursorDownTimer.stop()
                break;
            case ButtonHandler.DPAD_LEFT:
                print("dpad left released")
                cursorLeftTimer.stop()
                break;
            case ButtonHandler.DPAD_RIGHT:
                print("dpad right released")
                cursorRightTimer.stop()
                break;
            }
        }

        onButtonPressed: {
            switch (button) {
            case ButtonHandler.VOLUME_UP:
                if (obj.isSupported(Remote.F_VOLUME_UP)) {
                    volumeUpTimer.start()
                }
                break;
            case ButtonHandler.VOLUME_DOWN:
                if (obj.isSupported(Remote.F_VOLUME_DOWN)) {
                    volumeDownTimer.start()
                }
                break;
            case ButtonHandler.CHANNEL_UP:
                if (obj.isSupported(Remote.F_CHANNEL_UP)) {
                    channelUpTimer.start()
                }
                break;
            case ButtonHandler.CHANNEL_DOWN:
                if (obj.isSupported(Remote.F_CHANNEL_DOWN)) {
                    channelDownTimer.start()
                }
                break;
            case ButtonHandler.DPAD_UP:
                if (obj.isSupported(Remote.F_CURSOR_UP)) {
                   cursorUpTimer.start()
                }
                break;
            case ButtonHandler.DPAD_DOWN:
                if (obj.isSupported(Remote.F_CURSOR_DOWN)) {
                    cursorDownTimer.start()
                }
                break;
            case ButtonHandler.DPAD_LEFT:
                if (obj.isSupported(Remote.F_CURSOR_LEFT)) {
                    cursorLeftTimer.start()
                }
                break;
            case ButtonHandler.DPAD_RIGHT:
                if (obj.isSupported(Remote.F_CURSOR_RIGHT)) {
                    cursorRightTimer.start()
                }
                break;
            case ButtonHandler.DPAD_MIDDLE:
                if (obj.isSupported(Remote.F_CURSOR_OK)) {
                    obj.cursorOK();
                }
                break;
            case ButtonHandler.TOP_LEFT:
                if (obj.isSupported(Remote.F_MUTE_TOGGLE)) {
                    obj.muteToggle();
                }
                break;
            case ButtonHandler.BOTTOM_LEFT:
                if (obj.isSupported(Remote.F_BACK)) {
                    obj.back();
                }
                break;
            case ButtonHandler.BOTTOM_RIGHT:
                if (obj.isSupported(Remote.F_MENU)) {
                    obj.menu();
                }
                break;
            }

        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Item {
        id: topItem
        width: parent.width; height: 125
        anchors.top: parent.top

        Text {
            id: title
            color: Style.color.text
            text: obj.friendly_name
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            width: parent.width-232
            anchors { left: parent.left; leftMargin: 106; verticalCenter: parent.verticalCenter }
            font: Style.font.button
            lineHeight: 1
        }

        Text {
            color: Style.color.text
            text: icon.text
            width: 85; height: 85
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font { family: "icons"; pixelSize: 100 }
            anchors { left: parent.left; leftMargin: 10; verticalCenter: parent.verticalCenter }
        }
    }

    SwipeView {
        id: pagesSwipeView
        width: parent.width; height: parent.height - topItem.height - tooltips.height - 50
        anchors { top: topItem.bottom; topMargin: 0 }
        currentIndex: 0

        // buttons
        Loader {
            asynchronous: true
            sourceComponent: buttonView
        }

        // transport buttons
        Loader {
            asynchronous: true
            sourceComponent: buttonTransportView
        }

        // function buttons
        Repeater {
            model: (obj.isSupported(Remote.F_FUNCTION_RED) ||
                     obj.isSupported(Remote.F_FUNCTION_GREEN) ||
                     obj.isSupported(Remote.F_FUNCTION_BLUE) ||
                     obj.isSupported(Remote.F_FUNCTION_YELLOW) ||
                     obj.isSupported(Remote.F_FUNCTION_ORANGE)) ? 1 : 0
            Loader {
                asynchronous: true
                sourceComponent: functionButtonView
            }
        }

        // channels
        Repeater {
            model: (obj.channels.length > 0) ? 1 : 0
            Loader {
                asynchronous: true
                sourceComponent: channelView
            }
        }

        // custom buttons
        Loader {
            asynchronous: true
            sourceComponent: customButtonView
        }

    }

    Component {
        id: buttonView
        CardButtons {}
    }

    Component {
        id: buttonTransportView
        CardButtonsTransport {}
    }

    Component {
        id: channelView
        CardChannels {}
    }

    Component {
        id: customButtonView
        CustomButtons {}
    }

    Component {
        id: functionButtonView
        FunctionButtons {}
    }

    PageIndicator {
        id: indicator
        count: pagesSwipeView.count
        currentIndex: pagesSwipeView.currentIndex

        anchors { bottom: tooltips.top; horizontalCenter: parent.horizontalCenter }

        delegate: Rectangle {
            width: 8; height: 8
            radius: height/2
            color: Style.color.text
            opacity: index == pagesSwipeView.currentIndex ? 1 : 0.3
        }
    }

    Item {
        id: tooltips
        width: parent.width; height: 80
        anchors.bottom: parent.bottom

        Item {
            id: leftToolTip
            width: childrenRect.width; height: 60
            anchors { left: parent.left; leftMargin: 20; verticalCenter: parent.verticalCenter }

            Text {
                color: Style.color.text
                text: Style.icon.circle
                width: 60; height: 60
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font { family: "icons"; pixelSize: 60 }
                anchors { left: parent.left; verticalCenter: parent.verticalCenter }
            }

            Text {
                color: Style.color.text
                opacity: 0.5
                text: qsTr("Mute") + translateHandler.emptyString
                verticalAlignment: Text.AlignVCenter
                anchors { left: parent.left; leftMargin: 50; verticalCenter: parent.verticalCenter }
                font { family: "Open Sans Regular"; pixelSize: 24 }
                lineHeight: 1
            }
        }

        Item {
            id: middleToolTip
            width: childrenRect.width; height: 60
            anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }

            Text {
                color: Style.color.text
                text: Style.icon.square
                width: 60; height: 60
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font {family: "icons"; pixelSize: 60 }
                anchors { left: parent.left; verticalCenter: parent.verticalCenter }
            }

            Text {
                color: Style.color.text
                opacity: 0.5
                text: qsTr("Back") + translateHandler.emptyString
                verticalAlignment: Text.AlignVCenter
                anchors { left: parent.left; leftMargin: 50; verticalCenter: parent.verticalCenter }
                font { family: "Open Sans Regular"; pixelSize: 24 }
                lineHeight: 1
            }
        }

        Item {
            id: rightToolTip
            width: childrenRect.width; height: 60
            anchors { right: parent.right; rightMargin: 40; verticalCenter: parent.verticalCenter }

            Text {
                color: Style.color.text
                text: Style.icon.square_full
                width: 60; height: 60
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font { family: "icons"; pixelSize: 60 }
                anchors { left: parent.left; verticalCenter: parent.verticalCenter }
            }

            Text {
                color: Style.color.text
                opacity: 0.5
                text: qsTr("Menu") + translateHandler.emptyString
                verticalAlignment: Text.AlignVCenter
                anchors { left: parent.left; leftMargin: 50; verticalCenter: parent.verticalCenter }
                font { family: "Open Sans Regular"; pixelSize: 24 }
                lineHeight: 1
            }
        }
    }
}
