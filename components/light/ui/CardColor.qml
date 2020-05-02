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

import Haptic 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: card
    width: parent.width; height: parent.height
    color: Style.color.dark
    radius: Style.cornerRadius


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
            PropertyChanges { target: toggleButton; anchors.bottomMargin: -100; opacity: 0 }
            PropertyChanges { target: colorButton; anchors.bottomMargin: -100; opacity: 0 }
        },
        State {
            name: "open"
            PropertyChanges { target: percentage; anchors.topMargin: 100; opacity: 1 }
            PropertyChanges { target: title; opacity: 1 }
            PropertyChanges { target: percentageBG2; opacity: 1 }
            PropertyChanges { target: percentageBG; opacity: 1 }
            PropertyChanges { target: toggleButton; anchors.bottomMargin: 70; opacity: 1 }
            PropertyChanges { target: colorButton; anchors.bottomMargin: 70; opacity: 1 }
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
                PropertyAnimation { target: toggleButton; properties: "anchors.bottomMargin, opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: colorButton; properties: "anchors.bottomMargin, opacity"; easing.type: Easing.OutExpo; duration: 300 }
            }
        },
        Transition {
            to: "open"
            ParallelAnimation {
                PropertyAnimation { target: percentageBG2; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: percentageBG; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: percentage; properties: "anchors.topMargin, opacity"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 400 }
                PropertyAnimation { target: toggleButton; properties: "anchors.bottomMargin, opacity"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 400 }
                SequentialAnimation {
                    PauseAnimation { duration: 50 }
                    PropertyAnimation { target: colorButton; properties: "anchors.bottomMargin, opacity"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 400 }
                }
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
    Flipable {
        id: flipable
        width: parent.width; height: parent.height

        property bool flipped: false

        transform: Rotation {
            id: rotation
            origin.x: flipable.width/2
            origin.y: flipable.height/2
            axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
            angle: 0    // the default angle
        }

        states: State {
            name: "back"
            PropertyChanges { target: rotation; angle: 180 }
            when: flipable.flipped
        }

        transitions: Transition {
            NumberAnimation { target: rotation; property: "angle"; easing.type: Easing.OutExpo; duration: 500 }
        }

        front: Item {
            anchors.fill: parent

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

            Connections {
                target: card
                onUpdateBrightness: {
                    percentageBG.height = parent.height*brightness/100;
                    percentageBG2.height = parent.height*brightness/100;
                    percentage.text = brightness;
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
                color: Style.color.medium
                width: parent.width; height: 0
                radius: Style.cornerRadius
                anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }

                Behavior on height {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }

            Rectangle {
                id: percentageBG
                color: Style.color.highlight2
                width: parent.width; height: parent.height*brightness/100
                radius: Style.cornerRadius
                anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }

                Behavior on height {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }

            Text {
                id: icon
                color: Style.color.text
                text: Style.icon.light
                renderType: Text.NativeRendering
                width: 85; height: 85
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font {family: "icons"; pixelSize: 100 }
                anchors {top: parent.top; topMargin: 20; left: parent.left; leftMargin: 20}
            }

            Text {
                id: percentage
                color: Style.color.text
                text: brightness
                horizontalAlignment: Text.AlignLeft
                anchors { top: parent.top; topMargin: 100; left: parent.left; leftMargin: 30 }
                font {family: "Open Sans Light"; pixelSize: 180 }
            }

            Text {
                color: Style.color.text
                opacity: percentage.opacity
                text: "%"
                anchors { left: percentage.right; bottom: percentage.bottom; bottomMargin: 30 }
                font {family: "Open Sans Light"; pixelSize: 100 }
            }

            Text {
                id: title
                color: Style.color.text
                text: obj.friendly_name
                wrapMode: Text.WordWrap
                width: parent.width-60
                anchors { top: percentage.bottom; topMargin: -40; left: parent.left; leftMargin: 30 }
                font {family: "Open Sans Regular"; pixelSize: 60 }
                lineHeight: 0.9
            }

            Text {
                id: areaText
                color: Style.color.text
                opacity: 0.5
                text: obj.area
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                width: parent.width-60
                anchors { top: title.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
                font {family: "Open Sans Regular"; pixelSize: 24 }
            }

            BasicUI.CustomButton {
                id: toggleButton
                anchors { left:parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
                color: Style.color.text
                buttonTextColor: Style.color.background
                buttonText: obj.state ? qsTr("Turn off") + translateHandler.emptyString : qsTr("Turn on") + translateHandler.emptyString

                mouseArea.onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    obj.toggle();
                }
            }

            BasicUI.CustomButton {
                id: colorButton
                anchors { left:toggleButton.right; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
                color: Style.color.text
                buttonTextColor: Style.color.background
                buttonText: qsTr("Color") + translateHandler.emptyString

                mouseArea.onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    flipable.flipped = !flipable.flipped
                }
            }
        } // front item end

        back: Item {
            id: backItem
            anchors.fill: parent

            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource:
                    Rectangle {
                    id: opacityMask
                    width: backItem.width; height: backItem.height
                    radius: Style.cornerRadius

                    Behavior on radius {
                        NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
                    }
                }
            }

            Canvas {
                id: canvas
                width: parent.width; height: parent.height

                function getColor(x, y) {
                    var ctx = getContext("2d");
                    canvas.requestPaint();

                    var imageData = ctx.getImageData(x, y, 1, 1).data;

                    picker.x = x-picker.width/2;
                    picker.y = y-picker.height/2;
                    picker.color = Qt.rgba(imageData[0]/255, imageData[1]/255, imageData[2]/255, 1);
                }

                onPaint: {
                    var ctx = getContext("2d");

                    var gradient = ctx.createLinearGradient(0, 0, parent.width, 0);

                    // Create color gradient
                    gradient.addColorStop(0,    "rgba(255,   0,   0, 1)");
                    gradient.addColorStop(0.15, "rgba(255,   0, 255, 1)");
                    gradient.addColorStop(0.33, "rgba(0,     0, 255, 1)");
                    gradient.addColorStop(0.49, "rgba(0,   255, 255, 1)");
                    gradient.addColorStop(0.67, "rgba(0,   255,   0, 1)");
                    gradient.addColorStop(0.84, "rgba(255, 255,   0, 1)");
                    gradient.addColorStop(1,    "rgba(255,   0,   0, 1)");

                    ctx.fillStyle = gradient;
                    ctx.fillRect(0, 0, parent.width, parent.height);

                    gradient = ctx.createLinearGradient(0, 0, 0, parent.height)

                    gradient.addColorStop(1,   "rgba(255, 255, 255, 1)");
                    gradient.addColorStop(0.5, "rgba(255, 255, 255, 0)");
                    gradient.addColorStop(0.5, "rgba(0,     0,   0, 0)");
                    gradient.addColorStop(0,   "rgba(0,     0,   0, 1)");

                    ctx.fillStyle = gradient;
                    ctx.fillRect(0, 0, parent.width, parent.height);

                }

                MouseArea {
                    anchors.fill: parent

                    onPositionChanged: {
                        canvas.getColor(mouseX, mouseY);

                        icon_b.opacity = 0;
                        color_b.opacity = 0;
                        title_b.opacity = 0;
                        areaText_b.opacity = 0;
                        button_b.opacity = 0;
                        closeButton_b.opacity = 0;
                        closeButton_b.enabled = false;
                    }

                    onReleased: {
                        obj.setColor(picker.color);

                        icon_b.opacity = 1;
                        color_b.opacity = 1;
                        title_b.opacity = 1;
                        areaText_b.opacity = 1;
                        button_b.opacity = 1;
                        closeButton_b.opacity = 1;
                        closeButton_b.enabled = true;
                    }
                }
            }

            Text {
                id: icon_b
                color: Style.color.text
                text: Style.icon.light
                renderType: Text.NativeRendering
                width: 85; height: 85
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font { family: "icons"; pixelSize: 100 }
                anchors {top: parent.top; topMargin: 20; left: parent.left; leftMargin: 20}

                Behavior on opacity {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }

            Text {
                id: color_b
                color: Style.color.text
                text: "R: " + Math.round(picker.color.r*255) + "\nG: " + Math.round(picker.color.g*255) + "\nB: " + Math.round(picker.color.b*255)
                wrapMode: Text.WordWrap
                width: parent.width-60
                anchors { top: icon_b.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
                font { family: "Open Sans Regular"; pixelSize: 50 }
                lineHeight: 0.9

                Behavior on opacity {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }

            Text {
                id: title_b
                color: Style.color.text
                text: obj.friendly_name
                wrapMode: Text.WordWrap
                width: parent.width-60
                anchors { top: color_b.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
                font { family: "Open Sans SemiBold"; pixelSize: 60 }
                lineHeight: 0.9

                Behavior on opacity {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }

            Text {
                id: areaText_b
                color: Style.color.text
                opacity: 0.5
                text: obj.area
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                width: parent.width-60
                anchors { top: title_b.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
                font { family: "Open Sans Regular"; pixelSize: 24 }

                Behavior on opacity {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }

            Rectangle {
                id: picker
                width: 60; height: width
                border { color: Style.color.text; width: 4 }
                radius: width/2
            }

            BasicUI.CustomButton {
                id: button_b
                anchors { left:parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
                color: Style.color.text
                buttonTextColor: Style.color.background
                buttonText: "OK"

                mouseArea.onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    flipable.flipped = !flipable.flipped
                }

                Behavior on opacity {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }
        } // back Item end
    } // flipabble end
}
