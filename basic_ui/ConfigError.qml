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
import Launcher 1.0
import Haptic 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    anchors.fill: parent
    color: "#000000"

    Launcher {
        id: launcher
    }

    Text {
        id: titleText
        color: "#ffffff"
        width: parent.width - 80
        text: qsTr("Invalid configuration file") + translateHandler.emptyString
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors { verticalCenter: parent.verticalCenter; verticalCenterOffset: -50; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 27 }
        lineHeight: 1
    }

    Text {
        id: smallText
        color: "#ffffff"
        opacity: 0.5
        text: qsTr("Navigate your internet browser to: http://") + wifi.wifiStatus.ipAddress + translateHandler.emptyString + qsTr("\n and use the web configurator\nto fix the problem.")
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        width: parent.width - 80
        anchors { top: titleText.bottom; topMargin: 20; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 20 }
        lineHeight: 1
    }

    Rectangle {
        id: button
        width: buttonText.implicitWidth+60; height: 60
        color: "#000000"
        radius: 40
        border.color: "#ffffff"
        anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom; bottomMargin: 60 }

        states: State {
            name: "pressed"
            when: mouseArea.pressed
            PropertyChanges {
                target: button
                color: "#ffffff"
                border.color: "#000000"
            }
        }

        transitions: [
          Transition {
              from: ""; to: "pressed"; reversible: true
              PropertyAnimation { target: button
                                  properties: "color"; duration: 300 }
          }]

        Text {
            id: buttonText
            color: "#ffffff"
            text: qsTr("Reboot") + translateHandler.emptyString
            anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font { family: "Open Sans Regular"; pixelSize: 27 }
            lineHeight: 0.8
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            onClicked: {
                Haptic.playEffect(Haptic.Click);
                launcher.launch("fbv -d 1 $YIO_MEDIA_DIR/splash/bye.png")
                console.debug("now reboot");
                launcher.launch("reboot");
            }
        }
    }

    Component.onCompleted: {
        remoteConfigEnabled = true;
        webserver.startService();
    }
}
