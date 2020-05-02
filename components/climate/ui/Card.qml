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
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import Style 1.0

import Haptic 1.0
import ButtonHandler 1.0

import Entity.Climate 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: card
    width: parent.width; height: parent.height
    color: Style.color.dark
    radius: Style.cornerRadius

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: ButtonHandler

        onButtonPressed: {
            switch (button) {
            case ButtonHandler.DPAD_UP:
                if (obj.isSupported(Climate.F_TARGET_TEMPERATURE)) {
                    var t = targetTemperature+0.5
                    targetTemperature = t;
                    obj.setTargetTemperature(targetTemperature);
                }
                break;
            case ButtonHandler.DPAD_DOWN:
                if (obj.isSupported(Climate.F_TARGET_TEMPERATURE)) {
                    t = targetTemperature-0.5
                    targetTemperature = t;
                    obj.setTargetTemperature(targetTemperature);
                }
                break;
            }

        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    state: "closed"

    states: [
        State {
            name: "moving"
            PropertyChanges {target: temperatureText; opacity: 0}
            PropertyChanges {target: titleText; opacity: 0}
            PropertyChanges {target: areaText; opacity: 0}
            PropertyChanges {target: turnOnButton; opacity: 0}
            PropertyChanges {target: modeButton; opacity: 0}
            PropertyChanges {target: dialListView; opacity: 1}
        },
        State {
            name: "notmoving"
            PropertyChanges {target: temperatureText; opacity: 1}
            PropertyChanges {target: titleText; opacity: 1}
            PropertyChanges {target: areaText; opacity: 1}
            PropertyChanges {target: turnOnButton; opacity: 1}
            PropertyChanges {target: modeButton; opacity: 1}
            PropertyChanges {target: dialListView; opacity: 0}
        },
        State {
            name: "closed"
            PropertyChanges { target: temperatureText; anchors.topMargin: 100; opacity: 0 }
            PropertyChanges { target: titleText; opacity: 0 }
            PropertyChanges { target: temperatureDial; opacity: 0 }
            PropertyChanges { target: modeButton; anchors.bottomMargin: -100; opacity: 0 }
            PropertyChanges { target: turnOnButton; anchors.bottomMargin: -100; opacity: 0 }
            PropertyChanges {target: dialListView; opacity: 0}
        },
        State {
            name: "open"
            PropertyChanges { target: temperatureText; anchors.topMargin: 0; opacity: 1 }
            PropertyChanges { target: titleText; opacity: 1 }
            PropertyChanges { target: temperatureDial; opacity: 1 }
            PropertyChanges { target: modeButton; anchors.bottomMargin: 70; opacity: 1 }
            PropertyChanges { target: turnOnButton; anchors.bottomMargin: 70; opacity: 1 }
            PropertyChanges {target: dialListView; opacity: 0}
        }
    ]

    transitions: [
        Transition {
            to: "moving"
            PropertyAnimation { target: temperatureText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: titleText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: areaText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: turnOnButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: modeButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: dialListView; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }

        },
        Transition {
            to: "notmoving"
            PropertyAnimation { target: temperatureText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: titleText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: areaText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: turnOnButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: modeButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: dialListView; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
        },
        Transition {
            to: "closed"
            ParallelAnimation {
                PropertyAnimation { target: temperatureText; properties: "anchors.topMargin, opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: titleText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: temperatureDial; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: modeButton; properties: "anchors.bottomMargin, opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: turnOnButton; properties: "anchors.bottomMargin, opacity"; easing.type: Easing.OutExpo; duration: 300 }
            }
        },
        Transition {
            to: "open"
            ParallelAnimation {
                PropertyAnimation { target: temperatureDial; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: temperatureText; properties: "anchors.topMargin, opacity"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 400 }
                SequentialAnimation {
                    PauseAnimation { duration: 100 }
                    ParallelAnimation {
                        PropertyAnimation { target: turnOnButton; properties: "anchors.bottomMargin, opacity"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 400 }
                        SequentialAnimation {
                            PauseAnimation { duration: 50 }
                            PropertyAnimation { target: modeButton; properties: "anchors.bottomMargin, opacity"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 400 }
                        }
                    }
                }
                SequentialAnimation {
                    PauseAnimation { duration: 100 }
                    PropertyAnimation { target: titleText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                }
            }
        }
    ]

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property double targetTemperature: obj.targetTemperature


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function findNumber(temp) {
        for (var i=0; i<dialListModel.count; i++) {
            if (dialListModel.get(i).temp === temp) {
                return i;
            }
        }
    }

    Component.onCompleted: {
        card.state = "open";

        var tempBase = temperatureDial.from;
        for (var i=temperatureDial.from; i<(temperatureDial.to*2)-3; i++) {
            var item = {};
            item["temp"] = tempBase;
            dialListModel.append(item);
            tempBase = tempBase + 0.5;
        }
        dialListView.currentIndex = findNumber(targetTemperature);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Dial {
        id: temperatureDial
        width: parent.width-100; height: width
        anchors { top: parent.top; topMargin: 140; horizontalCenter: parent.horizontalCenter }
        from: obj.temperatureMin; to: obj.temperatureMax
        stepSize: 0.5
        snapMode: Dial.SnapAlways
        value: card.state == "moving" ? targetTemperature : obj.targetTemperature

        background: Rectangle {
            id: dialBg
            x: temperatureDial.width / 2 - width / 2; y: temperatureDial.height / 2 - height / 2
            width: Math.max(64, Math.min(temperatureDial.width, temperatureDial.height)); height: width
            color: {
                if ((obj.state === Climate.HEAT && targetTemperature > obj.temperature) || (targetTemperature > obj.temperature)) {
                    return Style.color.orange
                } else if ((obj.state === Climate.COOL && targetTemperature < obj.temperature) || targetTemperature < obj.temperature) {
                    return Style.color.blue
                } else {
                    return Style.color.highlight2
                }
            }
            radius: width / 2

            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: Item {
                    width: dialBg.width; height: dialBg.height
                    Rectangle {
                        anchors.fill: parent
                        radius: dialBg.radius
                    }
                }
            }

            ListModel {
                id: dialListModel
            }

            ListView {
                id: dialListView
                width: parent.width; height: parent.height
                spacing: 10
                interactive: false
                highlightMoveDuration: 200
                highlightRangeMode: ListView.StrictlyEnforceRange
                preferredHighlightBegin: height / 2 - 80; preferredHighlightEnd: height / 2 + 80

                model: dialListModel

                delegate: Component {
                    Item {
                        height: 160
                        anchors.horizontalCenter: parent.horizontalCenter
                        opacity: ListView.isCurrentItem ? 1 : 0.5
                        scale: ListView.isCurrentItem ? 1 : 0.5

                        Behavior on opacity {
                            NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
                        }

                        Behavior on scale {
                            NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
                        }

                        Text {
                            id: temperatureTextDial
                            color: Style.color.text
                            text: {
                                var s = temp.toString().split(".")
                                return s[0]
                            }
                            horizontalAlignment: Text.AlignLeft
                            anchors.centerIn: parent
                            font {family: "Open Sans Light"; pixelSize: 140 }
                        }

                        Text {
                            color: Style.color.text
                            text: {
                                var s = temp.toString().split(".")
                                if (s[1]) {
                                    return s[1]
                                } else {
                                    return ""
                                }
                            }
                            anchors { top: temperatureTextDial.top; topMargin: 16; left: temperatureTextDial.right; leftMargin: 0 }
                            font {family: "Open Sans Regular"; pixelSize: 80 }
                        }
                    }
                }
            }
        }

        handle: Rectangle {
            id: handleItem
            x: temperatureDial.background.x + temperatureDial.background.width / 2 - width / 2
            y: temperatureDial.background.y + temperatureDial.background.height / 2 - height / 2
            width: 60; height: 60
            color: Style.color.background
            opacity: 0.5
            radius: 30
            antialiasing: true
            transform: [
                Translate {
                    y: -Math.min(temperatureDial.background.width, temperatureDial.background.height) * 0.45 + handleItem.height / 2
                },
                Rotation {
                    angle: temperatureDial.angle
                    origin.x: handleItem.width / 2
                    origin.y: handleItem.height / 2
                }
            ]
        }

        onValueChanged: {
            Haptic.playEffect(Haptic.Bump);
            targetTemperature = value;
            var i = findNumber(targetTemperature);
            if (i)
                dialListView.currentIndex = i;
        }

        onPressedChanged: {
            if (pressed) {
                card.state = "moving"
            }
            else {
                obj.setTargetTemperature(targetTemperature);
                card.state = "notmoving"
            }
        }
    }

    Text {
        id: icon
        color: Style.color.text
        text: Style.icon.climate
        renderType: Text.NativeRendering
        width: 85; height: 85
        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
        font { family: "icons"; pixelSize: 100 }
        anchors { top: parent.top; topMargin: 20; left: parent.left; leftMargin: 20 }
    }

    Text {
        id: temperatureText
        color: Style.color.text
        text: {
            var s = targetTemperature.toString().split(".")
            return s[0]
        }
        horizontalAlignment: Text.AlignLeft
        anchors { top: icon.bottom; topMargin: 0; left: parent.left; leftMargin: 30 }
        font { family: "Open Sans Light"; pixelSize: 140 }
    }

    Text {
        color: Style.color.text
        opacity: temperatureText.opacity
        text: {
            var s = targetTemperature.toString().split(".")
            if (s[1]) {
                return s[1] + obj.temperatureUnit
            } else {
                return obj.temperatureUnit
            }
        }
        anchors { top: temperatureText.top; topMargin: 16; left: temperatureText.right; leftMargin: 0 }
        font { family: "Open Sans Regular"; pixelSize: 80 }
    }

    Text {
        id: titleText
        color: Style.color.text
        text: obj.friendly_name
        wrapMode: Text.WordWrap
        width: parent.width-60
        anchors { top: temperatureText.bottom; topMargin: 0; left: parent.left; leftMargin: 30 }
        font { family: "Open Sans Regular"; pixelSize: 60 }
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
        anchors { top: titleText.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
        font { family: "Open Sans Regular"; pixelSize: 24 }
    }

    BasicUI.CustomButton {
        id: turnOnButton
        visible: obj.isSupported(Climate.F_OFF) && obj.isSupported(Climate.F_ON)
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
        id: modeButton
        anchors { left: (obj.isSupported(Climate.F_OFF) && obj.isSupported(Climate.F_ON)) ? turnOnButton.right : parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
        color: Style.color.text
        buttonTextColor: Style.color.background
        buttonText: qsTr("Mode") + translateHandler.emptyString
        visible: obj.isSupported(Climate.F_HVAC_MODES)

        mouseArea.onClicked: {
            Haptic.playEffect(Haptic.Click);
            var list = [];

            if (obj.isSupported(Climate.OFF)) {
                var item = {}
                item["original"] = "Off";
                item["translated"] = qsTr("Off")+translateHandler.emptyString;
                list.push(item);
            }
            if (obj.isSupported(Climate.F_HEAT)) {
                item = {}
                item["original"] = "Heat";
                item["translated"] = qsTr("Heat")+translateHandler.emptyString;
                list.push(item);
            }
            if (obj.isSupported(Climate.F_COOL)) {
                item = {}
                item["original"] = "Cool";
                item["translated"] = qsTr("Cool")+translateHandler.emptyString;
                list.push(item);
            }
            contextMenuLoader.setSource("qrc:/components/climate/ui/ContextMenu.qml", { "width": card.width, "climateObj": obj, "list": list })
        }
    }

    Loader {
        id: contextMenuLoader
        anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom }
        visible: modeButton.visible

        onStatusChanged: {
            if (contextMenuLoader.status == Loader.Ready)
                contextMenuLoader.item.state = "open"
        }
    }
}
