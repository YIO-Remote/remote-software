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

import Launcher 1.0

import "settings" as Settings

Item {
    width: parent.width-20
    height: settingsFlow.height
    anchors.horizontalCenter: parent.horizontalCenter

    Launcher {
        id: settingsLauncher
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SETTINGS BLOCKS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Flow {
        id: settingsFlow
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 40

        Rectangle {
            width: parent.width
            height: 240
            color: Style.colorBackground

            Text {
                id: titleText
                color: Style.colorText
                text: qsTr("Settings") + translateHandler.emptyString
                anchors.centerIn: parent
                font.family: "Open Sans Regular"
                font.weight: Font.Normal
                font.pixelSize: 60
                lineHeight: 1
            }
        }

        Settings.Display {}

        Settings.Softwareupdate {}

        Settings.PowerSaving {}

        Settings.Languages {}

        Settings.Integrations {
            visible: integrations.list.length == 0 ? false : true
        }

        Settings.Battery {}

        Settings.Bluetooth {}

        Settings.Network {}

        Settings.RemoteConfig {}

        Settings.System {}

    }
}
