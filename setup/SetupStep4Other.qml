/******************************************************************************
 *
 * Copyright (C) 2018-2020 Marton Borzak <hello@martonborzak.com>
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
import QtQuick.Controls 2.12
import Style 1.0
import Haptic 1.0
import WifiControl 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: container
    width: parent.width; height: 400 //childrenRect.height + 40
    color: Style.color.background

    MouseArea {
        anchors.fill: parent
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property var swipeViewObj

    function joinNetwork(name, security) {
        swipeViewObj.parent.parent.wifiPassword = passwordField.text;
        swipeViewObj.parent.parent.wifiSsid = name;

        wifi.join(name, passwordField.text, security);
        inputPanel.active = false;
        container.state = "closed";
        swipeViewObj.incrementCurrentIndex();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"


    states: [
        State { name: "closed";
            PropertyChanges {target: container; opacity: 0; y: 800 }
        },
        State { name: "open";
            PropertyChanges {target: container; opacity: 1; y: 0 }
        }
    ]
    transitions: [
        Transition {to: "closed";
            SequentialAnimation {
                PropertyAnimation { target: container; properties: "opacity, y"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAction { target: loader_second; property: "source"; value: "" }
                PropertyAction { target: loader_second; property: "active"; value: false }
            }
        },
        Transition {to: "open";
            PropertyAnimation { target: container; properties: "opacity, y"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAction { target: inputPanel; property: "active"; value: true }
        }
    ]

    Component.onCompleted: {
        container.state = "open";
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Text {
        id: titleText
        color: Style.color.text
        width: parent.width - 100
        wrapMode: Text.WordWrap
        verticalAlignment: Text.AlignVCenter
        text: qsTr("Enter your WiFi network credentials") + translateHandler.emptyString
        anchors { left: parent.left; leftMargin: 20; verticalCenter: closeButton.verticalCenter }
        font: Style.font.button
        lineHeight: 1.2
    }

    Rectangle {
        id: ssidFieldContainer
        width: parent.width - 40; height: 80
        anchors { top: closeButton.bottom; topMargin: 20; horizontalCenter: parent.horizontalCenter }
        color: Style.color.text
        radius: Style.cornerRadius

        TextField {
            id: ssidField
            cursorVisible: false
            width: parent.width-80; height: parent.height
            anchors { left: parent.left; leftMargin: 10; verticalCenter: parent.verticalCenter }
            placeholderText: qsTr("SSID") + translateHandler.emptyString
            echoMode: TextInput.Normal
            color: Style.color.background
            font { family: "Open Sans Regular"; pixelSize: 27 }
            focus: true

            background: Rectangle {
                color: Style.color.backgroundTransparent
                border.width: 0
            }

            onFocusChanged: {
                if (focus) {
                    inputPanel.active = true;
                }
            }

            Component.onCompleted: {
                ssidField.forceActiveFocus();
            }
        }
    }

    Rectangle {
        id: passwordFieldContainer
        width: parent.width - 40; height: 80
        anchors { top: ssidFieldContainer.bottom; topMargin: 20; horizontalCenter: parent.horizontalCenter }
        color: Style.color.text
        radius: Style.cornerRadius

        TextField {
            id: passwordField
            cursorVisible: false
            width: parent.width-80; height: parent.height
            anchors { left: parent.left; leftMargin: 10; verticalCenter: parent.verticalCenter }
            placeholderText: qsTr("Password") + translateHandler.emptyString
            echoMode: TextInput.Password
            passwordCharacter: "*"
            color: Style.color.background
            font { family: "Open Sans Regular"; pixelSize: 27 }
            focus: true

            background: Rectangle {
                color: Style.color.backgroundTransparent
                border.width: 0
            }

            Binding {
                target: passwordField
                property: "passwordMaskDelay"
                value: 1000
                when: passwordField.hasOwnProperty("passwordMaskDelay")

            }

            onAccepted: {
                joinNetwork(ssidField.text, WifiSecurityEnum.WPA2_PSK);
            }

            Text {
                color: Style.color.background
                text: Style.icon.eye
                renderType: Text.NativeRendering
                width: 70; height: 70
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font {family: "icons"; pixelSize: 80 }
                anchors { right: parent.right; rightMargin: -60; verticalCenter: parent.verticalCenter }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (passwordField.echoMode == TextInput.Password) {
                            passwordField.echoMode = TextInput.Normal
                        } else {
                            passwordField.echoMode = TextInput.Password
                        }
                    }
                }
            }
        }
    }

    BasicUI.CustomButton {
        anchors { top: passwordFieldContainer.bottom; topMargin: 40; left: passwordFieldContainer.left }
        buttonText: qsTr("Join") + translateHandler.emptyString
        mouseArea.onClicked: {
            Haptic.playEffect(Haptic.Click);
            joinNetwork(ssidField.text, WifiSecurityEnum.WPA2_PSK);
        }
    }

    Text {
        id: closeButton
        color: Style.color.text
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
                inputPanel.active = false;
                container.state = "closed";
            }
        }
    }
}
