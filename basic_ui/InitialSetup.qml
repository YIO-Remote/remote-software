/******************************************************************************
 *
 * Copyright (C) 2020 Marton Borzak <hello@martonborzak.com>
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

Item {
    anchors.fill: parent

    Text {
        id: titleText
        color: Style.color.text
        width: parent.width - 80
        text: qsTr("Use the web configurator\nto set up YIO remote") + translateHandler.emptyString
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors { verticalCenter: parent.verticalCenter; verticalCenterOffset: -50; horizontalCenter: parent.horizontalCenter }
        font: Style.font.button
    }

    Text {
        id: smallText
        color: Style.color.text
        opacity: 0.5
        text: wifi.wifiStatus.ipAddress == "" ? qsTr("Loading") + translateHandler.emptyString : qsTr("Navigate your internet browser to: http://") + wifi.wifiStatus.ipAddress + translateHandler.emptyString
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        width: parent.width - 80
        anchors { top: titleText.bottom; topMargin: 20; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 20 }
        lineHeight: 1
    }

    Image {
        visible: wifi.wifiStatus.ipAddress == ""
        asynchronous: true
        width: 26; height: 26
        anchors { top: smallText.bottom; topMargin: 40; horizontalCenter: parent.horizontalCenter }
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/statusbar/statusbar-loader.png"

        RotationAnimator on rotation {
            id: loadingIconAnim
            running: wifi.wifiStatus.ipAddress == ""
            loops: Animation.Infinite
            from: 0; to: 360
            duration: 2000
        }
    }
}
