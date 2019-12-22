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
import QtGraphicalEffects 1.0

import Entity.Remote 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: cardRemote
    width: parent.width
    height: parent.height
    color: colorDark

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: buttonHandler
        enabled: parent.state == "open" ? true : false

        onButtonPress: {
            switch (button) {
            case "volume up":
                if (obj.isSupported(Remote.F_VOLUME_UP)) {
                    console.debug("REMOTE VOLUME UP");
                    obj.volumeUp();
                }
                break;
            case "volume down":
                if (obj.isSupported(Remote.F_VOLUME_DOWN)) {
                    console.debug("REMOTE VOLUME DOWN");
                    obj.volumeDown();
                }
                break;
            case "channel up":
                if (obj.isSupported(Remote.F_CHANNEL_UP)) {
                    console.debug("REMOTE CHANNEL UP");
                    obj.channelUp();
                }
                break;
            case "channel down":
                if (obj.isSupported(Remote.F_CHANNEL_DOWN)) {
                    console.debug("REMOTE CHANNEL DOWN");
                    obj.channelDown();
                }
                break;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Item {
        width: parent.width
        height: 125
        anchors.top: parent.top

        Text {
            id: title
            color: colorText
            text: obj.friendly_name
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            width: parent.width-232
            anchors.left: parent.left
            anchors.leftMargin: 106
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            color: colorText
            text: icon.text
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 100 }
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
        }

    }

    Item {
        id: contentGroup0
        width: parent.width-40
        height: parent.height
        anchors.top: parent.top
        anchors.topMargin: 120
        anchors.horizontalCenter: parent.horizontalCenter
        opacity: group1.state == "open" || group2.state == "open" ? 0.1 : 1
        enabled: group1.state == "open" || group2.state == "open" ? 0.1 : 1

        Item {
            width: 80
            height: 60

            Rectangle {
                width: 80
                height: 60
                radius: width/2
                color: colorRed
            }

            Text {
                color: colorText
                text: "O"
                verticalAlignment: Text.AlignVCenter
                anchors.centerIn: parent
                font.family: "Open Semibold"
                font.weight: Font.Normal
                font.pixelSize: 27
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    obj.powerToggle();
                }
            }
        }
    }

    Group {
        id: group1
        height: 580
        offset: 80

        onStateChanged: {
            if (state == "closed") {
                group2.state = "closed";
            }
        }

        Item {
            id: contentGroup1
            width: parent.width
            height: parent.height
            opacity: parent.state == "open" ? 1 : 0.3
            enabled: parent.state == "open" ? true : false

            Grid {
                anchors.top: parent.top
                anchors.topMargin: 60
                anchors.horizontalCenter: parent.horizontalCenter
                columns: 3
                columnSpacing: 50
                rowSpacing: 30

                Repeater {
                    model: 10

                    Item {
                        width: 80
                        height: 60

                        Rectangle {
                            width: 80
                            height: 60
                            radius: width/2
                            color: colorBackgroundTransparent
                            border.color: colorText
                            border.width: 2
                            opacity: 0.2
                        }

                        Text {
                            color: colorText
                            text: index == 9 ? 0 : index + 1
                            verticalAlignment: Text.AlignVCenter
                            anchors.centerIn: parent
                            font.family: "Open Semibold"
                            font.weight: Font.Normal
                            font.pixelSize: 27
                        }
                    }
                }
            }
        }
    }

    Group {
        id: group2
        //        offset: 160
        height: 500

        onStateChanged: {
            if (state == "open") {
                group1.state = "move";
            } else {
                group1.state = "open";
            }
        }

        Item {
            id: contentGroup2
            width: parent.width
            height: parent.height
            opacity: parent.state == "open" ? 1 : 0.3
            enabled: parent.state == "open" ? true : false

            Grid {
                anchors.top: parent.top
                anchors.topMargin: 60
                anchors.horizontalCenter: parent.horizontalCenter
                columns: 4
                spacing: 60

                Rectangle {
                    width: 40
                    height: width
                    radius: width/2
                    color: "#EA003C"
                }

                Rectangle {
                    width: 40
                    height: width
                    radius: width/2
                    color: "#91BF4C"
                }

                Rectangle {
                    width: 40
                    height: width
                    radius: width/2
                    color: "#D7B435"
                }

                Rectangle {
                    width: 40
                    height: width
                    radius: width/2
                    color: "#284CC5"
                }
            }
        }
    }

    Rectangle {
        width: parent.width
        height: 80
        color: colorHighlight2
        radius: cornerRadius
        anchors.bottom: parent.bottom
    }
}
