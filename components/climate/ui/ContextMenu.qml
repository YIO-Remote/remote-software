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

Rectangle {
    id: main
    width: parent.width
    height: 100+(80*list.length)
    radius: cornerRadius
    color: colorBackground

    property var list: []
    property var climateObj

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"

    Component.onCompleted: {
        state = "open";
    }


    states: [
        State { name: "closed";
            PropertyChanges {target: main; y: main.height; enabled: false }
        },
        State { name: "open";
            PropertyChanges {target: main; y: 0; enabled: true }
        }
    ]
    transitions: [
        Transition {to: "closed";
            SequentialAnimation {
                PropertyAnimation { target: main; properties: "y"; easing.type: Easing.OutExpo; duration: 300 }
            }
        },
        Transition {to: "open";
            SequentialAnimation {
                PropertyAnimation { target: main; properties: "y"; easing.type: Easing.OutExpo; duration: 300 }
            }
        }
    ]

    Flow {
        anchors.fill: parent
        spacing: 0

        Item {
            width: main.width
            height: 20
        }

        Repeater {
            model: list

            Item {
                width: main.width
                height: 80

                Text {
                    id: icon
                    color: colorText
                    text: {
                        console.debug(list[index])
                        if (list[index]["original"] === "Off")
                            return "\uE908"
                        else if (list[index]["original"] === "Heat")
                            return "\uE91F"
                        else if (list[index]["original"] === "Cool")
                            return "\uE91E"
                    }
                    renderType: Text.NativeRendering
                    width: 70
                    height: 70
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font { family: "icons"; pixelSize: 80 }
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    text: list[index]["translated"]
                    color: colorText
                    anchors.left: icon.right
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    font.family: "Open Sans"
                    font.pixelSize: 27
                    lineHeight: 1
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        haptic.playEffect("click");
                        main.state = "closed";
                        if (list[index]["original"] === "Off")
                            climateObj.turnOff();
                        if (list[index]["original"] === "Heat")
                            climateObj.heat();
                        if (list[index]["original"] === "Cool")
                            climateObj.cool();
                    }
                }
            }
        }

        Item {
            width: main.width
            height: 80

            Text {
                text: qsTr("Cancel") + translateHandler.emptyString
                color: colorText
                opacity: 0.5
                anchors.centerIn: parent
                font.family: "Open Sans"
                font.pixelSize: 25
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                lineHeight: 1
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    haptic.playEffect("click");
                    main.state = "closed";
                }
            }
        }
    }
}
