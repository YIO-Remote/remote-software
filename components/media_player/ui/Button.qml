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
import QtGraphicalEffects 1.0
import Style 1.0

import "qrc:/scripts/helper.js" as JSHelper
import "qrc:/components" as Comp
import "qrc:/basic_ui" as BasicUI

import Entity.MediaPlayer 1.0
import MediaPlayerUtils 1.0

Comp.ButtonBase {
    id: mediaplayerButton
    icon: Style.icons.music
    cardLoader.source: "qrc:/components/media_player/ui/Card.qml"

    // override default settings
    title.anchors.verticalCenterOffset: obj.source === "" ? 0 : -15
    button.enabled: obj.isSupported(MediaPlayer.F_TURN_ON) ? true : false

    // include mediaplayer utils
    MediaPlayerUtils {
        id: mediaplayerUtils
    }

    ListModel {
        id: recentSearches

        Component.onCompleted: {
            obj.recentSearches = recentSearches;
        }
    }


    // additional UI elements
    Text {
        id: info
        color: Style.colorText
        opacity: 0.5
        text: obj.source
        elide: Text.ElideRight
        wrapMode: Text.WordWrap
        width: title.width
        anchors.left: parent.left
        anchors.leftMargin: title.x
        anchors.top: parent.top
        anchors.topMargin: title.y + title.height
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 20
        lineHeight: 1
        visible: mediaplayerButton.state == "closed" ? true : false
    }

    closeButtonMouseArea.onClicked: {
        inputPanel.active = false
    }

    // album art
    property string m_image: obj.mediaImage
    onM_imageChanged: {
        mediaplayerUtils.imageURL = obj.mediaImage
    }

    BasicUI.CustomImageLoader {
        id: image
        visible: mediaplayerButton.state == "closed" ? true : false
        width: 80
        height: 80
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        url: mediaplayerUtils.smallImage == "" ? "" : mediaplayerUtils.smallImage

        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: Item {
                width: image.width
                height: image.height
                Rectangle {
                    anchors.fill: parent
                    radius: Style.cornerRadius/2
                }
            }
        }
    }
}
