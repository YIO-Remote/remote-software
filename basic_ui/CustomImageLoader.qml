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

Item {
    id: imageLoader
    property string url: ""
    property string prevUrl: ""

    function startLoader() {
            loader.opacity = 1;
    }

    function stopLoader() {
        loader.opacity = 0;
    }

    onUrlChanged: {
        if (url != prevUrl) {
            image2.opacity = 0;
            image2.source = url;
            prevUrl = url;
        }
    }

    Image {
        id: image1
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        asynchronous: false
        cache: false

        onStatusChanged: {
            if (image1.status == Image.Ready) {
                image2.opacity = 0;
            }
        }
    }

    Image {
        id: image2
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        asynchronous: false
        opacity: 0
        cache: false

        Behavior on opacity {
            NumberAnimation { duration: 800; easing.type: Easing.OutExpo }
        }

        onStatusChanged: {
            if (image2.status == Image.Ready) {
                image2.opacity = 1;
            }
        }

        onOpacityChanged: {
            if (image2.opacity == 1) {
                image1.source = url;
            }
        }
    }

    Item {
        id: loader
        anchors.fill: parent
        opacity: 0
        visible: opacity != 0 ? true : false

        Behavior on opacity {
            NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
        }

        Image {
            asynchronous: true
            width: 26; height: 26
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/statusbar/statusbar-loader.png"

            RotationAnimator on rotation {
                id: loadingIconAnim
                running: loader.visible
                loops: Animation.Infinite
                from: 0; to: 360
                duration: 2000
            }
        }
    }
}
