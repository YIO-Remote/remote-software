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
import Launcher 1.0

Rectangle {
    id: closingScreen
    anchors.fill: parent
    color: "#00000000"

    Launcher {
        id: launcher
    }

    SequentialAnimation {
        id: startAnim
        running: true

        PauseAnimation {duration: 1000}
        ParallelAnimation {
            ParallelAnimation {
                PropertyAnimation { target: left; properties: "width"; to: 240; easing.type: Easing.Linear; duration: 1000 }
                PropertyAnimation { target: right; properties: "width"; to: 240; easing.type: Easing.Linear; duration: 1000 }
            }
        }
        PropertyAction { target: closingScreen; property: "startAnimFinished"; value: true }
    }

    property bool startAnimFinished: false

    onStartAnimFinishedChanged: {
        if (startAnimFinished) {
            console.debug("NOW SHUTDOWN");
            launcher.launch("fbv -d 1 /bye.png")
            buttonHandler.interruptHandler.shutdown();
            launcher.launch("halt");
        }
    }

    Rectangle {
        id: left
        width: 0
        height: 800
        color: "#000000"

        anchors {
            left: parent.left
        }
    }

    Rectangle {
        id: right
        width: 0
        height: 800
        color: "#000000"

        anchors {
            right: parent.right
        }
    }

    MouseArea {
        anchors.fill: parent
    }
}


