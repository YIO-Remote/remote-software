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

import "qrc:/scripts/helper.js" as JSHelper
import "qrc:/scripts/softwareupdate.js" as JSUpdate
import "qrc:/basic_ui" as BasicUI

Item {
    width: parent.width
    height: header.height + section.height + 20

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DISPLAY
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text {
        id: header
        color: Style.colorText
        text: qsTr("Software update") + translateHandler.emptyString
        anchors.left: parent.left
        font.family: "Open Sans Regular"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: section
        width: parent.width
        height: childrenRect.height + 50 //softwareUpdateText.height + smallText.height + 60
        radius: Style.cornerRadius
        color: Style.colorDark

        anchors.top: header.bottom
        anchors.topMargin: 20

        Text {
            id: softwareUpdateText
            color: Style.colorText
            text: qsTr("Auto update") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        BasicUI.CustomSwitch {
            id: softwareUpdateButton

            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: softwareUpdateText.verticalCenter

            checked: config.settings.softwareupdate
            mouseArea.onClicked: {
                var tmp = config.config
                tmp.settings.softwareupdate = !tmp.settings.softwareupdate
                config.config = tmp
                config.writeConfig();
            }
        }


        Text {
            id: smallText
            color: Style.colorText
            opacity: 0.5
            text: qsTr("Automatically look for updates and update when a new software version is available.\nUpdates are installed between 03.00 am and 05.00 am.") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            width: parent.width - 40 - softwareUpdateButton.width
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: softwareUpdateButton.bottom
            anchors.topMargin: 10
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Rectangle {
            id: line
            width: parent.width
            height: 2
            color: Style.colorBackground
            anchors.top: smallText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: uptodateText
            color: Style.colorText
            text: updateAvailable ? qsTr("New software is available.\nYIO remote ") + _new_version + translateHandler.emptyString : qsTr("Your software is up to date.") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            width: parent.width-40
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line.bottom
            anchors.topMargin: 20
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: uptodateTextsmall
            color: Style.colorText
            opacity: 0.5
            text: updateAvailable? qsTr("Installed version: YIO Remote ") + _current_version + translateHandler.emptyString : qsTr("YIO Remote ") + _current_version
            wrapMode: Text.WordWrap
            width: parent.width - 40
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: uptodateText.bottom
            anchors.topMargin: 10
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        BasicUI.CustomButton {
            id: updateButton
            buttonText: qsTr("Update") + translateHandler.emptyString
            anchors.top: uptodateTextsmall.bottom
            anchors.topMargin: 30
            anchors.left: parent.left
            anchors.leftMargin: 20
            visible: updateAvailable

            mouseArea.onClicked: {
                if (updateAvailable) {
                    console.debug("Updating the app")
                    // TODO create a update servcie class instead of launching hard coded shell scripts from QML
                    fileio.write("/usr/bin/updateURL", updateURL);
                    mainLauncher.launch("systemctl restart update.service");
                }
                Qt.quit();
            }
        }

        Rectangle {
            id: line2
            width: parent.width
            height: 2
            color: Style.colorBackground
            anchors.top: updateAvailable ? updateButton.bottom : uptodateTextsmall.bottom
            anchors.topMargin: 30
        }

        BasicUI.CustomButton {
            id: checkforUpdateButton
            buttonText: qsTr("Check for update") + translateHandler.emptyString
            anchors.top: line2.bottom
            anchors.topMargin: 30
            anchors.left: parent.left
            anchors.leftMargin: 20

            mouseArea.onClicked: {
                JSUpdate.checkForUpdate();
            }
        }
    }
}
