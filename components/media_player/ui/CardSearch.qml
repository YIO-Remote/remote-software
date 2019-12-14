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

import QtQuick 2.0
import QtQuick.Controls 2.5

import "qrc:/basic_ui" as BasicUI

Item {
    width: parent.width
    height: parent.height

    Connections {
        target: card

        onLooseFocus: {
            searchTextField.focus = false;
        }
    }

    Rectangle {
        width: parent.width - 60
        height: 80
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.horizontalCenter: parent.horizontalCenter
        color: colorText
        radius: cornerRadius

        Text {
            id: searchIcon
            color: colorBackground
            text: "\uE90C"
            renderType: Text.NativeRendering
            width: 70
            height: 70
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    haptic.playEffect("click");
                    obj.search(searchTextField.text);
                    searchTextField.focus = false;
                }
            }
        }

        TextField {
            id: searchTextField
            cursorVisible: false
            width: parent.width-90
            height: parent.height
            anchors.left: searchIcon.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            placeholderText: qsTr("Search") + translateHandler.emptyString
            color: colorBackground
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27

            background: Rectangle {
                color: colorBackgroundTransparent
                border.width: 0
            }

            onAccepted: {
                obj.search(searchTextField.text)
            }

            onFocusChanged: {
                if (focus)
                    inputPanel.active = true
                else
                    inputPanel.active = false
            }
        }
    }
}
