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
import Style 1.0
import Haptic 1.0
import WifiControl 1.0

Item {
    id: container
    width: 480; height: 800

    Connections {
        target: wifi
        onWifiStatusChanged: {
            console.debug("Wifi status changed:" + wifi.wifiStatus);
            if (wifi.wifiStatus.connected) {
                loadingIconAnim.stop();
                container.parent.parent.incrementCurrentIndex();
            }
        }
    }

    Image {
        id: yio_O
        asynchronous: true
        width: 96; height: 96
        anchors { horizontalCenter: parent.horizontalCenter; top: parent.top; topMargin: 320 }
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/loading/O.png"

        RotationAnimator on rotation {
            id: loadingIconAnim
            running: true
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 2000
        }
    }

    Text {
        color: Style.colorText
        text: qsTr("Connecting") + translateHandler.emptyString
        anchors { top: yio_O.bottom; topMargin: 40; horizontalCenter: parent.horizontalCenter }
        font: Style.buttonFont
    }
}
