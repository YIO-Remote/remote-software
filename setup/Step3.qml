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

Item {
    id: container
    width: 480; height: 800

    Component.onCompleted: {
        networkScanTimer.start();
    }

    Text {
        id: titleText
        color: Style.colorText
        text: qsTr("Wi-Fi setup") + translateHandler.emptyString
        horizontalAlignment: Text.AlignHCenter
        anchors { top: parent.top; topMargin: 100; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 60 }
        lineHeight: 1
    }

    Text {
        id: smalltext
        color: Style.colorText
        opacity: 0.5
        text: qsTr("Select a Wi-Fi network.") + translateHandler.emptyString
        horizontalAlignment: Text.AlignHCenter
        anchors { top: titleText.bottom; topMargin: 10; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 27 }
        lineHeight: 1
    }

    Timer {
        id: networkScanTimer
        repeat: true
        running: false
        interval: 10000
        triggeredOnStart: true

        onTriggered: {
            wifi.startNetworkScan();
        }
    }

    ListView {
        model: wifi.networkScanResult
        anchors { top: smalltext.bottom; topMargin: 80 }
        width: parent.width; height: childrenRect.height
        interactive: false

        delegate: Item {
            width: parent.width; height: 80

            Text {
                id: delegateSSID
                color: Style.colorText
                text: wifi.networkScanResult[index].name
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
                anchors { right: parent.right; verticalCenter: delegateSSID.verticalCenter }
            }

            Text {
                color: Style.colorText
                text: {
                    if (wifi.networkScanResult[index].signalStrength == SignalStrengthEnum.WEAK || wifi.networkScanResult[index].signalStrength == SignalStrengthEnum.NONE)
                        return Style.icons.wifi_1
                    else if (wifi.networkScanResult[index].signalStrength == SignalStrengthEnum.OK)
                        return Style.icons.wifi_2
                    else if (wifi.networkScanResult[index].signalStrength == SignalStrengthEnum.GOOD || wifi.networkScanResult[index].signalStrength == SignalStrengthEnum.EXCELLENT)
                        return Style.icons.wifi_3
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
                    loader_second.setSource("qrc:/setup/Step4.qml", { "obj": wifi.networkScanResult[index], "swipeViewObj": container.parent.parent })
                    loader_second.active = true;
                    networkScanTimer.stop();
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
