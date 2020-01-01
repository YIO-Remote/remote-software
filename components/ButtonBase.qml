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
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0

import "qrc:/scripts/helper.js" as JSHelper
import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: buttonContainer

    property var  obj

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO INTEGRATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: obj

        onConnectedChanged: {
            if (obj.connected) {
                buttonContainer.opacity = 1
                buttonContainer.enabled = true
            }
            else {
                buttonContainer.opacity = 0.3
                buttonContainer.enabled = false
                buttonContainer.state = "closed"
                cardLoader.active = false;
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
    color: colorDark
    radius: cornerRadius

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property var originParent: buttonContainer.parent

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // LAYER MASK TO MASK EVERYTHING THAT IS INSIDE THE BUTTON
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    layer.enabled: true
    layer.effect: OpacityMask {
        maskSource: Item {
            width: buttonContainer.width
            height: buttonContainer.height
            Rectangle {
                anchors.fill: parent
                radius: cornerRadius
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    state: "closed"

    states: [
        State {
            name: "closed"
            PropertyChanges {target: buttonContainer; width: 460; height: 125; scale: 1}
            ParentChange { target: buttonContainer; parent: originParent }
            PropertyChanges {target: loader_main; state: "visible" }
        },
        State {
            name: "open"
            PropertyChanges {target: buttonContainer; width: 480; height: 720}
            ParentChange { target: buttonContainer; parent: contentWrapper; x: 20; y: 80 }
            PropertyChanges {target: loader_main; state: "hidden" }
        }
    ]

    transitions: [
        Transition {
            to: "closed"
            ParallelAnimation {
                PropertyAnimation { target: buttonContainer; properties: "width, height"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: buttonContainer; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                ParentAnimation {
                    NumberAnimation { properties: "x,y"; easing.type: Easing.OutExpo; duration: 300 }
                }
            }
        },
        Transition {
            to: "open"
            ParallelAnimation {
                PropertyAnimation { target: buttonContainer; properties: "width, height"; easing.type: Easing.OutBack; easing.overshoot: 1.2; duration: 300 }
                PropertyAnimation { target: buttonContainer; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
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
            haptic.playEffect("press");

            addToFavButton.state = "open"
        }

        onClicked: {
            haptic.playEffect("click");
            originParent = buttonContainer.parent
//            buttonContainer.state = "open"
            cardLoader.active = true;
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CLOSED STATE ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias title: title
    property alias icon: icon.text
    property alias button: button


    Item {
        width: parent.width
        height: 125
        anchors.top: parent.top

        Text {
            id: title
            color: colorText
            text: obj.friendly_name
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            width: parent.width-232
            anchors.left: parent.left
            anchors.leftMargin: 126
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: icon
            color: colorText
            text: ""
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 100 }
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
        }

    }

    BasicUI.CustomSwitch {
        id: button

        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 20
        enabled: false
        visible: enabled

        checked: obj.state
        mouseArea.enabled: buttonContainer.state == "open" ? false: true
        mouseArea.onClicked: {
            if (obj.state) {
                obj.turnOff();
            } else {
                obj.turnOn();
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ADD TO FAVORITE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias addToFavButton: addToFavButton

    Rectangle {
        id: addToFavButton
        width: 0
        height: 0
        z: 1000
        radius: 200
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        color: colorHighlight1

        state: "closed"

        states: [
            State {
                name: "closed"
                PropertyChanges {target: addToFavButton; width: 0; height: 0; radius: 200; color: colorHighlight1}
                PropertyChanges {target: addToFavButtonCircle; opacity: 0}
                PropertyChanges {target: addToFavButtonText; opacity: 0}
            },
            State {
                name: "open"
                PropertyChanges {target: addToFavButton; width:500; height: 500; color: colorHighlight1}
                PropertyChanges {target: addToFavButtonCircle; opacity: 1}
                PropertyChanges {target: addToFavButtonText; opacity: 1}
            }
        ]

        transitions: [
            Transition {
                to: "closed"
                SequentialAnimation {
//                    PauseAnimation { duration: 300 }
                    ParallelAnimation {
                        PropertyAnimation { target: addToFavButtonCircle; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                        PropertyAnimation { target: addToFavButtonText; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                    }
                    PropertyAnimation { target: addToFavButton; properties: "width, height, radius"; easing.type: Easing.OutExpo; duration: 400 }
                    PropertyAnimation { target: addToFavButton; properties: "color"; duration: 1 }
                }
            },
            Transition {
                to: "open"
                SequentialAnimation {
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
                haptic.playEffect("click");
                addToFavButton.state = "closed"
            }
        }

        Rectangle {
            id: addToFavButtonCircle
            width: 80
            height: width
            radius: width/2
            color: colorDark
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 40
            opacity: 0

            states: State {
                name: "pressed"
                when: mouseAreaFav.pressed === true
                PropertyChanges {
                    target: addToFavButtonCircle
                    color: colorHighlight1
                }
            }

            transitions: [
                Transition {
                    from: ""; to: "pressed"; reversible: true
                    PropertyAnimation { target: addToFavButtonCircle
                        properties: "color"; duration: 300 }
                }]

            Image {
                asynchronous: true
                width: 80
                height: 80
                fillMode: Image.PreserveAspectFit
                source: obj.favorite ? "qrc:/images/components/fav-minus.png" : "qrc:/images/components/fav-plus.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                ColorOverlay {
                    visible: !darkMode
                    anchors.fill: parent
                    source: parent
                    color: colorText
                }
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
            color: colorText
            text: obj.favorite ? qsTr("Remove from favorites") + translateHandler.emptyString : qsTr("Add to favorites") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            anchors.verticalCenter: addToFavButtonCircle.verticalCenter
            anchors.left: addToFavButtonCircle.right
            anchors.leftMargin: 26
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
            opacity: 0
        }
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // OPEN STATE ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias cardLoader: cardLoader

    Loader {
        id: cardLoader
        width: buttonContainer.width
        height: buttonContainer.height
        asynchronous: true
        active: false //buttonContainer.state == "open"
        source: "qrc:/components/remote/ui/Card.qml"
        opacity: cardLoader.status == Loader.Ready ? 1 : 0

        onStatusChanged: {
            if (cardLoader.status == Loader.Ready) {
                buttonContainer.state = "open";
            }
        }

        Behavior on opacity {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutExpo
            }
        }

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CLOSE BUTTON
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Connections {
        target: buttonHandler
        enabled: state == "open" ? true : false

        onButtonPress: {
            if (standbyControl.mode == "on" || standbyControl.mode == "dim") {
                switch (button) {
                case "top right":
                    buttonContainer.state = "closed"
                    cardLoader.active = false;
                    break;
                }
            }
        }
    }

    property alias closeButtonMouseArea: closeButtonMouseArea

    Text {
        id: closeButton
        color: colorText
        visible: buttonContainer.state == "open"
        text: "\uE915"
        renderType: Text.NativeRendering
        width: 70
        height: 70
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font {family: "icons"; pixelSize: 80 }
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 20

        MouseArea {
            id: closeButtonMouseArea
            width: parent.width + 20
            height: parent.height + 20
            anchors.centerIn: parent

            onClicked: {
                haptic.playEffect("click");
                buttonContainer.state = "closed"
                cardLoader.active = false;
            }
        }
    }

}
