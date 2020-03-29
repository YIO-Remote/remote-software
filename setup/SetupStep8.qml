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
import Style 1.0
import Haptic 1.0
import WifiControl 1.0

Item {
    id: container
    width: 480; height: 800

    property var _swipeView
    property bool _currentItem: false

    Connections {
        target: bluetoothArea
        onDockFound: {
            console.debug("Dock found! Sending credentials...");
            // show dock page
            var msg = {};
            msg.ssid = wifi.wifiStatus.name;
            msg.password = wifiPassword;
            msg.remote_id = api.hostname;
            bluetoothArea.sendInfoToDock(JSON.stringify(msg));

            // mdns discovery of docks
            apiConnection.enabled = true;
            api.discoveredServices("_yio-dock-api._tcp");
            mdnsDiscoveryTimeout.start();
        }
    }

    Connections {
        id: apiConnection
        target: api
        enabled: false
        onServiceDiscovered: {
            // stop timeout timer
            mdnsDiscoveryTimeout.stop();
            bluetoothDiscoveryTimeout.stop();

            // TODO: need to check if the same dock was found as in the bluetooth setup
            console.debug("Dock API discovered: " + api.discoveredServices);

            // show success page
            _swipeView.dockSuccess = true;
            _swipeView.incrementCurrentIndex();
        }
    }

    Timer {
        id: bluetoothDiscoveryTimeout
        running: _currentItem
        interval: 20000
        repeat: false

        onTriggered: {
            _swipeView.dockSuccess = false;
            _swipeView.incrementCurrentIndex();
        }
    }

    Timer {
        id: mdnsDiscoveryTimeout
        running: false
        interval: 20000
        repeat: false

        onTriggered: {
            _swipeView.dockSuccess = false;
            _swipeView.incrementCurrentIndex();
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
            running: _currentItem
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 2000
        }
    }

    Text {
        color: Style.colorText
        text: qsTr("Setting up your YIO Dock") + translateHandler.emptyString
        anchors { top: yio_O.bottom; topMargin: 40; horizontalCenter: parent.horizontalCenter }
        font: Style.fonts.button
    }
}
