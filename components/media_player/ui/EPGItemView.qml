/******************************************************************************
 *
 * Copyright (C) 2020 Michael Lörcher <MichaelLoercher@web.de>
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
    property var tvchannelModel


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UTILITIES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    MediaPlayerUtils {
        id: mediaplayerUtils
        imageURL: tvchannelModel.imageUrl
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
        contentHeight: 150 + image.height + title.height + artist.height + channelListView.height
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
            url: tvchannelModel.imageUrl === "" ? "qrc:/images/mini-music-player/no_image.png" : tvchannelModel.imageUrl
        }

        Text {
            color: Style.color.text
            text: Style.icon.play
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
                    obj.playMedia(tvchannelModel.id, tvchannelModel.type);
                }
            }
        }

        Text {
            id: time
            color: Style.color.text
            text: tvchannelModel.time
            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: parent.width-80
            font { family: "Open Sans Bold"; weight: Font.Bold; pixelSize: 30 }
            lineHeight: 1
            anchors { horizontalCenter: parent.horizontalCenter; top: image.bottom; topMargin: 20 }
        }

        Text {
            id: title
            color: Style.color.text
            text: tvchannelModel.title
            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: parent.width-80
            font { family: "Open Sans Bold"; weight: Font.Bold; pixelSize: 30 }
            lineHeight: 1
            anchors { horizontalCenter: parent.horizontalCenter; top: image.bottom; topMargin: 20 }
        }

        Text {
            id: subtitle
            color: Style.color.text
            text: tvchannelModel.subtitle
            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: parent.width-80
            font: Style.font.button
            anchors { top: title.bottom; horizontalCenter: parent.horizontalCenter }
        }
        ScrollView {
            id: view
            anchors { top: subtitle.bottom; leftMargin: 20}


            width: parent.width-10
            height:200
         TextArea {
            id: description
            color: Style.color.text
            text: tvchannelModel.description
            //horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
            //elide: Text.ElideRight
            wrapMode: Text.WordWrap
            width: view.width
            height:200
            font: Style.font.button

        }
        }

    }
}
