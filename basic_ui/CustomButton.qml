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
import QtQuick.Controls 2.5

Rectangle {

    property alias mouseArea: mouseArea
    property alias buttonText: buttonText.text
    property alias buttonTextColor: buttonText.color

    id: button
    width: buttonText.implicitWidth+60
    height: 60
    color: colorBackgroundTransparent
    radius: 40
    border.color: colorLine

    states: State {
        name: "pressed"
        when: mouseArea.pressed == true
        PropertyChanges {
            target: button
            color: colorLine
            border.color: colorBackgroundTransparent
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
        color: colorText
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font { family: "Open Sans Regular"; pixelSize: 27 }
        lineHeight: 0.8
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            haptic.playEffect("click");
        }
    }
}
