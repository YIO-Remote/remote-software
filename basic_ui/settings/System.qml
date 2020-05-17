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
import Config 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: container
    width: parent.width; height: childrenRect.height + 40
    radius: Style.cornerRadius
    color: Style.color.dark

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

    Flow {
        id: flow
        width: parent.width
        spacing: 0

        Softwareupdate {}

        Rectangle {
            width: parent.width; height: 2
            color: Style.color.background
        }

        Item {
            width: parent.width; height: childrenRect.height + 40

            Text {
                id: unitText
                color: Style.color.text
                text: qsTr("Unit system") + translateHandler.emptyString
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                font: Style.font.button
            }

            CheckBox {
                id: metricCheckBox
                checked: config.unitSystem == 0
                anchors { left: parent.left; leftMargin: 20; top: unitText.bottom; topMargin: 20 }

                indicator: Rectangle {
                    implicitWidth: 20; implicitHeight: 20
                    anchors.centerIn: parent
                    radius: Style.cornerRadius
                    color: metricCheckBox.checked ? Style.color.text : Style.color.light
                }

                background: Rectangle {
                    radius: 20
                    color: metricCheckBox.checked ? Style.color.highlight1 : Style.color.dark
                    border { width: 2; color: metricCheckBox.checked ? Style.color.highlight1 : Style.color.light }
                }

                onCheckedChanged: {
                    if (checked) {
                        config.unitSystem = 0;
                    }
                }
            }

            Text {
                id: metricCheckBoxText
                color: Style.color.text
                opacity: metricCheckBox.checked ? 1 : 0.5
                text: qsTr("Metric") + translateHandler.emptyString
                anchors { left: metricCheckBox.right; leftMargin: 20; verticalCenter: metricCheckBox.verticalCenter }
                font: Style.font.button

                Behavior on opacity { NumberAnimation { duration: 200 } }
            }

            CheckBox {
                id: imperialCheckBox
                checked: config.unitSystem == 1
                anchors { left: metricCheckBoxText.right; leftMargin: 20; verticalCenter: metricCheckBox.verticalCenter }

                indicator: Rectangle {
                    implicitWidth: 20; implicitHeight: 20
                    anchors.centerIn: parent
                    radius: Style.cornerRadius
                    color: imperialCheckBox.checked ? Style.color.text : Style.color.light
                }

                background: Rectangle {
                    radius: 20
                    color: imperialCheckBox.checked ? Style.color.highlight1 : Style.color.dark
                    border { width: 2; color: imperialCheckBox.checked ? Style.color.highlight1 : Style.color.light }
                }

                onCheckedChanged: {
                    if (checked) {
                        config.unitSystem = 1;
                    }
                }
            }

            Text {
                color: Style.color.text
                opacity: imperialCheckBox.checked ? 1 : 0.5
                text: qsTr("Imperial") + translateHandler.emptyString
                anchors { left: imperialCheckBox.right; leftMargin: 20; verticalCenter: imperialCheckBox.verticalCenter }
                font: Style.font.button

                Behavior on opacity { NumberAnimation { duration: 200 } }
            }
        }

        Rectangle {
            width: parent.width; height: 2
            color: Style.color.background
        }

        Item {
            width: parent.width; height: childrenRect.height + 40

            Text {
                id: uptimeText
                color: Style.color.text
                text: qsTr("Uptime") + translateHandler.emptyString
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                font: Style.font.button
            }

            Text {
                id: uptimeValue
                color: Style.color.text
                text: "0h"
                horizontalAlignment: Text.AlignRight
                anchors { right: parent.right; rightMargin: 20; verticalCenter: uptimeText.verticalCenter }
                font: Style.font.button
            }

        }

        Rectangle {
            width: parent.width; height: 2
            color: Style.color.background
        }

        Item {
            width: parent.width; height: childrenRect.height + 40

            Text {
                id: temperatureText
                color: Style.color.text
                text: qsTr("CPU temperature") + translateHandler.emptyString
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                font: Style.font.button
            }

            Text {
                id: temperatureValue
                color: Style.color.text
                text: "36ºC"
                horizontalAlignment: Text.AlignRight
                anchors { right: parent.right; rightMargin: 20; verticalCenter: temperatureText.verticalCenter }
                font: Style.font.button
            }
        }

        Rectangle {
            width: parent.width; height: 2
            color: Style.color.background
        }

        Item {
            width: parent.width; height: childrenRect.height + 30
            BasicUI.CustomButton {
                id: buttonReboot
                buttonText: qsTr("Reboot") + translateHandler.emptyString
                anchors { top: parent.top; topMargin: 30; left: parent.left; leftMargin: (parent.width - (buttonReboot.width + buttonShutdown.width + 40))/2 }

                mouseArea.onClicked: {
                    // TODO create a framebuffer device class instead of launching hard coded shell scripts from QML
                    settingsLauncher.launch("fbv -d 1 $YIO_MEDIA_DIR/splash/bye.png")
                    console.debug("now reboot")
                    // TODO create a device class for system reboot instead of launching hard coded shell scripts from QML
                    settingsLauncher.launch("reboot");
                }
            }

            BasicUI.CustomButton {
                id: buttonShutdown
                buttonText: qsTr("Shutdown") + translateHandler.emptyString
                anchors { top: parent.top; topMargin: 30; left: buttonReboot.right; leftMargin: 40 }

                mouseArea.onClicked: {
                    loadingScreen.source = "qrc:/basic_ui/ClosingScreen.qml";
                    loadingScreen.active = true;
                }
            }
        }
    }
}
