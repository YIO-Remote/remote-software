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
                color: Style.color.red
                mouseArea.onClicked: { obj.red(); }
                mouseArea.onPressed: {}
                mouseArea.onReleased: { redButton.color = Style.color.red }
                mouseArea.onCanceled: { redButton.color = Style.color.red }
            }

            Button {
                id: greenButton
                visible: obj.isSupported(Remote.F_FUNCTION_GREEN)
                width: height
                border { color: Style.color.dark; width: 2 }
                color: Style.color.green
                mouseArea.onClicked: { obj.green(); }
                mouseArea.onPressed: {}
                mouseArea.onReleased: { greenButton.color = Style.color.green }
                mouseArea.onCanceled: { greenButton.color = Style.color.green }
            }

            Button {
                id: yellowButton
                visible: obj.isSupported(Remote.F_FUNCTION_YELLOW)
                width: height
                border { color: Style.color.dark; width: 2 }
                color: Style.color.yellow
                mouseArea.onClicked: { obj.yellow(); }
                mouseArea.onPressed: {}
                mouseArea.onReleased: { yellowButton.color = Style.color.yellow }
                mouseArea.onCanceled: { yellowButton.color = Style.color.yellow }
            }

            Button {
                id: blueButton
                visible: obj.isSupported(Remote.F_FUNCTION_BLUE)
                width: height
                border { color: Style.color.dark; width: 2 }
                color: Style.color.blue
                mouseArea.onClicked: { obj.blue(); }
                mouseArea.onPressed: {}
                mouseArea.onReleased: { blueButton.color = Style.color.blue }
                mouseArea.onCanceled: { blueButton.color = Style.color.blue }
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
                color: Style.color.orange
                mouseArea.onClicked: { obj.orange(); }
                mouseArea.onPressed: {}
                mouseArea.onReleased: { orangeButton.color = Style.color.orange }
                mouseArea.onCanceled: { orangeButton.color = Style.color.orange }
            }
        }
    }
}



