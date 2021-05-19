/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 * Copyright (C) 2021 Michael Loercher <michaelloercher@web.de>
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
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0

import Style 1.0

import Haptic 1.0
import StandbyControl 1.0
import ButtonHandler 1.0
import IntegrationConnectionStates 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: buttonContainer

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO INTEGRATION
    // Disable button if the integration is disconnected
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: obj

        onConnectedChanged: {
            if (obj.connected) {
                buttonContainer.opacity = 1
                buttonContainer.enabled = true
            } else if (obj.state === IntegrationConnectionStates.DISCONNECTED) {
                buttonContainer.opacity = 0.3
                buttonContainer.enabled = false
                buttonContainer.close()
            }
            else {
                buttonContainer.opacity = 0.3
                buttonContainer.enabled = false
            }
        }
    }

    Behavior on opacity {
        PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BASIC SETTINGS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    width: 460
    height: 125
    anchors.horizontalCenter: parent.horizontalCenter
    color: Style.color.dark
    radius: Style.cornerRadius


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property var obj
    property var originParent: buttonContainer.parent
    property bool _isCurrentItem: parent.__isCurrentItem ? parent.__isCurrentItem : false

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function open() {
        buttonContainer.originParent = buttonContainer.parent
        cardLoader.active = true;
        buttonContainer.state = "open";
    }

    function close() {
        buttonContainer.state = "closed"
        if (cardLoader.item) {
            cardLoader.item.state = "closed"
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    state: "closed"

    states: [
        State {
            name: "closed"
            PropertyChanges {target: buttonContainer; width: 460; height: 125; scale: 1 }
            ParentChange { target: buttonContainer; parent: originParent }
            PropertyChanges {target: loader_main; state: "visible" }
            PropertyChanges {target: cardLoader; opacity: 0}
            PropertyChanges {target: titleElement; opacity: 1}
        },
        State {
            name: "open"
            PropertyChanges {target: buttonContainer; width: 480; height: 720; scale: 1 }
            ParentChange { target: buttonContainer; parent: contentWrapper; x: 20; y: 80 }
            PropertyChanges {target: loader_main; state: "hidden" }
            PropertyChanges {target: cardLoader; opacity: 1 }
            PropertyChanges {target: titleElement; opacity: 0}
        }
    ]

    transitions: [
        Transition {
            to: "closed"
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: cardLoader; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                    ParallelAnimation {
                        PropertyAnimation { target: buttonContainer; properties: "width, height, scale"; easing.type: Easing.OutExpo; duration: 300 }
                        ParentAnimation {
                            NumberAnimation { properties: "x,y"; easing.type: Easing.OutExpo; duration: 300 }
                        }
                        SequentialAnimation {
                            PauseAnimation { duration: 100 }
                            PropertyAnimation { target: titleElement; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        }
                    }
                }
                PropertyAction { target: cardLoader; property: "active"; value: false }
            }
        },
        Transition {
            to: "open"
            ParallelAnimation {
                PropertyAnimation { target: buttonContainer; properties: "width, height, scale"; easing.type: Easing.OutBack; easing.overshoot: 1.5; duration: 400 }
                PropertyAnimation { target: cardLoader; properties: "opacity"; duration: 1 }
                PropertyAnimation { target: titleElement; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                ParentAnimation {
                    NumberAnimation { properties: "x,y"; easing.type: Easing.OutBack; easing.overshoot: 0.8; duration: 300 }
                }
            }
        }
    ]


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MOUSEAREAS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MouseArea {
        anchors.fill: parent
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: buttonContainer.state == "open" ? false : true

        onPressAndHold: {
            Haptic.playEffect(Haptic.Press);
            addToFavButton.state = "open"
        }

        onClicked: {
            Haptic.playEffect(Haptic.Click);
            buttonContainer.open();
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CLOSED STATE ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias title: title
    property alias icon: icon.text
    property alias button: button

    Item {
        id: titleElement
        width: parent.width
        height: 125
        anchors.top: parent.top

        Text {
            id: title
            color: Style.color.text
            width: parent.width-232
            text: obj ? obj.friendly_name : ""
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            anchors { left: parent.left; leftMargin: 126; verticalCenter: parent.verticalCenter}
            font: Style.font.button
        }

        Text {
            id: icon
            color: Style.color.text
            text: ""
            width: 85; height: 85
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 100 }
            anchors { left: parent.left; leftMargin: 20; verticalCenter: parent.verticalCenter }
        }
    }

    BasicUI.CustomSwitch {
        id: button
        anchors { right: parent.right; rightMargin: 20; verticalCenter: parent.verticalCenter }
        enabled: obj ? obj.supportsOn : false
        visible: enabled

        checked: obj ? obj.isOn : false
        mouseArea.enabled: buttonContainer.state == "open" ? false: true
        mouseArea.onClicked: {
            if (obj.isOn) {
                obj.turnOff();
            } else {
                obj.turnOn();
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ADD TO FAVORITE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // LAYER MASK FOR ROUNDED CORNERS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    layer.enabled: false
    layer.effect: OpacityMask {
        maskSource:
            Rectangle {
            id: opacityMask
            width: buttonContainer.width; height: buttonContainer.height
            radius: Style.cornerRadius

            Behavior on radius {
                NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
            }
        }
    }

    property alias addToFavButton: addToFavButton

    Rectangle {
        id: addToFavButton
        width: 0; height: 0
        z: 1000
        radius: 200
        anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter}
        color: Style.color.highlight1

        state: "closed"

        states: [
            State {
                name: "closed"
                PropertyChanges {target: addToFavButton; width: 0; height: 0; radius: 200; color: Style.color.highlight1}
                PropertyChanges {target: addToFavButtonCircle; opacity: 0}
                PropertyChanges {target: addToFavButtonText; opacity: 0}
            },
            State {
                name: "open"
                PropertyChanges {target: addToFavButton; width:500; height: 500; color: Style.color.highlight1}
                PropertyChanges {target: addToFavButtonCircle; opacity: 1}
                PropertyChanges {target: addToFavButtonText; opacity: 1}
            }
        ]

        transitions: [
            Transition {
                to: "closed"
                SequentialAnimation {
                    ParallelAnimation {
                        PropertyAnimation { target: addToFavButtonCircle; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                        PropertyAnimation { target: addToFavButtonText; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                    }
                    PropertyAnimation { target: addToFavButton; properties: "width, height, radius"; easing.type: Easing.OutExpo; duration: 400 }
                    PropertyAnimation { target: addToFavButton; properties: "color"; duration: 1 }
                    PropertyAction { target: buttonContainer; property: "layer.enabled"; value: false }
                }
            },
            Transition {
                to: "open"
                SequentialAnimation {
                    PropertyAction { target: buttonContainer; property: "layer.enabled"; value: true }
                    PropertyAnimation { target: addToFavButton; properties: "width, height, radius"; easing.type: Easing.InExpo; duration: 400 }
                    ParallelAnimation {
                        PropertyAnimation { target: addToFavButtonCircle; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                        PropertyAnimation { target: addToFavButtonText; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                    }
                    PropertyAnimation { target: addToFavButton; properties: "color"; easing.type: Easing.InExpo; duration: 400 }
                }
            }
        ]

        MouseArea {
            anchors.fill: parent

            onClicked: {
                Haptic.playEffect(Haptic.Click);
                addToFavButton.state = "closed"
            }
        }

        Rectangle {
            id: addToFavButtonCircle
            width: 80; height: width
            radius: width/2
            color: Style.color.dark
            opacity: 0
            anchors { left: parent.left; leftMargin: 40; verticalCenter: parent.verticalCenter }

            states: State {
                name: "pressed"
                when: mouseAreaFav.pressed === true
                PropertyChanges {
                    target: addToFavButtonCircle
                    color: Style.color.highlight1
                }
            }

            transitions: [
                Transition {
                    from: ""; to: "pressed"; reversible: true
                    PropertyAnimation { target: addToFavButtonCircle
                        properties: "color"; duration: 300 }
                }]

            Text {
                color: Style.color.text
                text: obj && obj.favorite ? Style.icon.fav_remove : Style.icon.fav_add
                renderType: Text.NativeRendering
                width: 80; height: 80
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font {family: "icons"; pixelSize: 80 }
                anchors.verticalCenter: parent.verticalCenter; anchors.horizontalCenter: parent.horizontalCenter
            }

            MouseArea {
                id: mouseAreaFav
                anchors.fill: parent
                enabled: addToFavButtonCircle.opacity == 1 ? true : false

                onClicked: {
                    obj.favorite = !obj.favorite;
                    addToFavButton.state = "closed";
                }
            }
        }

        Text {
            id: addToFavButtonText
            color: Style.color.text
            opacity: 0
            text: obj && obj.favorite ? qsTr("Remove from favorites") + translateHandler.emptyString : qsTr("Add to favorites") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            anchors { left: addToFavButtonCircle.right; leftMargin: 26; verticalCenter: addToFavButtonCircle.verticalCenter }
            font: Style.font.button
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // OPEN STATE ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias cardLoader: cardLoader

    Loader {
        id: cardLoader
        width: buttonContainer.width; height: buttonContainer.height
        asynchronous: true
        active: false
        source: "qrc:/components/remote/ui/Card.qml"
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CLOSE BUTTON
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Connections {
        target: ButtonHandler
        enabled: state == "open" ? true : false

        onButtonPressed: {
            if (StandbyControl.mode === StandbyControl.ON || StandbyControl.mode === StandbyControl.DIM) {
                if (button === ButtonHandler.TOP_RIGHT) {
                    buttonContainer.close();
                }
            }
        }
    }

    property alias closeButtonMouseArea: closeButtonMouseArea

    Text {
        id: closeButton
        color: Style.color.text
        visible: buttonContainer.state == "open"
        text: Style.icon.close
        renderType: Text.NativeRendering
        width: 70; height: 70
        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
        font {family: "icons"; pixelSize: 80 }
        anchors { top: parent.top; topMargin: 20; right: parent.right; rightMargin: 10 }

        MouseArea {
            id: closeButtonMouseArea
            width: parent.width + 20; height: parent.height + 20
            anchors.centerIn: parent

            onClicked: {
                Haptic.playEffect(Haptic.Click);
                buttonContainer.close();
            }
        }
    }
}
