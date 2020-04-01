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
import Style 1.0

Rectangle {
    id: main
    width: title.implicitWidth + 40
    height: 40
    color: selected ? Style.color.line : Style.color.backgroundTransparent
    radius: height/2
    border.color: Style.color.line
    border.width: 2
    opacity: selected ? 1 : 0.5

    Behavior on opacity {
        PropertyAnimation {
            duration: 300
            easing.type: Easing.OutExpo
        }
    }

    property bool selected: false
    property alias tag: title.text

    Text {
        id: title
        color: selected ? Style.color.background : Style.color.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: "Open Sans Regular"
        font.pixelSize: 20
        lineHeight: 1
        anchors.centerIn: parent
    }

}
