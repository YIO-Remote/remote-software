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

Rectangle {
    id: profiles
    width: 480; height: 0
    color: Style.color.highlight2
    radius: Style.cornerRadius
    anchors.bottom: parent.bottom

    Component.onCompleted: {
        parent.width = width;
        parent.height = height;
        parent.anchors.bottom = parent.parent.bottom;

        for (var value in config.ui_config.profiles) {
            var v = config.ui_config.profiles[value];
            v["id"] = value
            profileModel.append(v);
        }

        for (var i=0; i<profileModel.count; i++) {
            if (profileModel.get(i).id === config.profile) {
                profileModel.move(i, 0, 1);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"

    states: [
        State { name: "closed";
            PropertyChanges {target: profiles; height: 0 }
            PropertyChanges {target: parent; height: 0 }
            PropertyChanges {target: loader_main; state: "visible" }
        },
        State { name: "open";
            when: parent.status === Loader.Ready
            PropertyChanges {target: profiles; height: 670 }
            PropertyChanges {target: parent; height: 670 }
            PropertyChanges {target: loader_main; state: "hidden" }
        }
    ]

    transitions: [
        Transition {to: "closed";
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: profiles; properties: "height"; easing.type: Easing.OutExpo; duration: 300 }
                    PropertyAnimation { target: parent; properties: "height"; easing.type: Easing.OutBack; easing.overshoot: 0.8; duration: 300 }
                }
                PropertyAction { target: loader_second; property: "source"; value: "" }
            }
        },
        Transition {to: "open";
            PropertyAnimation { target: profiles; properties: "height"; easing.type: Easing.OutBack; easing.overshoot: 0.8; duration: 300 }
            PropertyAnimation { target: parent; properties: "height"; easing.type: Easing.OutBack; easing.overshoot: 0.8; duration: 300 }
        }
    ]

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Text {
        id: title
        color: Style.color.text
        text: qsTr("Profiles") + translateHandler.emptyString
        anchors { top: parent.top; topMargin: 30; left: parent.left; leftMargin: 30 }
        font { family: "Open Sans Regular"; pixelSize: 60 }
        lineHeight: 0.9
    }

    // PROFILES
    ListView {
        id: profileListView
        width: parent.width-60; height: parent.height
        anchors { top: title.bottom; topMargin: 70; left: parent.left; leftMargin: 30 }
        spacing: 30

        model: profileModel
        delegate: profileDelegate

        move: Transition {
                NumberAnimation { properties: "y"; easing.type: Easing.OutExpo; duration: 300 }
            }
    }

    // MODEL
    ListModel {
        id: profileModel
    }

    // LISTVIEW DELEGATE
    Component {
        id: profileDelegate

        Item {
            width: 380; height: id == config.profile ? 100 : 70

            Rectangle {
                width: parent.width; height: 2
                color: Style.color.light
                visible: id == config.profile ? true : false
                anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }
            }

            Rectangle {
                id: img
                width: 70; height: 70
                radius: width/2
                color: id == config.profile ? Style.color.text : Style.color.highlight1
                anchors { top: parent.top; left: parent.left }

                Text {
                    anchors.centerIn: parent
                    color: id == config.profile ? Style.color.background : Style.color.text
                    text: name.substring(0,1);
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font { family: "Open Sans Bold"; weight: Font.Bold; pixelSize: 40 }
                }
            }

            Text {
                color: id == config.profile ? Style.color.text : Style.color.highlight1
                text: name
                anchors { verticalCenter: img.verticalCenter; left: img.right; leftMargin: 20 }
                font { family: "Open Sans Regular"; pixelSize: 27 }
                lineHeight: 0.9
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    config.profile = id;
                    profileModel.move(index, 0, 1);
                }
            }
        }
    }

    Text {
        id: smallText
        color: Style.color.text
        opacity: 0.5
        text: qsTr("To edit your profiles, use the web configurator tool in settings.") + translateHandler.emptyString
        wrapMode: Text.WordWrap
        width: parent.width - 60
        anchors { left: parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 30 }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 20 }
        lineHeight: 1
    }

    Text {
        id: closeButton
        color: Style.color.text
        text: Style.icon.close
        renderType: Text.NativeRendering
        width: 70; height: 70
        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
        font { family: "icons"; pixelSize: 80 }
        anchors { top: parent.top; topMargin: 20; right: parent.right; rightMargin: 10 }

        MouseArea {
            id: closeButtonMouseArea
            width: parent.width + 20; height: parent.height + 20
            anchors.centerIn: parent

            onClicked: {
                Haptic.playEffect(Haptic.Click);
                profiles.state = "closed"
            }
        }
    }
}
