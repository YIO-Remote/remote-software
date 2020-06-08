/******************************************************************************
 *
 * Copyright (C) 2020 Marton Borzak <hello@martonborzak.com>
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
import Entity.Remote 1.0

Item {
    width: parent.width - 60
    anchors.horizontalCenter: parent.horizontalCenter

    Flow {
        width: parent.width - 60
        anchors { top: parent.top; horizontalCenter: parent.horizontalCenter }
        spacing: 32

        Button {
            visible: obj.isSupported(Remote.F_PLAY)
            title: qsTr("Play") + translateHandler.emptyString
            mouseArea.onClicked: { obj.play(); }
        }

        Button {
            visible: obj.isSupported(Remote.F_PAUSE)
            title: qsTr("Pause") + translateHandler.emptyString
            mouseArea.onClicked: { obj.pause(); }
        }

        Button {
            visible: obj.isSupported(Remote.F_STOP)
            title: qsTr("Stop") + translateHandler.emptyString
            mouseArea.onClicked: { obj.stop(); }
        }

        Button {
            visible: obj.isSupported(Remote.F_PREVIOUS)
            title: qsTr("Prev") + translateHandler.emptyString
            mouseArea.onClicked: { obj.previous(); }
        }

        Button {
            visible: obj.isSupported(Remote.F_NEXT)
            title: qsTr("Next") + translateHandler.emptyString
            mouseArea.onClicked: { obj.next(); }
        }

        Button {
            visible: obj.isSupported(Remote.F_INFO)
            title: qsTr("Info") + translateHandler.emptyString
            mouseArea.onClicked: { obj.info(); }
        }

        Button {
            visible: obj.isSupported(Remote.F_BACKWARD)
            title: qsTr("BWD") + translateHandler.emptyString
            mouseArea.onClicked: { obj.backward(); }
        }

        Button {
            visible: obj.isSupported(Remote.F_FORWARD)
            title: qsTr("FWD") + translateHandler.emptyString
            mouseArea.onClicked: { obj.forward(); }
        }

        Button {
            visible: obj.isSupported(Remote.F_RECORD)
            title: qsTr("Rec") + translateHandler.emptyString
            mouseArea.onClicked: { obj.record(); }
        }

        Item { width: 118; height: 70 }

        Button {
            visible: obj.isSupported(Remote.F_EXIT)
            title: qsTr("Exit") + translateHandler.emptyString
            mouseArea.onClicked: { obj.exit(); }
        }

        Item { width: 118; height: 70 }

        Button {
            visible: obj.isSupported(Remote.F_BACK)
            title: qsTr("Back") + translateHandler.emptyString
            mouseArea.onClicked: { obj.back(); }
        }

        Button {
            visible: obj.isSupported(Remote.F_HOME)
            title: qsTr("Home") + translateHandler.emptyString
            mouseArea.onClicked: { obj.home(); }
        }

        Button {
            visible: obj.isSupported(Remote.F_MENU)
            title: qsTr("Menu") + translateHandler.emptyString
            mouseArea.onClicked: { obj.menu(); }
        }
    }
}



