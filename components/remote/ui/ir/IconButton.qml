/******************************************************************************
 *
 * Copyright (C) 2020-2021 Nikolas Slottke <nikoslottke@gmail.com>
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
import Haptic 1.0

Rectangle {
    id: bg
    width: 118; height: 70
    radius: height/2
    border { color: Style.color.light; width: 2 }
    color: Style.color.backgroundTransparent

    property alias mouseArea: mouseArea
    property alias background: bg.color
    property alias image: image

    Behavior on color {
        PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
    }

    Image {
        id: image
        width: 48; height: 48
        anchors.centerIn: parent
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            Haptic.playEffect(Haptic.Click);
        }
        onPressed: {
            bg.color = Style.color.light
        }
        onReleased: {
            bg.color = Style.color.backgroundTransparent
        }
        onCanceled: {
            bg.color = Style.color.backgroundTransparent
        }
    }
} // Rectangle end
