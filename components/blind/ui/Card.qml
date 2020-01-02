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
import QtGraphicalEffects 1.0
import Style 1.0

import Haptic 1.0
import Entity.Blind 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: blindAdjust
    width: parent.width
    height: parent.height
    color: Style.colorDark

    // blind graphics draggable element

    property int position: obj.position

    property var percent
    property bool userMove: false

    Component.onCompleted: {
        percent = obj.position
    }

    Item {
        width: parent.width
        height: parent.height
        anchors.centerIn: parent

        Column {
            anchors.top: parent.top

            spacing: 10

            Repeater {
                id: repeater
                model: userMove ? Math.round(percent*36/100) : Math.round(obj.position*36/100)

                delegate: Rectangle {
                    width: blindAdjust.width
                    height: 10
                    color: Style.colorHighlight2
                }
            }
        }

        Rectangle {
            id: dragger_target
            width: parent.width
            height: parent.height
            color: Style.colorBackgroundTransparent
            y: userMove ? Math.round(height*percent/100) : Math.round(obj.position/100)
        }

        MouseArea {
            id: dragger
            anchors.fill: parent
            drag.target: dragger_target
            drag.axis: Drag.YAxis
            drag.minimumY: 0
            drag.maximumY: dragger_target.height

            onPositionChanged: {
                Haptic.playEffect("bump");
                percent = Math.round(mouse.y/parent.height*100)
                if (percent < 0) percent = 0
                if (percent > 100) percent = 100
            }

            onPressed: {
                userMove = true;
            }

            onReleased: {
                obj.setPosition(percent);
            }
        }
    }

    onPositionChanged: {
        if (userMove && position == percent) {
            userMove = false;
        } else {
            percent = position;
        }
    }

    Text {
        id: icon
        color: Style.colorText
        text: Style.icons.blind
        renderType: Text.NativeRendering
        width: 85
        height: 85
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font {family: "icons"; pixelSize: 100 }
        anchors {top: parent.top; topMargin: 20; left: parent.left; leftMargin: 20}
    }

    Text {
        id: percentage
        color: Style.colorText
        text: userMove ? percent : obj.position
        horizontalAlignment: Text.AlignLeft
        anchors { top: parent.top; topMargin: 100; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Light"; pixelSize: 180 }
    }

    Text {
        color: Style.colorText
        text: "%"
        anchors { left: percentage.right; bottom: percentage.bottom; bottomMargin: 30 }
        font {family: "Open Sans Light"; pixelSize: 100 }
    }

    Text {
        id: title
        color: Style.colorText
        text: obj.friendly_name
        wrapMode: Text.WordWrap
        width: parent.width-60
        anchors { top: percentage.bottom; topMargin: -40; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Regular"; pixelSize: 60 }
        lineHeight: 0.9
    }

    Text {
        id: areaText
        color: Style.colorText
        opacity: 0.5
        text: obj.area
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
        width: parent.width-60
        anchors { top: title.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Regular"; pixelSize: 24 }
    }

    BasicUI.CustomButton {
        anchors { left: parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
        color: Style.colorText
        buttonText: "   "
        visible: obj.isSupported(Blind.F_CLOSE)

        mouseArea.onClicked: {
            Haptic.playEffect("click");
            obj.close()
        }

        Image {
            asynchronous: true
            anchors.centerIn: parent
            source: "qrc:/components/blind/images/down-arrow.png"
        }
    }

    BasicUI.CustomButton {
        anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom; bottomMargin: 70 }
        color: Style.colorText
        buttonText: "   "
        visible: obj.isSupported(Blind.F_STOP)
        mouseArea.onClicked: {
            Haptic.playEffect("click");
            obj.stop()
        }

        Image {
            asynchronous: true
            anchors.centerIn: parent
            source: "qrc:/components/blind/images/stop.png"
        }
    }

    BasicUI.CustomButton {
        anchors { right: parent.right; rightMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
        color: Style.colorText
        buttonText: "   "
        visible: obj.isSupported(Blind.F_OPEN)

        mouseArea.onClicked: {
            Haptic.playEffect("click");
            obj.open()
        }

        Image {
            asynchronous: true
            anchors.centerIn: parent
            source: "qrc:/components/blind/images/up-arrow.png"
        }
    }

    Text {
        color: Style.colorText
        text: Style.icons.close
        renderType: Text.NativeRendering
        width: 70
        height: 70
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font {family: "icons"; pixelSize: 80 }
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 20

        MouseArea {
            width: parent.width + 20
            height: parent.height + 20
            anchors.centerIn: parent

            onClicked: {
                Haptic.playEffect("click");
                blindButton.state = "closed"
            }
        }
    }
}
