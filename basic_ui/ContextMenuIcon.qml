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
import Haptic 1.0
import Style 1.0

Rectangle {
    id: circleButton
    width: 30; height: 30
    color: Style.color.line
    radius: width/2

    property var colorBg: Style.color.background
    property alias mouseArea: mouseArea


    Rectangle {
        width: 4; height: 4
        color: colorBg
        radius: width/2
        anchors { left: parent.left; leftMargin: 8; verticalCenter: parent.verticalCenter }
    }

    Rectangle {
        width: 4; height: 4
        color: colorBg
        radius: width/2
        anchors.centerIn: parent
    }

    Rectangle {
        width: 4; height: 4
        color: colorBg
        radius: width/2
        anchors { right: parent.right; rightMargin: 8; verticalCenter: parent.verticalCenter }
    }

    MouseArea {
        id: mouseArea
        width: parent.width+20; height: parent.height+20
        anchors.centerIn: parent

        onClicked: {
            Haptic.playEffect(Haptic.Click);
        }
    }
}
