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
//import QtQuick.Controls 2.5
//import QtGraphicalEffects 1.0

import "qrc:/scripts/helper.js" as JSHelper
import "qrc:/components" as Comp

Comp.ButtonBase {
    id: climateButton
    icon: "\uE913"
    cardLoader.source: "qrc:/components/climate/ui/Card.qml"

    // override default settings
    title.anchors.verticalCenterOffset: -15

    // additional UI elements
    Text {
        id: info
        color: colorText
        opacity: 0.5
        text: qsTr("Temperature: ") + obj.temperature + obj.temperatureUnit + translateHandler.emptyString
        elide: Text.ElideRight
        wrapMode: Text.WordWrap
        width: title.width
        anchors.left: parent.left
        anchors.leftMargin: title.x
        anchors.top: parent.top
        anchors.topMargin: title.y + title.height
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 20
        lineHeight: 1
        z: climateButton.state == "open" ? (cardLoader.z - 1) : (cardLoader.z + 1)
    }
}
