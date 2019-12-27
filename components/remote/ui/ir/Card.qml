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

        onButtonPress: {
            switch (button) {
            case "volume up":
                if (obj.isSupported(Remote.F_VOLUME_UP)) {
                    obj.volumeUp();
                }
                break;
            case "volume down":
                if (obj.isSupported(Remote.F_VOLUME_DOWN)) {
                    obj.volumeDown();
                }
                break;
            case "channel up":
                if (obj.isSupported(Remote.F_CHANNEL_UP)) {
                    obj.channelUp();
                }
                break;
            case "channel down":
                if (obj.isSupported(Remote.F_CHANNEL_DOWN)) {
                    obj.channelDown();
                }
                break;
            case "dpad up":
                if (obj.isSupported(Remote.F_CURSOR_UP)) {
                    obj.cursorUp();
                }
                break;
            case "dpad down":
                if (obj.isSupported(Remote.F_CURSOR_DOWN)) {
                    obj.cursorDown();
                }
                break;
            case "dpad left":
                if (obj.isSupported(Remote.F_CURSOR_LEFT)) {
                    obj.cursorLeft();
                }
                break;
            case "dpad right":
                if (obj.isSupported(Remote.F_CURSOR_RIGHT)) {
                    obj.cursorRight();
                }
                break;
            case "dpad middle":
                if (obj.isSupported(Remote.F_CURSOR_OK)) {
                    obj.cursorOK();
                }
                break;
            case "top left":
                if (obj.isSupported(Remote.F_MUTE_TOGGLE)) {
                    obj.muteToggle();
                }
                break;
            case "bottom left":
                if (obj.isSupported(Remote.F_BACK)) {
                    obj.back();
                }
                break;
            case "bottom right":
                if (obj.isSupported(Remote.F_MENU)) {
                    obj.menu();
                }
                break;
            }

        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Item {
        id: topItem
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

    SwipeView {
        id: pagesSwipeView
        width: parent.width
        height: parent.height - topItem.height - tooltips.height - 50
        anchors.top: topItem.bottom
        anchors.topMargin: 0
        currentIndex: 0

        //buttons
        Item {
        }

        // channels
        Item {
            GridView {
                id: channelGridView
                width: parent.width-50
                height: parent.height
                anchors.horizontalCenter: parent.horizontalCenter
                cellWidth: width/3; cellHeight: cellWidth
                clip: true

                model: obj.channels
                delegate: Item {
                    width: channelGridView.cellWidth
                    height: width

                    Rectangle {
                        id: imageContainer
                        width: 80
                        height: width
                        anchors.centerIn: parent
                        radius: cornerRadius
                        color: colorDark

                        layer.enabled: true
                        layer.effect: OpacityMask {
                            maskSource: Item {
                                width: imageContainer.width
                                height: imageContainer.height
                                Rectangle {
                                    anchors.fill: parent
                                    radius: cornerRadius
                                }
                            }
                        }

                        Image {
                            anchors.fill: parent
                            fillMode: Image.PreserveAspectCrop
                            asynchronous: true
                            source: "file:/" + obj.channels[index].image
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                haptic.playEffect("click");
                                obj.channel(obj.channels[index].number);
                            }
                        }
                    }
                }
            }
        }
    }

    PageIndicator {
        id: indicator

        count: pagesSwipeView.count
        currentIndex: pagesSwipeView.currentIndex

        anchors.bottom: tooltips.top
        anchors.bottomMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter

        delegate: Rectangle {
            width: 8
            height: 8
            radius: height/2
            color: colorText
            opacity: index == pagesSwipeView.currentIndex ? 1 : 0.3
        }
    }

    Item {
        id: tooltips
        width: parent.width
        height: 80
        anchors.bottom: parent.bottom
    }
}
