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
                image.source: "qrc:/images/remote/fiber_manual_record-white-48dp.svg"
                mouseArea.onClicked: { obj.record(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_PLAY)
                image.source: "qrc:/images/remote/play_arrow-white-48dp.svg"
                mouseArea.onClicked: { obj.play(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_STOP)
                image.visible: true
                image.source: "qrc:/images/remote/stop-white-48dp.svg"
                mouseArea.onClicked: { obj.stop(); }
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            IconButton {
                visible: obj.isSupported(Remote.F_BACKWARD)
                image.source: "qrc:/images/remote/fast_rewind-white-48dp.svg"
                mouseArea.onClicked: { obj.backward(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_PAUSE)
                image.visible: true
                image.source: "qrc:/images/remote/pause-white-48dp.svg"
                mouseArea.onClicked: { obj.pause(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_FORWARD)
                image.source: "qrc:/images/remote/fast_forward-white-48dp.svg"
                mouseArea.onClicked: { obj.forward(); }
            }
        }

        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 32
            IconButton {
                visible: obj.isSupported(Remote.F_PREVIOUS)
                image.source: "qrc:/images/remote/skip_previous-white-48dp.svg"
                mouseArea.onClicked: { obj.previous(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_INFO)
                image.source: "qrc:/images/remote/info-white-48dp.svg"
                mouseArea.onClicked: { obj.info(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_NEXT)
                image.source: "qrc:/images/remote/skip_next-white-48dp.svg"
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
                image.source: "qrc:/images/remote/home-white-48dp.svg"
                mouseArea.onClicked: { obj.home(); }
            }

            IconButton {
                visible: obj.isSupported(Remote.F_MENU)
                image.source: "qrc:/images/remote/menu-white-48dp.svg"
                mouseArea.onClicked: { obj.menu(); }
            }
        }
    }
}



