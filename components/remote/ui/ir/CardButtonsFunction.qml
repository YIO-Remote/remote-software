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
import QtQuick.Layouts 1.3
import Style 1.0

import Haptic 1.0
import Entity.Remote 1.0

Item {
    width: parent.width - 60
    anchors.horizontalCenter: parent.horizontalCenter

    ColumnLayout {
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
        width: parent.width - 60
        spacing: 32

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            Button {
                id: redButton
                visible: obj.isSupported(Remote.F_FUNCTION_RED)
                width: height
                border { color: Style.color.dark; width: 2 }
                color: "#FF0000"
                mouseArea.onClicked: { obj.red(); }
                mouseArea.onPressed: {}
                mouseArea.onReleased: { redButton.color = "#FF0000" }
                mouseArea.onCanceled: { redButton.color = "#FF0000" }
            }

            Button {
                id: greenButton
                visible: obj.isSupported(Remote.F_FUNCTION_GREEN)
                width: height
                border { color: Style.color.dark; width: 2 }
                color: "#008000"
                mouseArea.onClicked: { obj.green(); }
                mouseArea.onPressed: {}
                mouseArea.onReleased: { greenButton.color = "#008000" }
                mouseArea.onCanceled: { greenButton.color = "#008000" }
            }

            Button {
                id: yellowButton
                visible: obj.isSupported(Remote.F_FUNCTION_YELLOW)
                width: height
                border { color: Style.color.dark; width: 2 }
                color: "#FFFF00"
                mouseArea.onClicked: { obj.yellow(); }
                mouseArea.onPressed: {}
                mouseArea.onReleased: { yellowButton.color = "#FFFF00" }
                mouseArea.onCanceled: { yellowButton.color = "#FFFF00" }
            }

            Button {
                id: blueButton
                visible: obj.isSupported(Remote.F_FUNCTION_BLUE)
                width: height
                border { color: Style.color.dark; width: 2 }
                color: "#0000FF"
                mouseArea.onClicked: { obj.blue(); }
                mouseArea.onPressed: {}
                mouseArea.onReleased: { blueButton.color = "#0000FF" }
                mouseArea.onCanceled: { blueButton.color = "#0000FF" }
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            Button {
                id: orangeButton
                visible: obj.isSupported(Remote.F_FUNCTION_ORANGE)
                width: height
                border { color: Style.color.dark; width: 2 }
                color: "#FFA500"
                mouseArea.onClicked: { obj.orange(); }
                mouseArea.onPressed: {}
                mouseArea.onReleased: { orangeButton.color = "#FFA500" }
                mouseArea.onCanceled: { orangeButton.color = "#FFA500" }
            }
        }
    }
}



