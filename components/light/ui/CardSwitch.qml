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
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: cardDimmable
    width: parent.width
    height: parent.height
    color: Style.colorDark

    MouseArea {
        id: dragger
        anchors.fill: parent

        onClicked: {
            haptic.playEffect("click");
            obj.toggle();
        }
    }

    Rectangle {
        id: percentageBG
        color: obj.state ? Style.colorHighlight1 : Style.colorBackgroundTransparent
        width: parent.width
        height: parent.height
        radius: Style.cornerRadius
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }

        Behavior on color {
            PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
        }
    }

    Text {
        id: icon
        color: Style.colorText
        text: "\uE901"
        renderType: Text.NativeRendering
        width: 85
        height: 85
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font {family: "icons"; pixelSize: 100 }
        anchors {top: parent.top; topMargin: 20; left: parent.left; leftMargin: 20}
    }

    Text {
        id: percentage
        color: Style.colorText
        text: obj.state ? "On" : "Off"
        horizontalAlignment: Text.AlignLeft
        anchors { top: icon.bottom; topMargin: -20; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Light"; pixelSize: 180 }
    }

    Text {
        id: title
        color: Style.colorText
        text: obj.friendly_name
        wrapMode: Text.WordWrap
        width: parent.width-60
        anchors { top: percentage.bottom; topMargin: -40; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Regular"; pixelSize: 60 }
        lineHeight: 0.9
    }

    Text {
        id: areaText
        color: Style.colorText
        opacity: 0.5
        text: obj.area
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
        width: parent.width-60
        anchors { top: title.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Regular"; pixelSize: 24 }
    }

    BasicUI.CustomButton {
        anchors { left:parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
        color: Style.colorText
        buttonTextColor: Style.colorBackground
        buttonText: obj.state ? qsTr("Turn off") + translateHandler.emptyString : qsTr("Turn on") + translateHandler.emptyString

        mouseArea.onClicked: {
            haptic.playEffect("click");
            obj.toggle();
        }
    }


    Text {
        id: closeButton
        color: Style.colorText
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
            width: parent.width + 20
            height: parent.height + 20
            anchors.centerIn: parent

            onClicked: {
                haptic.playEffect("click");
                lightButton.state = "closed"
            }
        }
    }
}
