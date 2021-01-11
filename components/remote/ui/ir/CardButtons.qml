/******************************************************************************
 *
 * Copyright (C) 2018-2020 Marton Borzak <hello@martonborzak.com>
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
import QtQuick.Layouts 1.3
import Style 1.0

import Haptic 1.0
import ButtonHandler 1.0
import Entity.Remote 1.0


Item {
    width: parent.width - 60
    anchors.horizontalCenter: parent.horizontalCenter

    ColumnLayout {
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 32

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32

            Rectangle {
                id: powerButton
                visible: obj.isSupported(Remote.F_POWER_TOGGLE) || (obj.isSupported(Remote.F_POWER_ON) && obj.isSupported(Remote.F_POWER_OFF))
                width: 118; height: 70
                radius: height/2
                color: Style.color.red

                Text {
                    color: Style.color.line
                    text: Style.icon.power_on
                    width: 70; height: 70
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    font {family: "icons"; pixelSize: 60 }
                    anchors.centerIn: parent
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    onClicked: {
                        Haptic.playEffect(Haptic.Click);
                        if (obj.isSupported(Remote.F_POWER_TOGGLE)) {
                            obj.powerToggle();
                        } else if (obj.isSupported(Remote.F_POWER_ON) && obj.isSupported(Remote.F_POWER_OFF)) {
                            if (obj.isOn)
                                obj.powerOff();
                            else
                                obj.powerOn();
                        }
                    }
                }
            } // Rectangle end

            Button {
                visible: obj.isSupported(Remote.F_SOURCE)
                title: qsTr("Source") + translateHandler.emptyString
                mouseArea.onClicked: { obj.source(); }
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            Button {
                visible: obj.isSupported(Remote.F_DIGIT_1)
                title: "1"
                mouseArea.onClicked: { obj.channel(1); }
            }

            Button {
                visible: obj.isSupported(Remote.F_DIGIT_2)
                title: "2"
                mouseArea.onClicked: { obj.channel(2); }
            }

            Button {
                visible: obj.isSupported(Remote.F_DIGIT_3)
                title: "3"
                mouseArea.onClicked: { obj.channel(3); }
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            Button {
                visible: obj.isSupported(Remote.F_DIGIT_4)
                title: "4"
                mouseArea.onClicked: { obj.channel(4); }
            }

            Button {
                visible: obj.isSupported(Remote.F_DIGIT_5)
                title: "5"
                mouseArea.onClicked: { obj.channel(5); }
            }

            Button {
                visible: obj.isSupported(Remote.F_DIGIT_6)
                title: "6"
                mouseArea.onClicked: { obj.channel(6); }
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            Button {
                visible: obj.isSupported(Remote.F_DIGIT_7)
                title: "7"
                mouseArea.onClicked: { obj.channel(7); }
            }

            Button {
                visible: obj.isSupported(Remote.F_DIGIT_8)
                title: "8"
                mouseArea.onClicked: { obj.channel(8); }
            }

            Button {
                visible: obj.isSupported(Remote.F_DIGIT_9)
                title: "9"
                mouseArea.onClicked: { obj.channel(9); }
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            Button {
                visible: obj.isSupported(Remote.F_DIGIT_SEPARATOR)
                title: "-"
                mouseArea.onClicked: { }
            }

            Button {
                visible: obj.isSupported(Remote.F_DIGIT_0)
                title: "0"
                mouseArea.onClicked: { obj.channel(0); }
            }

            Button {
                visible: obj.isSupported(Remote.F_DIGIT_ENTER)
                title: "E"
                mouseArea.onClicked: { }
            }
        }
    }
}



