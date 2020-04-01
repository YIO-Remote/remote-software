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
import Style 1.0

import Haptic 1.0

Item {
    width: 66
    height: 36

    property alias checked: button.checked
    property alias mouseArea: mouseArea
    property alias _opacity: button.opacity

    Switch {
        id: button
        implicitHeight: 36
        implicitWidth: 66
        enabled: mouseArea.enabled

        indicator: Rectangle {
            x: 4 + (button.visualPosition * (button.width - width - 8))
            y: (button.height - height) / 2
            width: 26
            height: 26
            radius: Style.cornerRadius
            color: button.checked ? Style.color.text : Style.color.light

            Behavior on x {
                enabled: !button.pressed
                SmoothedAnimation { velocity: 150 }
            }
        }

        background: Rectangle {
            radius: Style.cornerRadius+2
            color: button.checked ? Style.color.highlight1 : Style.color.dark
            border.width: 2
            border.color: button.checked ? Style.color.highlight1 : Style.color.light
        }
    }

    MouseArea {
        id: mouseArea
        width: button.width + 60
        height: button.height + 60

        onClicked: {
            Haptic.playEffect(Haptic.Click);
            button.toggle();
        }
    }
}
