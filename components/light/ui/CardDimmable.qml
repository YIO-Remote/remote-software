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
import Style 1.0

import Haptic 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: card
    width: parent.width; height: parent.height
    color: Style.colorDark
    radius: Style.cornerRadius

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: card

        onUpdateBrightness: {
            percentageBG.height = parent.height*brightness/100;
            percentageBG2.height = parent.height*brightness/100;
            percentage.text = brightness;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    state: "closed"

    states: [
        State {
            name: "closed"
            PropertyChanges { target: percentage; anchors.topMargin: 200; opacity: 0 }
            PropertyChanges { target: title; opacity: 0 }
            PropertyChanges { target: percentageBG2; opacity: 0 }
            PropertyChanges { target: percentageBG; opacity: 0 }
            PropertyChanges { target: buttonTurnOn; anchors.bottomMargin: -100; opacity: 0 }
        },
        State {
            name: "open"
            PropertyChanges { target: percentage; anchors.topMargin: 100; opacity: 1 }
            PropertyChanges { target: title; opacity: 1 }
            PropertyChanges { target: percentageBG2; opacity: 1 }
            PropertyChanges { target: percentageBG; opacity: 1 }
            PropertyChanges { target: buttonTurnOn; anchors.bottomMargin: 70; opacity: 1 }
        }
    ]

    transitions: [
        Transition {
            to: "closed"
            ParallelAnimation {
                PropertyAnimation { target: percentage; properties: "anchors.topMargin, opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: title; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: percentageBG2; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: percentageBG; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: buttonTurnOn; properties: "anchors.bottomMargin, opacity"; easing.type: Easing.OutExpo; duration: 300 }
            }
        },
        Transition {
            to: "open"
            ParallelAnimation {
                PropertyAnimation { target: percentageBG2; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: percentageBG; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: percentage; properties: "anchors.topMargin, opacity"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 400 }
                PropertyAnimation { target: buttonTurnOn; properties: "anchors.bottomMargin, opacity"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 400 }
                SequentialAnimation {
                    PauseAnimation { duration: 100 }
                    PropertyAnimation { target: title; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                }
            }
        }
    ]

    Component.onCompleted: {
        card.state = "open";
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property int brightness: obj.brightness

    onBrightnessChanged: {
        updateBrightness()
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SIGNALS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    signal updateBrightness()


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    MouseArea {
        id: dragger
        anchors.fill: parent
        drag { target: draggerTarget; axis: Drag.YAxis; minimumY: 0; maximumY: parent.height-10 }

        property int percent

        onPositionChanged: {
            Haptic.playEffect(Haptic.Bump);
            dragger.percent = Math.round((parent.height - 10 - mouse.y)/(parent.height-10)*100);
            if (dragger.percent < 0) dragger.percent = 0;
            if (dragger.percent > 100) dragger.percent = 100;
            if (dragger.percent > brightness) {
                percentageBG2.height = parent.height*dragger.percent/100
            } else {
                percentageBG.height = parent.height*dragger.percent/100
            }
            percentage.text = dragger.percent;
        }

        onReleased: {
            obj.setBrightness(dragger.percent);
        }
    }

    Rectangle {
        id: draggerTarget
        width: parent.width; height: 30
        color: "#00000000"
        y: parent.height - percentageBG.height
    }

    Rectangle {
        id: percentageBG2
        color: Style.colorMedium
        width: parent.width; height: 0
        radius: Style.cornerRadius
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }

        Behavior on height {
            PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
        }
    }

    Rectangle {
        id: percentageBG
        color: Style.colorHighlight2
        width: parent.width; height: parent.height*obj.brightness/100
        radius: Style.cornerRadius
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }

        Behavior on height {
            PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
        }
    }

    Text {
        id: icon
        color: Style.colorText
        text: Style.icons.light
        renderType: Text.NativeRendering
        width: 85; height: 85
        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
        font { family: "icons"; pixelSize: 100 }
        anchors { top: parent.top; topMargin: 20; left: parent.left; leftMargin: 20 }
    }

    Text {
        id: percentage
        color: Style.colorText
        text: obj.brightness
        horizontalAlignment: Text.AlignLeft
        anchors { top: parent.top; topMargin: 100; left: parent.left; leftMargin: 30 }
        font { family: "Open Sans Light"; pixelSize: 180 }
    }

    Text {
        color: Style.colorText
        opacity: percentage.opacity
        text: "%"
        anchors { left: percentage.right; bottom: percentage.bottom; bottomMargin: 30 }
        font { family: "Open Sans Light"; pixelSize: 100 }
    }

    Text {
        id: title
        color: Style.colorText
        text: obj.friendly_name
        wrapMode: Text.WordWrap
        width: parent.width-60
        anchors { top: percentage.bottom; topMargin: -40; left: parent.left; leftMargin: 30 }
        font { family: "Open Sans Regular"; pixelSize: 60 }
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
        font { family: "Open Sans Regular"; pixelSize: 24 }
    }

    BasicUI.CustomButton {
        id: buttonTurnOn
        anchors { left:parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
        color: Style.colorText
        buttonTextColor: Style.colorBackground
        buttonText: obj.state ? qsTr("Turn off") + translateHandler.emptyString : qsTr("Turn on") + translateHandler.emptyString

        mouseArea.onClicked: {
            Haptic.playEffect(Haptic.Click);
            obj.toggle();
        }
    }
}
