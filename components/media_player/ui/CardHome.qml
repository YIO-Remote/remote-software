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

import QtQuick 2.0

import Haptic 1.0
import Entity.MediaPlayer 1.0

import "qrc:/basic_ui" as BasicUI

Item {
    width: parent.width
    height: parent.height

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
            anchors.verticalCenterOffset: obj.source === "" ? 0 : -15
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: info
            color: colorText
            opacity: 0.5
            text: obj.source
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: title.width
            anchors.left: title.left
            anchors.top: title.bottom
            anchors.topMargin: -2
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
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

    BasicUI.CustomImageLoader {
        id: albumArt
        width: 280
        height: 280
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 118
        url: mediaplayerUtils.image === "" ? "qrc:/images/mini-music-player/no_image.png" : mediaplayerUtils.image
    }

    Text {
        id: songTitle
        color: colorText
        text: obj.mediaTitle
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
        width: parent.width-80
        font.family: "Open Sans"
        font.weight: Font.Bold
        font.styleName: "Bold"
        font.pixelSize: 30
        lineHeight: 1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: albumArt.bottom
        anchors.topMargin: 20
    }

    Text {
        id: artist
        color: colorText
        text: obj.mediaArtist
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
        width: parent.width-80
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
        anchors.top: songTitle.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Rectangle {
        id: progressBar
        visible: obj.isSupported(MediaPlayer.F_MEDIA_POSITION)
        width: parent.width-80
        height: 4
        color: "#000000"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: artist.bottom
        anchors.topMargin: 20

        Rectangle {
            width: parent.width*(obj.mediaProgress/obj.mediaDuration)
            height: parent.height
            color: colorLine

            Behavior on width {
                NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
            }
        }
    }

    Item {
        id: prevButton
        width: 120
        height: 120

        anchors {
            right: playButton.left
            rightMargin: 30
            verticalCenter: playButton.verticalCenter
        }

        Text {
            color: colorText
            text: "\uE909"
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
//            enabled: card.state === "open"

            onClicked: {
                Haptic.playEffect("click");
                obj.previous();
            }
        }
    }

    Item {
        id: playButton
        width: 120
        height: 120

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 80
        }

        Text {
            color: colorText
            text: "\uE905"
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
            opacity: obj.state === 3 ? 1 : 0

            Behavior on opacity {
                NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
            }
        }

        Text {
            color: colorText
            text: "\uE906"
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
            opacity: obj.state === 3 ? 0 : 1

            Behavior on opacity {
                NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
            }
        }

        MouseArea {
            anchors.fill: parent
//            enabled: card.state === "open"

            onClicked: {
                Haptic.playEffect("click");
                if (obj.state === 3) {
                    obj.pause();
                } else {
                    obj.play();
                }
            }
        }
    }

    Item {
        id: nextButton
        width: 120
        height: 120

        anchors {
            left: playButton.right
            leftMargin: 30
            verticalCenter: playButton.verticalCenter
        }

        Text {
            color: colorText
            text: "\uE904"
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
//            enabled: card.state === "open"

            onClicked: {
                Haptic.playEffect("click");
                obj.next();
            }
        }
    }
}
