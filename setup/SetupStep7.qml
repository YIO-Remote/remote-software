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

import "qrc:/basic_ui" as BasicUI

Item {
    id: container
    width: 480; height: 800

    Image {
        source: "qrc:/images/settings/setup-dock-led.png"
        anchors { bottom: dockImage.bottom; bottomMargin: -20; horizontalCenter: dockImage.horizontalCenter }

        SequentialAnimation on opacity {
            loops: Animation.Infinite

            NumberAnimation { duration: 1; from: 1; to: 0 }
            PauseAnimation { duration: 800 }
            NumberAnimation { duration: 1; from: 0; to: 1 }
            PauseAnimation { duration: 800 }
        }
    }

    Image {
        id: dockImage
        source: "qrc:/images/settings/setup-dock.png"
        anchors { top: parent.top; topMargin: 100; horizontalCenter: parent.horizontalCenter }
    }

    Text {
        id: smalltext
        color: Style.colorText
        opacity: 0.5
        text: qsTr("Connect the YIO Dock to\na power source and wait until it starts blinking.\n\n To share the Wi-Fi network credentials with the dock, tap next.") + translateHandler.emptyString
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        width: parent.width - 100
        anchors { top: dockImage.bottom; topMargin: 80; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 27 }
        lineHeight: 1
    }

    Item {
        width: childrenRect.width; height: buttonNext.height
        anchors { bottom: parent.bottom; bottomMargin: 60; horizontalCenter: parent.horizontalCenter }

        BasicUI.CustomButton {
            id: buttonNext
            buttonText: qsTr("Next") + translateHandler.emptyString
            anchors { verticalCenter: parent.verticalCenter; }
            mouseArea.onClicked: {
                Haptic.playEffect(Haptic.Click);
                container.parent.parent.parent.dockSkip = false;
                container.parent.parent.parent.incrementCurrentIndex();
                bluetoothArea.lookForDock();
            }
        }

        BasicUI.CustomButton {
            buttonText: qsTr("Skip") + translateHandler.emptyString
            anchors { left: buttonNext.right; leftMargin: 30; verticalCenter: parent.verticalCenter; }
            mouseArea.onClicked: {
                Haptic.playEffect(Haptic.Click);
                container.parent.parent.parent.dockSkip = true;
                container.parent.parent.parent.incrementCurrentIndex();
                container.parent.parent.parent.incrementCurrentIndex();
            }
        }
    }
}
