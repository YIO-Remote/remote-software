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

Rectangle {
    id: element
    width: parent.width
    height: 80
    color: selected ? colorHighlight : Style.colorLine
    radius: Style.cornerRadius

    property alias ssid: textSSID.text
    property alias line: line
    property alias mouseArea: mouseArea
    property bool selected: false
    property int buttonId
    property int rssi

    signal clicked(int buttonId);

    Behavior on color {
        PropertyAnimation { duration: 400 }
    }

    Text {
        id: textSSID
        color: Style.colorBackground
        text: "Network SSID"
        width: 370
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 4
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 28
        lineHeight: 1
    }

//    Image {
//        asynchronous: true
//        source: rssi > -60 ? "qrc:/images/icon-wifi-3.png" : ( rssi < -60 && rssi > -80 ? "qrc:/images/icon-wifi-2.png" : "qrc:/images/icon-wifi-1.png")
//        anchors.right: parent.right
//        anchors.rightMargin: 10
//        anchors.verticalCenter: parent.verticalCenter
//    }

    Rectangle {
        id: line
        width: parent.width
        height: 1
        visible: !selected
        color: Style.colorBackground
//        opacity: 0.15
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            parent.clicked(parent.buttonId)
        }
    }
}
