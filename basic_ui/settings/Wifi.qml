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

import Haptic 1.0
import WifiControl 1.0

import "qrc:/basic_ui" as BasicUI

Item {
    width: parent.width; height: childrenRect.height

    Rectangle {
        id: containerBluetooth
        width: parent.width; height: childrenRect.height
        radius: Style.cornerRadius
        color: Style.color.dark

        Rectangle {
            id: bluetoothSection
            width: parent.width; height: childrenRect.height + 40
            radius: Style.cornerRadius
            color: Style.color.dark

            Text {
                id: bluetoothText
                color: Style.color.text
                text: qsTr("Area detection") + translateHandler.emptyString
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                font: Style.font.button
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
                }
            }


            Text {
                id: smallText
                color: Style.color.text
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
        anchors { top: containerBluetooth.bottom; topMargin: 20 }
        radius: Style.cornerRadius
        color: Style.color.dark

        Flow {
            id: wifiFlow
            width: parent.width
            spacing: 0

            // WIFI ON OFF
            Item {
                width: parent.width; height: childrenRect.height + 40

                Text {
                    id: wifiOnOffText
                    color: Style.color.text
                    text: qsTr("WiFi") + translateHandler.emptyString
                    anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                    font: Style.font.button
                }

                BasicUI.CustomSwitch {
                    id: autobrightnessButton

                    anchors { right: parent.right; rightMargin: 20; verticalCenter: wifiOnOffText.verticalCenter }

                    checked: wifi.isConnected()
                    mouseArea.onClicked: {
                        if (autobrightnessButton.checked) {
                            wifi.off();
                        } else {
                            wifi.on();
                        }
                    }
                }
            } // WIFI ON OFF END

            Rectangle {
                width: parent.width; height: 2
                color: Style.color.background
            }

            // CURRENT WIFI NETWORK
            Item {
                width: parent.width; height: currentWifiSSID.height + 40

                Text {
                    id: currentWifiSSID
                    color: Style.color.text
                    text: wifi.wifiStatus.name
                    anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                    font: Style.font.button
                }

                Text {
                    color: Style.color.text
                    opacity: 0.3
                    text: Style.icon.wifi_3
                    renderType: Text.NativeRendering
                    width: 70; height: 70
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    font {family: "icons"; pixelSize: 60 }
                    anchors { right: parent.right; verticalCenter: currentWifiSSID.verticalCenter }
                }

                Text {
                    color: Style.color.text
                    text: {
                        if (wifi.wifiStatus.signalStrength == SignalStrengthEnum.WEAK || wifi.wifiStatus.signalStrength == SignalStrengthEnum.NONE)
                            return Style.icon.wifi_1
                        else if (wifi.wifiStatus.signalStrength == SignalStrengthEnum.OK)
                            return Style.icon.wifi_2
                        else if (wifi.wifiStatus.signalStrength == SignalStrengthEnum.GOOD || wifi.wifiStatus.signalStrength == SignalStrengthEnum.EXCELLENT)
                            return Style.icon.wifi_3
                        else return ""
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
                color: Style.color.background
            }

            // OTHER NETWORKS
            Item {
                width: parent.width; height: childrenRect.height + 40

                Text {
                    id: otherNetworksText
                    color: Style.color.highlight1
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
                    triggeredOnStart: true

                    onTriggered: {
                        wifi.startNetworkScan();
                    }
                }

                ListView {
                    model: wifi.networkScanResult
                    anchors { top: otherNetworksText.bottom; topMargin: 20 }
                    width: parent.width; height: childrenRect.height
                    interactive: false

                    delegate: Item {
                        width: parent.width; height: wifi.networkScanResult[index].name === wifi.wifiStatus.name ? 0 : 80
                        visible: wifi.networkScanResult[index].name === wifi.wifiStatus.name ? false : true

                        Text {
                            id: delegateSSID
                            color: Style.color.text
                            text: wifi.networkScanResult[index].name
                            anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                            font: Style.font.button
                        }

                        Text {
                            color: Style.color.text
                            opacity: 0.3
                            text: Style.icon.wifi_3
                            renderType: Text.NativeRendering
                            width: 70; height: 70
                            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                            font {family: "icons"; pixelSize: 60 }
                            anchors { right: parent.right; verticalCenter: delegateSSID.verticalCenter }
                        }

                        Text {
                            color: Style.color.text
                            text: {
                                if (wifi.networkScanResult[index].signalStrength == SignalStrengthEnum.WEAK || wifi.networkScanResult[index].signalStrength == SignalStrengthEnum.NONE)
                                    return Style.icon.wifi_1
                                else if (wifi.networkScanResult[index].signalStrength == SignalStrengthEnum.OK)
                                    return Style.icon.wifi_2
                                else if (wifi.networkScanResult[index].signalStrength == SignalStrengthEnum.GOOD || wifi.networkScanResult[index].signalStrength == SignalStrengthEnum.EXCELLENT)
                                    return Style.icon.wifi_3
                                else return ""
                            }
                            renderType: Text.NativeRendering
                            width: 70; height: 70
                            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                            font {family: "icons"; pixelSize: 60 }
                            anchors { right: parent.right; verticalCenter: delegateSSID.verticalCenter }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                Haptic.playEffect(Haptic.Click);
                                loader_second.setSource("qrc:/basic_ui/settings/WifiConnect.qml", { "obj": wifi.networkScanResult[index]})
                                loader_second.active = true;
                            }
                        }

                    }

                    add: Transition {
                        PropertyAnimation { properties: "opacity"; from: 0; to: 1; duration: 400; easing.type: Easing.OutExpo }
                    }

                    displaced: Transition {
                        PropertyAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBounce }
                    }
                }
            }
        }
    }
}
