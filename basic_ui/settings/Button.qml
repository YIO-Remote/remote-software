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

Rectangle {
    id: button
    width: parent.width; height: 120
    radius: Style.cornerRadius
    color: Style.colorDark

    property string title
    property string subtitle: ""
    property string icon
    property string url

    Text {
        id: buttonIcon
        color: Style.colorText
        text: icon
        renderType: Text.NativeRendering
        width: 70; height: 70
        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
        font {family: "icons"; pixelSize: 80 }
        anchors { left: parent.left; leftMargin: 15; verticalCenter: parent.verticalCenter }
    }

    Text {
        id: titleText
        color: Style.colorText
        text: title
        anchors { left: buttonIcon.right; leftMargin: 15; verticalCenter: parent.verticalCenter; verticalCenterOffset: subtitle == "" ? 0 : -16 }
        font: Style.buttonFont
        lineHeight: 1
    }

    Text {
        id: subTitleText
        visible: subtitle != ""
        color: Style.colorText
        opacity: 0.5
        text: subtitle
        anchors { left: titleText.left; top: titleText.bottom; topMargin: 10 }
        font { family: "Open Sans Regular"; pixelSize: 20 }
        lineHeight: 1
    }

    Text {
        id: forwardIcon
        color: Style.colorText
        text: Style.icons.right_arrow
        renderType: Text.NativeRendering
        width: 70; height: 70
        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
        font {family: "icons"; pixelSize: 80 }
        anchors { right: parent.right; verticalCenter: parent.verticalCenter }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            loadPage(url, title);
        }
    }
}
