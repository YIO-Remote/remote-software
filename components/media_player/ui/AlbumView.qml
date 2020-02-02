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

import QtQuick 2.12
import QtQuick.Controls 2.12
import Style 1.0

import Haptic 1.0
import MediaPlayerUtils 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: main
    width: parent.width; height: parent.height
    color: mediaplayerUtils.pixelColor
    radius: Style.cornerRadius

    Behavior on color {
        ColorAnimation { duration: 300 }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property var albumModel


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UTILITIES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    MediaPlayerUtils {
        id: mediaplayerUtils
        imageURL: albumModel.imageUrl
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias itemFlickable: itemFlickable

    Flickable {
        id: itemFlickable
        width: parent.width; height: parent.height-100
        maximumFlickVelocity: 6000
        flickDeceleration: 1000
        contentHeight: 150 + image.height + title.height + artist.height + trackListView.height
        boundsBehavior: Flickable.DragAndOvershootBounds
        flickableDirection: Flickable.VerticalFlick
        clip: true

        Behavior on contentY {
            PropertyAnimation {
                duration: 300
                easing.type: Easing.OutExpo
            }
        }

        ScrollBar.vertical: ScrollBar {
            opacity: 0.5
        }


        BasicUI.CustomImageLoader {
            id: image
            width: 280; height: 280
            anchors { horizontalCenter: parent.horizontalCenter; top: parent.top; topMargin: 100 }
            url: albumModel.imageUrl === "" ? "qrc:/images/mini-music-player/no_image.png" : albumModel.imageUrl
        }

        Text {
            color: Style.colorText
            text: Style.icons.play
            renderType: Text.NativeRendering
            width: 70; height: 70
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: image

            MouseArea {
                width: parent.width + 20; height: parent.height + 20
                anchors.centerIn: parent
                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    obj.playMedia(albumModel.id, albumModel.type);
                }
            }
        }

        Text {
            id: title
            color: Style.colorText
            text: albumModel.title
            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: parent.width-80
            font { family: "Open Sans Bold"; weight: Font.Bold; pixelSize: 30 }
            lineHeight: 1
            anchors { horizontalCenter: parent.horizontalCenter; top: image.bottom; topMargin: 20 }
        }

        Text {
            id: artist
            color: Style.colorText
            text: albumModel.subtitle
            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: parent.width-80
            font: Style.buttonFont
            anchors { top: title.bottom; horizontalCenter: parent.horizontalCenter }
        }

        ListView {
            id: trackListView
            width: parent.width-60; height: childrenRect.height
            spacing: 20
            interactive: false
            anchors { top: artist.bottom; topMargin: 40; horizontalCenter: parent.horizontalCenter }

            model: albumModel.model
            delegate: trackThumbnail
        }

        Component {
            id: trackThumbnail

            Item {
                width: parent.width; height: 80

                Text {
                    id: trackNumber
                    text: index+1
                    color: Style.colorText
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                    font { family: "Open Sans Regular"; pixelSize: 25 }
                    lineHeight: 1
                }

                Text {
                    id: albumTitleText
                    text: item_title
                    elide: Text.ElideRight
                    width: parent.width-100
                    wrapMode: Text.NoWrap
                    color: Style.colorText
                    anchors { left: parent.left; leftMargin: 45; top: parent.top }
                    font { family: "Open Sans Regular"; pixelSize: 25 }
                    lineHeight: 1
                }

                Text {
                    id: albumSubTitleText
                    text: item_subtitle
                    elide: Text.ElideRight
                    width: albumTitleText.width
                    wrapMode: Text.NoWrap
                    color: Style.colorText
                    opacity: 0.6
                    anchors { left: albumTitleText.left; top: albumTitleText.bottom; topMargin: 5 }
                    font { family: "Open Sans Regular"; pixelSize: 20 }
                    lineHeight: 1
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        Haptic.playEffect(Haptic.Click);
                        obj.playMedia(item_key, item_type);
                    }
                }

                BasicUI.ContextMenuIcon {
                    colorBg: mediaplayerUtils.pixelColor
                    anchors { right: parent.right; verticalCenter: parent.verticalCenter }

                    mouseArea.onClicked: {
                        Haptic.playEffect(Haptic.Click);
                        contextMenuLoader.setSource("qrc:/basic_ui/ContextMenu.qml", { "width": main.width, "id": item_key, "type": item_type, "list": item_commands })
                    }
                }
            }
        }
    }
}
