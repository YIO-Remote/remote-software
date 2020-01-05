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

import Haptic 1.0
import WifiControl 1.0

Item {
    width: parent.width
    height: header.height + section.height + 20

    property var wifiNetworks
    property var wifiNetworkSelected: ""

    function addNetworks() {
        var comp = Qt.createComponent("qrc:/basic_ui/settings/WifiNetworkListElement.qml");

        wifiNetworks = wifi.networkScanResult
        for (var i = 0; i < wifiNetworks.length; i++) {
            console.log("Adding network: " + wifiNetworks[i])
            var obj = comp.createObject(flowWifiList, {ssid: wifiNetworks[i].name, rssi: wifiNetworks[i].signalStrength, buttonId: i});
            obj.clicked.connect(buttonClicked)
        }
    }

    function buttonClicked(buttonId) {
        wifiNetworkSelected = wifiNetworks[buttonId].name;
        wifiSwipeview.currentIndex += 1;
        popup.height = 200;
    }

    Component.onCompleted: {
        wifi.networksFound.connect(addNetworks);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NETWORK
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Text {
        id: header
        color: Style.colorText
        text: qsTr("Network") + translateHandler.emptyString
        anchors.left: parent.left
        font.family: "Open Sans Regular"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: section
        width: parent.width
        height: childrenRect.height + 40
        radius: Style.cornerRadius
        color: Style.colorDark

        anchors.top: header.bottom
        anchors.topMargin: 20

        Item {
            id: wifiSSID
            width: parent.width
            height: 50
            anchors.top: parent.top
            anchors.topMargin: 20

            Text {
                id: wifiSSIDText
                color: Style.colorText
                text: wifi.wifiStatus.name
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                font.family: "Open Sans Regular"
                font.weight: Font.Normal
                font.pixelSize: 27
                lineHeight: 1
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    Haptic.playEffect(Haptic.Click);

                    // Start wifi network scan
                    wifiNetworks = [];

                    // clear list
                    for (var k = flowWifiList.children.length; k>0; k--) {
                        flowWifiList.children[k-1].destroy();
                    }

                    // emits networksFound! See above's Component.onCompleted
                    wifi.startNetworkScan()

                    // open popup that displays the list
                    popup.open();
                }
            }

        }


        // FIXME create an escape functionality. User is stuck in the popup :(
        Popup {
            id: popup
            x: 0
            y: 0
            width: section.width
            height: 400
            modal: true
            focus: true
            clip: true
            closePolicy: Popup.NoAutoClose

            enter: Transition {
                NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; easing.type: Easing.OutExpo; duration: 300 }
                NumberAnimation { property: "width"; from: 0; to: section.width; easing.type: Easing.OutBack; duration: 300 }
                NumberAnimation { property: "height"; from: 0; to: 400; easing.type: Easing.OutBack; duration: 300 }
            }

            exit: Transition {
                NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; easing.type: Easing.InExpo; duration: 300 }
                NumberAnimation { property: "width"; from: section.width; to: 0; easing.type: Easing.InExpo; duration: 300 }
                NumberAnimation { property: "height"; from: 400; to: 0; easing.type: Easing.InExpo; duration: 300 }
            }

            background: Rectangle {
                anchors.fill: parent
                color: Style.colorLine
                radius: Style.cornerRadius
            }

            SwipeView {
                id: wifiSwipeview
                anchors.fill: parent
                interactive: false

                Item {
                    id: page1

                    Flickable {
                        id: flickableWifiList
                        anchors.top: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: parent.width
                        height: parent.height
                        maximumFlickVelocity: 4000
                        flickDeceleration: 1000
                        contentHeight: flowWifiList.height
                        boundsBehavior: Flickable.DragAndOvershootBounds
                        flickableDirection: Flickable.VerticalFlick

                        ScrollBar.vertical: ScrollBar {
                            opacity: 0.5
                        }

                        Flow {
                            id: flowWifiList
                            width: parent.width
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            spacing: 0
                        }
                    }
                }

                Item {
                    id: page2

                    Text {
                        color: Style.colorBackground
                        text: wifiNetworkSelected
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.top: parent.top
                        anchors.topMargin: 20
                        font.family: "Open Sans Regular"
                        font.weight: Font.Normal
                        font.pixelSize: 27
                        lineHeight: 1
                    }

                    TextField {
                            id: textfieldWifiPassword
                            width: parent.width-40
                            anchors.top: parent.top
                            anchors.topMargin: 100
                            anchors.horizontalCenter: parent.horizontalCenter
                            height: 60
                            echoMode: TextInput.PasswordEchoOnEdit
                            placeholderText: "Password"
                            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhPreferLowercase | Qt.ImhSensitiveData | Qt.ImhNoPredictiveText

                            font.family: "Neuzeit Grotesk"
                            font.weight: Font.Normal
                            font.pixelSize: 28
                            color: "#000000"

                            background: Rectangle {
                                implicitWidth: parent.width-40
                                implicitHeight: 60
                                color: Style.colorHighlight1
                            }
                        }
                }
            }
        }


        Rectangle {
            id: line0
            width: parent.width
            height: 2
            color: Style.colorBackground
            anchors.top: wifiSSID.bottom
            anchors.topMargin: 20
        }

        Text {
            id: wifiSignalText
            color: Style.colorText
            text: qsTr("Wi-Fi signal strength") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line0.bottom
            anchors.topMargin: 20
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: wifiSignalValue
            color: Style.colorText
            text: wifi.wifiStatus.rssi
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: wifiSignalText.verticalCenter
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line6
            width: parent.width
            height: 2
            color: Style.colorBackground
            anchors.top: wifiSignalText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: ipaddressText
            color: Style.colorText
            text: qsTr("IP address") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line6.bottom
            anchors.topMargin: 20
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            color: Style.colorText
            text: wifi.wifiStatus.ipAddress
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: ipaddressText.verticalCenter
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line7
            width: parent.width
            height: 2
            color: Style.colorBackground
            anchors.top: ipaddressText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: macaddressText
            color: Style.colorText
            text: qsTr("MAC address") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line7.bottom
            anchors.topMargin: 20
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            color: Style.colorText
            text: wifi.wifiStatus.macAddress
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: macaddressText.verticalCenter
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }
    }
}
