/******************************************************************************
 *
 * Copyright (C) 2020 Marton Borzak <hello@martonborzak.com>
 * Copyright (C) 2020-2021 Nikolas Slottke <nikoslottke@gmail.com>
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
import QtQuick.Layouts 1.3
import Style 1.0

import Haptic 1.0
import Entity.Remote 1.0

Item {
    width: parent.width - 60
    anchors.horizontalCenter: parent.horizontalCenter

    ColumnLayout {
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
        width: parent.width - 60
        spacing: 32

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            IconButton {
                visible: obj.isSupported(Remote.F_RECORD)
                icon.text: Style.icon.record
                mouseArea.onClicked: { obj.record(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_PLAY)
                icon.text: Style.icon.play
                mouseArea.onClicked: { obj.play(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_STOP)
                icon.text: Style.icon.stop
                mouseArea.onClicked: { obj.stop(); }
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            IconButton {
                visible: obj.isSupported(Remote.F_BACKWARD)
                icon.text: Style.icon.rewind
                mouseArea.onClicked: { obj.backward(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_PAUSE)
                icon.text: Style.icon.pause
                mouseArea.onClicked: { obj.pause(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_FORWARD)
                icon.text: Style.icon.fast_forward
                mouseArea.onClicked: { obj.forward(); }
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            IconButton {
                visible: obj.isSupported(Remote.F_PREVIOUS)
                icon.text: Style.icon.prev
                mouseArea.onClicked: { obj.previous(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_INFO)
                icon.text: Style.icon.info
                mouseArea.onClicked: { obj.info(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_NEXT)
                icon.text: Style.icon.next
                mouseArea.onClicked: { obj.next(); }
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            Button {
                visible: obj.isSupported(Remote.F_EXIT)
                title: qsTr("Exit") + translateHandler.emptyString
                mouseArea.onClicked: { obj.exit(); }
            }

            Button {
                visible: obj.isSupported(Remote.F_GUIDE)
                title: qsTr("Guide") + translateHandler.emptyString
                mouseArea.onClicked: { obj.guide(); }
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            Button {
                visible: obj.isSupported(Remote.F_BACK)
                title: qsTr("Back") + translateHandler.emptyString
                mouseArea.onClicked: { obj.back(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_HOME)
                icon.text: Style.icon.home
                mouseArea.onClicked: { obj.home(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_MENU)
                icon.text: Style.icon.menu
                mouseArea.onClicked: { obj.menu(); }
            }
        }
    }
}



