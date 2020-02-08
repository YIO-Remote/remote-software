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
import QtQuick.Controls 2.12
import Style 1.0
import Haptic 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: container
    width: parent.width; height: 400 //childrenRect.height + 40
    color: Style.colorBackground

    MouseArea {
        anchors.fill: parent
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property var obj

    function joinNetwork() {
        wifi.join(obj.name, passwordField.text, obj.security);
        inputPanel.active = false;
        container.state = "closed";
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
            PropertyChanges {target: container; opacity: 1; y: 40 }
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
        color: Style.colorText
        width: parent.width - 100
        wrapMode: Text.WordWrap
        verticalAlignment: Text.AlignVCenter
        text: qsTr("Enter password for\n\"%1\"").arg(obj.name) + translateHandler.emptyString
        anchors { left: parent.left; leftMargin: 20; verticalCenter: closeButton.verticalCenter }
        font: Style.buttonFont
        lineHeight: 1.2
    }

    Rectangle {
        id: searchContainer
        width: parent.width - 40; height: 80
        anchors { top: closeButton.bottom; topMargin: 20; horizontalCenter: parent.horizontalCenter }
        color: Style.colorText
        radius: Style.cornerRadius

        TextField {
            id: passwordField
            cursorVisible: false
            width: parent.width; height: parent.height
            anchors.centerIn: parent
            placeholderText: qsTr("Password") + translateHandler.emptyString
            echoMode: TextInput.Password
            passwordCharacter: "*"
            color: Style.colorBackground
            font { family: "Open Sans Regular"; pixelSize: 27 }
            focus: true

            background: Rectangle {
                color: Style.colorBackgroundTransparent
                border.width: 0
            }

            onAccepted: {
                joinNetwork();
            }

            onFocusChanged: {
                if (focus) {
                    inputPanel.active = true;
                } else
                    inputPanel.active = false;
            }
        }

        BasicUI.CustomButton {
            anchors { top: passwordField.bottom; topMargin: 40; left: passwordField.left }
            buttonText: qsTr("Join") + translateHandler.emptyString
            mouseArea.onClicked: {
                Haptic.playEffect(Haptic.Click);
                joinNetwork();
            }
        }
    }

    Text {
        id: closeButton
        color: Style.colorText
        text: Style.icons.close
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
