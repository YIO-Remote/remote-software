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

import WifiControl 1.0

import "qrc:/basic_ui" as BasicUI

Item {
    Rectangle {
        id: containerBluetooth
        width: parent.width; height: childrenRect.height
        radius: Style.cornerRadius
        color: Style.colorDark

        Rectangle {
            id: bluetoothSection
            width: parent.width; height: childrenRect.height + 40
            radius: Style.cornerRadius
            color: Style.colorDark

            Text {
                id: bluetoothText
                color: Style.colorText
                text: qsTr("Area detection") + translateHandler.emptyString
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                font: Style.buttonFont
            }

            BasicUI.CustomSwitch {
                id: bluetoothButton

                anchors { right: parent.right; rightMargin: 20; verticalCenter: bluetoothText.verticalCenter }

                checked: config.settings.bluetootharea
                mouseArea.onClicked: {
                    if (bluetoothButton.checked) {
                        bluetoothArea.stopScan();
                        bluetoothArea.turnOff();
                    } else {
                        bluetoothArea.startScan();
                    }

                    var tmp = config.config
                    tmp.settings.bluetootharea = !tmp.settings.bluetootharea
                    config.config = tmp;
                    config.writeConfig();
                }
            }


            Text {
                id: smallText
                color: Style.colorText
                opacity: 0.5
                text: qsTr("Automatically scans for bluetooth beacons associated with areas, finds the closest and displays devices in that area.") + translateHandler.emptyString
                wrapMode: Text.WordWrap
                width: parent.width - 40 - bluetoothButton.width
                anchors { left: parent.left; leftMargin: 20; top: bluetoothButton.bottom; topMargin: 10 }
                font { family: "Open Sans Regular"; pixelSize: 20 }
                lineHeight: 1
            }
        }
    }

    Rectangle {
        id: containerWifi
        width: parent.width; height: childrenRect.height
        anchors { top: containerBluetooth.bottom; topMargin: 10 }
        radius: Style.cornerRadius
        color: Style.colorDark

        Flow {
            id: wifiFlow
            width: parent.width
            spacing: 0

            // WIFI ON OFF
            Item {
                width: parent.width; height: childrenRect.height + 40

                Text {
                    id: wifiOnOffText
                    color: Style.colorText
                    text: qsTr("WiFi") + translateHandler.emptyString
                    anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                    font: Style.buttonFont
                }

                BasicUI.CustomSwitch {
                    id: autobrightnessButton

                    anchors { right: parent.right; rightMargin: 20; verticalCenter: wifiOnOffText.verticalCenter }

                    checked: wifi.isConnected()
                    mouseArea.onClicked: {
                        // implement wifi turn off?
                    }
                }
            } // WIFI ON OFF END

            Rectangle {
                width: parent.width; height: 2
                color: Style.colorBackground
            }

            // CURRENT WIFI NETWORK
            Item {
                width: parent.width; height: currentWifiSSID.height + 40

                Text {
                    id: currentWifiSSID
                    color: Style.colorText
                    text: wifi.wifiStatus.name + translateHandler.emptyString
                    anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                    font: Style.buttonFont
                }

                Text {
                    color: Style.colorText
                    opacity: 0.3
                    text: Style.icons.wifi_3
                    renderType: Text.NativeRendering
                    width: 70; height: 70
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    font {family: "icons"; pixelSize: 60 }
                    anchors { right: parent.right; verticalCenter: currentWifiSSID.verticalCenter }
                }

                Text {
                    id: buttonIcon
                    color: Style.colorText
                    text: {
                        if (wifi.wifiStatus.rssi <= -88)
                            return Style.icons.wifi_3
                        else if (wifi.wifiStatus.rssi > -88 && wifi.wifiStatus.rssi <= -30)
                            return Style.icons.wifi_2
                        else
                            return Style.icons.wifi_1
                    }
                    renderType: Text.NativeRendering
                    width: 70; height: 70
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    font {family: "icons"; pixelSize: 60 }
                    anchors { right: parent.right; verticalCenter: currentWifiSSID.verticalCenter }
                }
            }

            Rectangle {
                width: parent.width; height: 2
                color: Style.colorBackground
            }

            // OTHER NETWORKS
            Item {
                width: parent.width; height: childrenRect.height + 40

                Text {
                    color: Style.colorHighlight1
                    text: qsTr("Other networks") + translateHandler.emptyString
                    wrapMode: Text.WordWrap
                    anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                    font { family: "Open Sans Regular"; pixelSize: 20 }
                    lineHeight: 1
                }

                // scanning for wifi networks every 10 seconds
                Timer {
                    repeat: true
                    running: true
                    interval: 10000

                    onTriggered: {
                        wifi.startNetworkScan();
                    }
                }

                Connections {
                    target: wifi

                    onNetworksFound: {
                        console.debug(wifi.networkScanResult);
                    }
                }
            }
        }
    }
}
