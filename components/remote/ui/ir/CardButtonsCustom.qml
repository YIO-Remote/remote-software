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

import Haptic 1.0
import Entity.Remote 1.0

Item {
    width: parent.width - 60
    anchors.horizontalCenter: parent.horizontalCenter

    Flickable {
        id: customButtonFlick
        width: parent.width - 60
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        contentWidth: parent.width
        contentHeight: customButtonRep.count * (70 + customButtonColumn.spacing) // Button height = 70
        clip: true
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: customButtonColumn
            width: parent.width - 60
            spacing: 32
            Repeater {
                id: customButtonRep
                model: obj.customButtons()

                Button {
                    visible: true
                    width: parent.width
                    title: qsTr(modelData.replace(new RegExp("_", "g"), ' ')) + translateHandler.emptyString
                    mouseArea.onClicked: { obj.customCommand(index); }
                    //Component.onCompleted: console.log("button height:", height)
                }
            }
        }

        Component.onCompleted: {
            //console.log("customButtonFlick.contentHeight:", customButtonFlick.contentHeight)
            //console.log("customButtonRep.count:", customButtonRep.count)
        }
    }
}
