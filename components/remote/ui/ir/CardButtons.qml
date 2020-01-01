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
import Style 1.0

import Entity.Remote 1.0

Item {
    width: parent.width - 60
    anchors.horizontalCenter: parent.horizontalCenter

    Rectangle {
        id: bg
        visible: obj.isSupported(Remote.F_POWER_TOGGLE) || (obj.isSupported(Remote.F_POWER_ON) && obj.isSupported(Remote.F_POWER_OFF))
        width: 110
        height: 60
        radius: height/2
        color: Style.colorRed
        anchors.top: parent.top
        anchors.left: parent.left

        Text {
            color: Style.colorLine
            text: "\uE908"
            renderType: Text.NativeRendering
            width: 70
            height: 70
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 60 }
            anchors.centerIn: parent
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                haptic.playEffect("click");
                if (obj.isSupported(Remote.F_POWER_TOGGLE)) {
                    obj.powerToggle();
                } else if (obj.isSupported(Remote.F_POWER_ON) && obj.isSupported(Remote.F_POWER_OFF)) {
                    if (obj.isOn)
                        obj.powerOff();
                    else
                        obj.powerOn();
                }
            }
        }
    } // Rectangle end

    Button {
        visible: obj.isSupported(Remote.F_SOURCE)
        anchors.top: parent.top
        anchors.right: parent.right
        title: qsTr("Source") + translateHandler.emptyString
        mouseArea.onClicked: {
            obj.source();
        }
    }

    Button {
        visible: obj.isSupported(Remote.F_INFO)
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        title: qsTr("Info") + translateHandler.emptyString
        mouseArea.onClicked: {
            obj.info();
        }
    }

    Button {
        visible: obj.isSupported(Remote.F_MENU)
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        title: qsTr("Menu") + translateHandler.emptyString
        mouseArea.onClicked: {
            obj.menu();
        }
    }

    Button {
        visible: obj.isSupported(Remote.F_GUIDE)
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        title: qsTr("Guide") + translateHandler.emptyString
        mouseArea.onClicked: {
            obj.guide();
        }
    }

}



