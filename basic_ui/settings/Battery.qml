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
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import Style 1.0

import Launcher 1.0
import StandbyControl 1.0

Item {
    width: parent.width
    height: header.height + section.height + 20

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BATTERY
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Component.onCompleted: {
        getHours();
    }

    property var hours: []

    function getHours() {
        if (hours === [] ) {
            hours.push(battery_data[battery_data.length-1].timestamp.getHours());
        }

        var tmp;
        tmp = hours;

        for (var i=battery_data.length-1; i>0; i--) {
            if ( hours.indexOf(battery_data[i].timestamp.getHours()) == -1 ) {
                tmp.push(battery_data[i].timestamp.getHours());
            }
        }
        hours = tmp;
    }

    Connections {
        target: applicationWindow

        onBatteryDataUpdated: {
            getHours();
        }

    }

    Text {
        id: header
        color: Style.colorText
        text: qsTr("Battery") + translateHandler.emptyString
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

        Text {
            id: batteryhealthText
            color: Style.colorText
            text: qsTr("Battery health") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            color: Style.colorText
            text: battery_health + "%"
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: batteryhealthText.verticalCenter
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line1
            width: parent.width
            height: 2
            color: Style.colorBackground
            anchors.top: batteryhealthText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: screenOnTimeText
            color: Style.colorHighlight1
            text: qsTr("Screen on") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line1.bottom
            anchors.topMargin: 10
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Text {
            id: screenOnTimeTextData
            text: StandbyControl.screenOnTime
            color: Style.colorText
            horizontalAlignment: Text.AlignRight
            anchors.left: screenOnTimeText.left
            anchors.top: screenOnTimeText.bottom
            anchors.topMargin: 10
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: screenOffTimeText
            color: Style.colorHighlight1
            text: qsTr("Screen off") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            anchors.left: parent.left
            anchors.leftMargin: parent.width/2
            anchors.top: line1.bottom
            anchors.topMargin: 10
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Text {
            id: screenOffTimeTextData
            text: StandbyControl.screenOffTime
            color: Style.colorText
            horizontalAlignment: Text.AlignRight
            anchors.left: screenOffTimeText.left
            anchors.top: screenOffTimeText.bottom
            anchors.topMargin: 10
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line2
            width: parent.width
            height: 2
            color: Style.colorBackground
            anchors.top: screenOnTimeTextData.bottom
            anchors.topMargin: 20
        }

        Text {
            id: batteryLevelText
            color: Style.colorHighlight1
            text: qsTr("Battery level") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line2.bottom
            anchors.topMargin: 10
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Image {
            id: batteryLevelBG
            asynchronous: true
            width: 420
            height: 120
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/settings/battery-level.png"
            anchors.top: batteryLevelText.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter

            ColorOverlay {
                visible: !Style.darkMode
                anchors.fill: parent
                source: parent
                color: Style.colorText
                antialiasing: true
            }
        }

        Item {
            width: batteryLevelBG.width
            height: batteryLevelBG.height
            anchors.centerIn: batteryLevelBG

            Row {
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 13

                spacing: 4

                Repeater {
                    model: battery_data.length > 35 ? 36 : battery_data.length

                    Rectangle {
                        width: 6
                        height: 96 * battery_data[index].level
                        color: battery_data[index].power < 0 ? Style.colorText : Style.colorGreen
                        anchors.bottom: parent.bottom
                    }
                }
            }
        }

        Text {
            id: averagePowerText
            color: Style.colorHighlight1
            text: qsTr("Average power") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: batteryLevelBG.bottom
            anchors.topMargin: 10
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Image {
            id: averagePowerBG
            asynchronous: true
            width: 420
            height: 120
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/settings/average-power.png"
            anchors.top: averagePowerText.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter

            ColorOverlay {
                visible: !Style.darkMode
                anchors.fill: parent
                source: parent
                color: Style.colorText
                antialiasing: true
            }
        }

        Item {
            width: averagePowerBG.width
            height: averagePowerBG.height
            anchors.centerIn: averagePowerBG

            Row {
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 13

                spacing: 4

                Repeater {
                    model: battery_data.length > 35 ? 36 : battery_data.length

                    Rectangle {
                        width: 6
                        height: (96 * (Math.abs(battery_data[index].power) / 5500)) + 1
                        color: battery_data[index].power < 0 ? Style.colorText : Style.colorGreen
                        anchors.bottom: parent.bottom
                    }
                }
            }
        }

        Item {
            width: 360
            height: 20
            anchors.top: averagePowerBG.bottom
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: 20

            RowLayout {
                layoutDirection: Qt.RightToLeft
                Layout.fillWidth: false
                spacing: 30
                anchors.left: parent.left

                Repeater {
                    model: hours.length > 7 ? 8 : (hours.length == 0 ? 1 : hours.length)

                    delegate: Text {
                        //: Battery level history data. When no data avaialable yet it shows this text under the empty graph.
                        text: hours.length == 0 ? (qsTr("No data available yet") + translateHandler.emptyString) : hours[hours.length-1-index]
                        color: Style.colorHighlight1
                        font.family: "Open Sans Regular"
                        font.weight: Font.Normal
                        font.pixelSize: 16
                        lineHeight: 1
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

        }
    }

}
