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
import QtGraphicalEffects 1.0

import Style 1.0

import Battery 1.0

import "qrc:/scripts/helper.js" as JSHelper

Rectangle {
    id: chargingScreen
    width: parent.width
    height: parent.height
    color: Style.colorBackground

    state: "hidden"

    states: [
        State { name: "visible"; when: Battery.isCharging; PropertyChanges {target: chargingScreen; opacity: 1; visible: true} },
        State { name: "hidden"; when: !Battery.isCharging; PropertyChanges {target: chargingScreen; opacity: 0; visible: false} }
    ]
    transitions: [
        Transition {to: "hidden"; PropertyAnimation { target: chargingScreen; properties: "opacity, visible"; easing.type: Easing.InExpo; duration: 1000 }},
        Transition {to: "visible"; SequentialAnimation {
                PropertyAnimation { target: chargingScreen; properties: "visible"; duration: 1 }
                PauseAnimation { duration: 1000 }
                PropertyAnimation { target: chargingScreen; properties: "opacity"; easing.type: Easing.OutExpo; duration: 1000 }
            }}
    ]

    onStateChanged: {
        if (state == "visible") {
            resetClock.start();
            showClock.start();
        } else {
            resetClock.start();
        }
    }

    property alias resetClock: resetClock

    ParallelAnimation {
        id: resetClock
        objectName: "resetClock"
        running: false

        PropertyAnimation { target: batteryIcon; properties: "x"; to: (parent.width-implicitWidth)/2; duration: 1 }
        PropertyAnimation { target: batteryIcon; properties: "y"; to: (parent.height-implicitHeight)/2; duration: 1 }
        PropertyAnimation { target: batteryIcon; properties: "scale"; to: 1; duration: 1 }
        PropertyAnimation { target: chargeText; properties: "opacity"; to: 1; duration: 1 }
        PropertyAnimation { target: timeText; properties: "anchors.bottomMargin"; to: -400; duration: 1 }
    }

    property alias showClock: showClock

    SequentialAnimation {
        id: showClock
        objectName: "showClock"
        running: false

        PauseAnimation {duration: 3000}
        ParallelAnimation {
            PropertyAnimation { target: batteryIcon; properties: "x"; to: 100; easing.type: Easing.OutExpo; duration: 600 }
            PropertyAnimation { target: batteryIcon; properties: "y"; to: 150; easing.type: Easing.OutExpo; duration: 600 }
            PropertyAnimation { target: batteryIcon; properties: "scale"; to: 0.5; easing.type: Easing.OutExpo; duration: 600 }
            PropertyAnimation { target: chargeText; properties: "opacity"; to: 0; easing.type: Easing.OutExpo; duration: 600 }
            PropertyAnimation { target: timeText; properties: "anchors.bottomMargin"; to: 40; easing.type: Easing.OutExpo; duration: 600 }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // BATTERY ICON
    //////////////////////////////////////////////////////////////////////////////////////////////////
    Item {
        id: batteryIcon
        //        anchors.verticalCenter: parent.verticalCenter
        //        anchors.horizontalCenter: parent.horizontalCenter
        x: (parent.width-implicitWidth)/2
        y: (parent.height-implicitHeight)/2

        // battery notch
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: batteryFrame.top
            anchors.bottomMargin: -12
            width: 83
            height: 56
            color: "#00000000"
            radius: 16
            border.width: 6
            border.color: Style.colorLine
        }

        // battery frame
        Rectangle {
            id: batteryFrame
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: 151
            height: 218
            color: Style.colorBackground
            border.width: 6
            border.color: Style.colorLine
            radius: 32

            // battery fill
            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 23
                width: 106
                height: Battery.level/100*172
                color: Style.colorGreen
                radius: 8
            }

            Image {
                asynchronous: true
                id: icon
                width: 44
                height: 94
                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/chargingscreen/icon-battery-lightning.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                ColorOverlay {
                    visible: !Style.darkMode
                    anchors.fill: parent
                    source: parent
                    color: Style.colorText
                }
            }
        }
    }

    Text {
        id: chargeText
        color: Style.colorText
        text: Math.round(Battery.level) + qsTr("% Charged") + translateHandler.emptyString
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 230
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 40
        horizontalAlignment: Text.AlignHCenter
        font.family: "Open Sans Regular"
        font.weight: Font.Normal
        font.pixelSize: 32
        lineHeight: 0.8
    }

    Text {
        id: timeText
        color: Style.colorText
        text: loader_main.item ? loader_main.item.statusBar.timeText.text : ""
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -400
        font.family: "Open Sans Light"
        font.pixelSize: 150
        lineHeight: 1
    }
}
