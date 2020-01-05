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

Rectangle {
    id: bg
    width: 110
    height: 60
    radius: height/2
    border.color: Style.colorLight
    border.width: 2
    color: Style.colorBackgroundTransparent

    property alias title: title.text
    property alias mouseArea: mouseArea

    Text {
        id: title
        color: Style.colorText
        width: parent.width-20
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.centerIn: parent
        font.family: "Open Sans Regular"
        font.weight: Font.Normal
        font.pixelSize: 24
        lineHeight: 1
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            haptic.playEffect(Haptic.Click);
        }
    }
} // Rectangle end
