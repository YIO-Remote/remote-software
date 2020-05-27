/******************************************************************************
 *
 * Copyright (C) 2018-2020 Marton Borzak <hello@martonborzak.com>
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
import Launcher 1.0

import "qrc:/basic_ui" as BasicUI

Item {
    id: container
    width: 480; height: 800

    Launcher { id: myLauncher }

    Text {
        id: titleText
        color: Style.color.text
        text: qsTr("Connected") + translateHandler.emptyString
        horizontalAlignment: Text.AlignHCenter
        anchors { top: parent.top; topMargin: 220; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 60 }
        lineHeight: 1
    }

    Text {
        id: smalltext
        color: Style.color.text
        opacity: 0.5
        text: qsTr("YIO Dock has successfully connected to your Wi-Fi network.\n\nTo use the web configurator,\ngo to Settings and turn on\nRemote configuration after restarting the remote.") + translateHandler.emptyString
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        width: parent.width - 80
        anchors { top: titleText.bottom; topMargin: 10; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 27 }
        lineHeight: 1
    }

    BasicUI.CustomButton {
        anchors { bottom: parent.bottom; bottomMargin: 60; horizontalCenter: parent.horizontalCenter }
        buttonText: qsTr("Reboot") + translateHandler.emptyString
        mouseArea.onClicked: {
            Haptic.playEffect(Haptic.Click);
            // reboot remote
            myLauncher.launch("reboot");
        }
    }

    Component.onCompleted: {
        // delete /firstrun
        console.debug("Deleting /firstrun success: " + fileio.deleteFile("/firstrun"));
    }
}
