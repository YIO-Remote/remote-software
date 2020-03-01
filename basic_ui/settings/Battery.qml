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

import StandbyControl 1.0
import Battery 1.0

Rectangle {
    id: container
    width: parent.width; height: childrenRect.height
    radius: Style.cornerRadius
    color: Style.colorDark

    Component.onCompleted: {
        getHours();
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property var hours: []


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    function getHours() {
        if (hours === [] ) {
            hours.push(StandbyControl.batteryData[StandbyControl.batteryData.length-1].timestamp.getHours());
        }

        var tmp;
        tmp = hours;

        for (var i=StandbyControl.batteryData.length-1; i>0; i--) {
            if ( hours.indexOf(StandbyControl.batteryData[i].timestamp.getHours()) == -1 ) {
                tmp.push(StandbyControl.batteryData[i].timestamp.getHours());
            }
        }
        hours = tmp;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: StandbyControl

        onBatteryDataChanged: {
            getHours();
        }

    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Flow {
        id: settingsFlow
        width: parent.width
        spacing: 0

        // BATTERY HEALTH
        Item {
            width: parent.width; height: childrenRect.height + 40

            Text {
                id: batteryhealthText
                color: Style.colorText
                text: qsTr("Battery health") + translateHandler.emptyString
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                font: Style.fonts.button
            }

            Text {
                color: Style.colorText
                text: Battery.health + "%"
                horizontalAlignment: Text.AlignRight
                anchors { right: parent.right; rightMargin: 20; verticalCenter: batteryhealthText.verticalCenter }
                font: Style.fonts.button
            }
        }

        Rectangle {
            width: parent.width; height: 2
            color: Style.colorBackground
        }

        // REMAINGING TIME
        Item {
            width: parent.width; height: childrenRect.height + 40

            Text {
                id: remainingTimeText
                color: Style.colorText
                text: qsTr("Remaining time") + translateHandler.emptyString
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                font: Style.fonts.button
            }

            Text {
                color: Style.colorText
                text: {
                    var hours = Battery.remainingLife;
                    var rhours = Math.floor(hours);
                    var minutes = (hours - rhours) * 60;
                    var rminutes = Math.round(minutes);

                    return rhours + "h " + rminutes + "m"
                }
                horizontalAlignment: Text.AlignRight
                anchors { right: parent.right; rightMargin: 20; verticalCenter: remainingTimeText.verticalCenter }
                font: Style.fonts.button
            }
        }

        Rectangle {
            width: parent.width; height: 2
            color: Style.colorBackground
        }

        // SCREEN ON OFF TIMES
        Item {
            width: parent.width; height: childrenRect.height + 40

            Text {
                id: screenOnTimeText
                color: Style.colorHighlight1
                text: qsTr("Screen on") + translateHandler.emptyString
                wrapMode: Text.WordWrap
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                font { family: "Open Sans Regular"; pixelSize: 20 }
                lineHeight: 1
            }

            Text {
                id: screenOnTimeTextData
                text: StandbyControl.screenOnTime
                color: Style.colorText
                horizontalAlignment: Text.AlignRight
                anchors { left: screenOnTimeText.left; top: screenOnTimeText.bottom; topMargin: 10 }
                font: Style.fonts.button
            }

            Text {
                id: screenOffTimeText
                color: Style.colorHighlight1
                text: qsTr("Screen off") + translateHandler.emptyString
                wrapMode: Text.WordWrap
                anchors { left: parent.left; leftMargin: parent.width/2; top: parent.top; topMargin: 20 }
                font { family: "Open Sans Regular"; pixelSize: 20 }
                lineHeight: 1
            }

            Text {
                id: screenOffTimeTextData
                text: StandbyControl.screenOffTime
                color: Style.colorText
                horizontalAlignment: Text.AlignRight
                anchors { left: screenOffTimeText.left; top: screenOffTimeText.bottom; topMargin: 10 }
                font: Style.fonts.button
            }
        }

        Rectangle {
            width: parent.width; height: 2
            color: Style.colorBackground
        }

        // BATTERY LEVEL
        Item {
            width: parent.width; height: childrenRect.height + 40

            Text {
                id: batteryLevelText
                color: Style.colorHighlight1
                text: qsTr("Battery level") + translateHandler.emptyString
                wrapMode: Text.WordWrap
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
                font { family: "Open Sans Regular"; pixelSize: 20 }
                lineHeight: 1
            }

            Image {
                id: batteryLevelBG
                asynchronous: true
                width: 420; height: 120
                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/settings/battery-level.png"
                anchors { top: batteryLevelText.bottom; topMargin: 20; horizontalCenter: parent.horizontalCenter }

                ColorOverlay {
                    visible: !Style.darkMode
                    anchors.fill: parent
                    source: parent
                    color: Style.colorText
                    antialiasing: true
                }
            }

            Item {
                width: batteryLevelBG.width; height: batteryLevelBG.height
                anchors.centerIn: batteryLevelBG

                Row {
                    anchors { left: parent.left; bottom: parent.bottom; bottomMargin: 13 }
                    spacing: 4

                    Repeater {
                        model: StandbyControl.batteryData.length > 35 ? 36 : StandbyControl.batteryData.length

                        Rectangle {
                            width: 6; height: 96 * StandbyControl.batteryData[index].level/100
                            color: StandbyControl.batteryData[index].power < 0 ? Style.colorText : Style.colorGreen
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
                anchors { left: parent.left; leftMargin: 20; top: batteryLevelBG.bottom; topMargin: 10 }
                font { family: "Open Sans Regular"; pixelSize: 20 }
                lineHeight: 1
            }

            Image {
                id: averagePowerBG
                asynchronous: true
                width: 420; height: 120
                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/settings/average-power.png"
                anchors { top: averagePowerText.bottom; topMargin: 20; horizontalCenter: parent.horizontalCenter }

                ColorOverlay {
                    visible: !Style.darkMode
                    anchors.fill: parent
                    source: parent
                    color: Style.colorText
                    antialiasing: true
                }
            }

            Item {
                width: averagePowerBG.width; height: averagePowerBG.height
                anchors.centerIn: averagePowerBG

                Row {
                    anchors { left: parent.left; bottom: parent.bottom; bottomMargin: 13 }
                    spacing: 4

                    Repeater {
                        model: StandbyControl.batteryData.length > 35 ? 36 : StandbyControl.batteryData.length

                        Rectangle {
                            width: 6; height: (96 * (Math.abs(StandbyControl.batteryData[index].power) / 5500)) + 1
                            color: StandbyControl.batteryData[index].power < 0 ? Style.colorText : Style.colorGreen
                            anchors.bottom: parent.bottom
                        }
                    }
                }
            }


            Item {
                width: 360; height: 20
                anchors { top: averagePowerBG.bottom; topMargin: 20; left: parent.left; leftMargin: 20 }

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
                            font { family: "Open Sans Regular"; pixelSize: 16 }
                            lineHeight: 1
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        } // BATTERY LEVEL END

        Rectangle {
            width: parent.width; height: 2
            color: Style.colorBackground
        }

        // POWER SAVING
        // WIFI
        Item {
            width: parent.width; height: childrenRect.height + 40

            Text {
                id: powerSavingText
                color: Style.colorText
                text: qsTr("Power saving") + translateHandler.emptyString
                font { family: "Open Sans Bold"; weight: Font.Bold; pixelSize: 27 }
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
            }

            Text {
                id: wifioffText
                color: Style.colorText
                text: config.settings.wifitime === 0 ? qsTr("Never turn off Wi-Fi") + translateHandler.emptyString : qsTr("Turn off Wi-Fi after %1 minutes of inactivity").arg(config.settings.wifitime/60) + translateHandler.emptyString
                wrapMode: Text.WordWrap
                width: parent.width-40
                anchors { left: parent.left; leftMargin: 20; top: powerSavingText.bottom; topMargin: 20 }
                font: Style.fonts.button
            }

            Slider {
                id: wifioffSlider
                from: 0
                to: 60
                value: config.settings.wifitime/60
                stepSize: 1
                live: true

                width: parent.width-40
                anchors { top: wifioffText.bottom; topMargin: 20; horizontalCenter: parent.horizontalCenter }

                property bool setbyUser: false

                background: Rectangle {
                    id: wifiSliderBg
                    x: parent.leftPadding
                    y: wifioffSlider.topPadding + wifioffSlider.availableHeight / 2 - height / 2
                    implicitWidth: 200; implicitHeight: wifioffSlider.pressed ? 28 : 4
                    width: wifioffSlider.availableWidth; height: implicitHeight
                    radius: wifioffSlider.pressed ? 14 : 2
                    color: Style.colorBackground

                    layer.enabled: true
                    layer.effect: OpacityMask {
                        maskSource:
                            Rectangle {
                            width: wifiSliderBg.width; height: wifiSliderBg.height
                            radius: wifiSliderBg.radius

                            Behavior on radius {
                                NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
                            }
                        }
                    }

                    Behavior on height {
                        PropertyAnimation {
                            duration: 300
                            easing.type: Easing.OutExpo
                        }
                    }

                    Rectangle {
                        width: wifioffSlider.visualPosition * parent.width; height: wifioffSlider.pressed ? 28 : 4
                        radius: 2
                        color: Style.colorHighlight1

                        Behavior on height {
                            PropertyAnimation {
                                duration: 300
                                easing.type: Easing.OutExpo
                            }
                        }
                    }
                }

                handle: Rectangle {
                    x: wifioffSlider.visualPosition * (wifioffSlider.availableWidth - width/2)
                    y: wifioffSlider.topPadding + wifioffSlider.availableHeight / 2 - height / 2
                    implicitWidth: 28; implicitHeight: 28
                    radius: Style.cornerRadius
                    color: Style.colorLine
                }

                onValueChanged: {
                    var tmp = config.settings;
                    tmp.wifitime = wifioffSlider.value*60;
                    if (setbyUser) {
                        config.settings = tmp;
                        setbyUser = false;
                    }
                }

                onMoved: {
                    setbyUser = true;
                }
            }

            Text {
                color: Style.colorText
                opacity: 0.5
                text: qsTr("Never") + translateHandler.emptyString
                anchors { left: parent.left; leftMargin: 20; top: wifioffSlider.bottom }
                font { family: "Open Sans Regular"; pixelSize: 20 }
                lineHeight: 1
            }

            Text {
                color: Style.colorText
                opacity: 0.5
                text: qsTr("60 minutes") + translateHandler.emptyString
                anchors { right: parent.right; rightMargin: 20; top: wifioffSlider.bottom }
                font { family: "Open Sans Regular"; pixelSize: 20 }
                lineHeight: 1
            }
        } // POWER SAVING END

        Rectangle {
            width: parent.width; height: 2
            color: Style.colorBackground
        }

        // SHUTDOWN
        Item {
            width: parent.width; height: childrenRect.height + 40

            Text {
                id: shutdownText
                color: Style.colorText
                text: config.settings.shutdowntime === 0 ? qsTr("Never turn off the remote") + translateHandler.emptyString : qsTr("Turn off the remote after %1 hours of inactivity").arg(config.settings.shutdowntime/60/60) + translateHandler.emptyString
                wrapMode: Text.WordWrap
                width: parent.width-40
                font { family: "Open Sans Bold"; pixelSize: 27 }
                anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
            }

            Slider {
                id: shutdownSlider
                from: 0
                to: 8
                value: config.settings.shutdowntime/60/60
                stepSize: 0.5
                live: true

                width: parent.width-40
                anchors { top: shutdownText.bottom; topMargin: 20; horizontalCenter: parent.horizontalCenter }

                property bool setbyUser: false

                background: Rectangle {
                    id: shutdownSliderBg
                    x: parent.leftPadding
                    y: shutdownSlider.topPadding + shutdownSlider.availableHeight / 2 - height / 2
                    implicitWidth: 200; implicitHeight: shutdownSlider.pressed ? 28 : 4
                    width: shutdownSlider.availableWidth; height: implicitHeight
                    radius: shutdownSlider.pressed ? 14 : 2
                    color: Style.colorBackground

                    layer.enabled: true
                    layer.effect: OpacityMask {
                        maskSource:
                            Rectangle {
                            width: shutdownSliderBg.width; height: shutdownSliderBg.height
                            radius: shutdownSliderBg.radius

                            Behavior on radius {
                                NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
                            }
                        }
                    }

                    Behavior on height {
                        PropertyAnimation {
                            duration: 300
                            easing.type: Easing.OutExpo
                        }
                    }

                    Rectangle {
                        width: shutdownSlider.visualPosition * parent.width; height: shutdownSlider.pressed ? 28 : 4
                        radius: 2
                        color: Style.colorHighlight1

                        Behavior on height {
                            PropertyAnimation {
                                duration: 300
                                easing.type: Easing.OutExpo
                            }
                        }
                    }
                }

                handle: Rectangle {
                    x: shutdownSlider.visualPosition * (shutdownSlider.availableWidth - width/2)
                    y: shutdownSlider.topPadding + shutdownSlider.availableHeight / 2 - height / 2
                    implicitWidth: 28; implicitHeight: 28
                    radius: Style.cornerRadius
                    color: Style.colorLine
                }

                onValueChanged: {
                    var tmp = config.settings
                    tmp.shutdowntime = shutdownSlider.value*60*60
                    if (setbyUser) {
                        config.settings = tmp
                        setbyUser = false;
                    }
                }

                onMoved: {
                    setbyUser = true;
                }
            }

            Text {
                color: Style.colorText
                opacity: 0.5
                text: qsTr("Never") + translateHandler.emptyString
                anchors { left: parent.left; leftMargin: 20; top: shutdownSlider.bottom }
                font { family: "Open Sans Regular"; pixelSize: 20 }
                lineHeight: 1
            }

            Text {
                color: Style.colorText
                opacity: 0.5
                text: qsTr("8 hours") + translateHandler.emptyString
                anchors { right: parent.right; rightMargin: 20; top: shutdownSlider.bottom }
                font { family: "Open Sans Regular"; pixelSize: 20 }
                lineHeight: 1
            }
        }
    }
}
