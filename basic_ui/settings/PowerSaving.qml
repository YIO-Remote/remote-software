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

Item {
    width: parent.width
    height: header.height + section.height + 20

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // POWER SAVING
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text {
        id: header
        color: Style.colorText
        text: qsTr("Power saving") + translateHandler.emptyString
        anchors.top: parent.top
        anchors.left: parent.left
        font.family: "Open Sans"
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
            id: wifioffText
            color: Style.colorText
            text: config.settings.wifitime == 0 ? qsTr("Never turn off Wi-Fi") + translateHandler.emptyString : qsTr("Turn off Wi-Fi after %1 minutes of inactivity").arg(config.settings.wifitime/60) + translateHandler.emptyString
            wrapMode: Text.WordWrap
            width: parent.width-40
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Slider {
            id: wifioffSlider
            from: 0
            value: config.settings.wifitime/60
            to: 60
            stepSize: 1
            live: true

            width: parent.width-40
            anchors.top: section.top
            anchors.topMargin: 114
            anchors.horizontalCenter: parent.horizontalCenter

            property bool setbyUser: false

            background: Rectangle {
                x: parent.leftPadding
                y: wifioffSlider.topPadding + wifioffSlider.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 4
                width: wifioffSlider.availableWidth
                height: implicitHeight
                radius: 4
                color: Style.colorBackground

                Rectangle {
                    width: wifioffSlider.visualPosition * parent.width
                    height: 4
                    radius: 2
                    color: Style.colorBackgroundTransparent

                    Rectangle {
                        width: parent.height
                        height: parent.width
                        anchors.centerIn: parent
                        rotation: -90
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: Style.colorMedium }
                            GradientStop { position: 1.0; color: Style.colorHighlight1 }
                        }
                    }
                }
            }

            handle: Rectangle {
                x: wifioffSlider.leftPadding + wifioffSlider.visualPosition * (wifioffSlider.availableWidth - width)
                y: wifioffSlider.topPadding + wifioffSlider.availableHeight / 2 - height / 2
                implicitWidth: 28
                implicitHeight: 28
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

//            onPressedChanged: {
//                if (!this.pressed) {
//                    config.writeConfig();
//                }
//            }
        }

        Text {
            color: Style.colorText
            opacity: 0.5
            text: qsTr("Never") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: wifioffSlider.bottom
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Text {
            color: Style.colorText
            opacity: 0.5
            text: qsTr("60 minutes") + translateHandler.emptyString
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.top: wifioffSlider.bottom
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Rectangle {
            id: line3
            width: parent.width
            height: 2
            color: Style.colorBackground
            anchors.top: wifioffSlider.bottom
            anchors.topMargin: 40
        }

        Text {
            id: shutdownText
            color: Style.colorText
            text: config.settings.shutdowntime == 0 ? qsTr("Never turn off the remote") + translateHandler.emptyString : qsTr("Turn off the remote after %1 hours of inactivity").arg(config.settings.shutdowntime/60/60) + translateHandler.emptyString
            wrapMode: Text.WordWrap
            width: parent.width-40
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line3.bottom
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Slider {
            id: shutdownSlider
            from: 0
            value: config.settings.shutdowntime/60/60
            to: 8
            stepSize: 0.5
            live: true

            width: parent.width-40
            anchors.top: line3.top
            anchors.topMargin: 114
            anchors.horizontalCenter: parent.horizontalCenter

            property bool setbyUser: false

            background: Rectangle {
                x: parent.leftPadding
                y: shutdownSlider.topPadding + shutdownSlider.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 4
                width: shutdownSlider.availableWidth
                height: implicitHeight
                radius: 4
                color: Style.colorBackground

                Rectangle {
                    width: shutdownSlider.visualPosition * parent.width
                    height: 4
                    radius: 2
                    color: Style.colorBackgroundTransparent

                    Rectangle {
                        width: parent.height
                        height: parent.width
                        anchors.centerIn: parent
                        rotation: -90
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: Style.colorMedium }
                            GradientStop { position: 1.0; color: Style.colorHighlight1 }
                        }
                    }
                }
            }

            handle: Rectangle {
                x: shutdownSlider.leftPadding + shutdownSlider.visualPosition * (shutdownSlider.availableWidth - width)
                y: shutdownSlider.topPadding + shutdownSlider.availableHeight / 2 - height / 2
                implicitWidth: 28
                implicitHeight: 28
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

//            onPressedChanged: {
//                if (!this.pressed) {
//                    config.writeConfig();
//                }
//            }
        }

        Text {
            color: Style.colorText
            opacity: 0.5
            text: qsTr("Never") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: shutdownSlider.bottom
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Text {
            color: Style.colorText
            opacity: 0.5
            text: qsTr("8 hours") + translateHandler.emptyString
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.top: shutdownSlider.bottom
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }
    }
}
