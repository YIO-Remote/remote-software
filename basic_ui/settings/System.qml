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
import Launcher 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: container
    width: parent.width; height: childrenRect.height + 40
    radius: Style.cornerRadius
    color: Style.colorDark

    Launcher {
        id: settingsLauncher
    }

    Timer {
        id: timer
        running: false
        repeat: true
        interval: 10000
        triggeredOnStart: true

        onTriggered: {
            // TODO create a device class instead of launching hard coded shell scripts from QML
            uptimeValue.text = settingsLauncher.launch("/usr/bin/yio-remote/uptime.sh").trim();
            temperatureValue.text = Math.round(parseInt(settingsLauncher.launch("cat /sys/class/thermal/thermal_zone0/temp"))/1000) + "ºC";
        }
    }

    Component.onCompleted: timer.start()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text {
        id: uptimeText
        color: Style.colorText
        text: qsTr("Uptime") + translateHandler.emptyString
        anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
        font: Style.buttonFont
    }

    Text {
        id: uptimeValue
        color: Style.colorText
        text: "0h"
        horizontalAlignment: Text.AlignRight
        anchors { right: parent.right; rightMargin: 20; verticalCenter: uptimeText.verticalCenter }
        font: Style.buttonFont
    }

    Rectangle {
        id: line7
        width: parent.width
        height: 2
        color: Style.colorBackground
        anchors { top: uptimeText.bottom; topMargin: 20 }
    }

    Text {
        id: temperatureText
        color: Style.colorText
        text: qsTr("CPU temperature") + translateHandler.emptyString
        anchors { left: parent.left; leftMargin: 20; top: line7.bottom; topMargin: 20 }
        font: Style.buttonFont
    }

    Text {
        id: temperatureValue
        color: Style.colorText
        text: "36ºC"
        horizontalAlignment: Text.AlignRight
        anchors { right: parent.right; rightMargin: 20; verticalCenter: temperatureText.verticalCenter }
        font: Style.buttonFont
    }

    Rectangle {
        id: line8
        width: parent.width
        height: 2
        color: Style.colorBackground
        anchors { top: temperatureText.bottom; topMargin: 20 }
    }

    BasicUI.CustomButton {
        id: buttonReboot
        buttonText: qsTr("Reboot") + translateHandler.emptyString
        anchors { top: line8.bottom; topMargin: 30; left: parent.left; leftMargin: (parent.width - (buttonReboot.width + buttonShutdown.width + 40))/2 }

        mouseArea.onClicked: {
            // TODO create a framebuffer device class instead of launching hard coded shell scripts from QML
            settingsLauncher.launch("fbv -d 1 /bye.png")
            console.debug("now reboot")
            // TODO create a device class for system reboot instead of launching hard coded shell scripts from QML
            settingsLauncher.launch("reboot");
        }
    }

    BasicUI.CustomButton {
        id: buttonShutdown
        buttonText: qsTr("Shutdown") + translateHandler.emptyString
        anchors { top: line8.bottom; topMargin: 30; left: buttonReboot.right; leftMargin: 40 }

        mouseArea.onClicked: {
            loadingScreen.source = "qrc:/basic_ui/ClosingScreen.qml";
            loadingScreen.active = true;
        }
    }
}
