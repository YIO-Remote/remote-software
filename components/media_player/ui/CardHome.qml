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

import QtQuick 2.0

import Haptic 1.0
import Entity.MediaPlayer 1.0
import Style 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: cardHome
    width: parent.width; height: parent.height
    color: mediaplayerUtils.pixelColor === "#000000" ? Style.colorDark : mediaplayerUtils.pixelColor
    radius: Style.cornerRadius

    Behavior on color {
        ColorAnimation { duration: 300 }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"

    states: [
        State {
            name: "open"
            PropertyChanges {target: songTitle; opacity: 1; anchors.topMargin: 20 }
            PropertyChanges {target: artist; opacity: 1 }
            PropertyChanges {target: playButton; opacity: 1; anchors.bottomMargin: 80 }
            PropertyChanges {target: prevButton; opacity: 1 }
            PropertyChanges {target: nextButton; opacity: 1 }
            PropertyChanges {target: progressBar; opacity: 1 }
            PropertyChanges {target: albumArt; opacity: 1; visible: true; anchors.topMargin: 118; scale: 1 }
        },
        State {
            name: "closed"
            PropertyChanges {target: songTitle; opacity: 0; anchors.topMargin: 40 }
            PropertyChanges {target: artist; opacity: 0 }
            PropertyChanges {target: playButton; opacity: 0; anchors.bottomMargin: 40 }
            PropertyChanges {target: prevButton; opacity: 0 }
            PropertyChanges {target: nextButton; opacity: 0 }
            PropertyChanges {target: progressBar; opacity: 0 }
            PropertyChanges {target: albumArt; opacity: 0; anchors.topMargin: 158; scale: 0.8 }
        }
    ]

    transitions: [
        Transition {
            to: "open"
            SequentialAnimation {
                PauseAnimation { duration: 150 }
                PropertyAnimation { target: albumArt; properties: "visible"; duration: 1 }
                ParallelAnimation {
                    PropertyAnimation { target: albumArt; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
                    PropertyAnimation { target: albumArt; properties: "anchors.topMargin, scale"; easing.type: Easing.OutBack; easing.overshoot: 1.4; duration: 400 }
                }
            }
            SequentialAnimation {
                PauseAnimation { duration: 250 }
                ParallelAnimation {
                    PropertyAnimation { target: songTitle; properties: "opacity, anchors.topMargin"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 400 }
                    PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                    SequentialAnimation {
                        PauseAnimation { duration: 100 }
                        PropertyAnimation { target: progressBar; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                    }
                }
            }
            SequentialAnimation {
                PauseAnimation { duration: 350 }
                ParallelAnimation {
                    PropertyAnimation { target: playButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                    PropertyAnimation { target: playButton; properties: "anchors.bottomMargin"; easing.type: Easing.OutBack; easing.overshoot: 1.4; duration: 300 }
                    PropertyAnimation { target: prevButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                    PropertyAnimation { target: nextButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                }
            }
        },
        Transition {
            to: "closed"
            PropertyAnimation { target: albumArt; properties: "opacity, anchors.topMargin, scale"; easing.type: Easing.OutExpo; duration: 200 }
            PropertyAnimation { target: songTitle; properties: "opacity"; easing.type: Easing.OutExpo; duration: 200 }
            PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 200 }
            PropertyAnimation { target: progressBar; properties: "opacity"; easing.type: Easing.OutExpo; duration: 200 }
            PropertyAnimation { target: playButton; properties: "opacity, anchors.bottomMargin"; easing.type: Easing.OutExpo; duration: 200 }
            PropertyAnimation { target: prevButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 200 }
            PropertyAnimation { target: nextButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 200 }
        }
    ]

    Component.onCompleted: {
        cardHome.state = "open";
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: parentCard
        onStateChanged: {
            if (parentCard.state === "closed") {
                cardHome.state = "closed";
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Item {
        width: parent.width; height: 125
        anchors.top: parent.top

        Text {
            id: title
            color: Style.colorText
            text: obj.friendly_name
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            width: parent.width-232
            anchors { left: parent.left; leftMargin: 106; verticalCenter: parent.verticalCenter; verticalCenterOffset: obj.source === "" ? 0 : -15 }
            font: Style.fonts.button
        }

        Text {
            id: info
            color: Style.colorText
            opacity: 0.5
            text: obj.source
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: title.width
            anchors { left: title.left; top: title.bottom; topMargin: -2 }
            font { family: "Open Sans Regular"; pixelSize: 20 }
            lineHeight: 1
        }

        Text {
            color: Style.colorText
            text: icon.text
            width: 85; height: 85
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 100 }
            anchors { left: parent.left; leftMargin: 10; verticalCenter: parent.verticalCenter }
        }
    }

    BasicUI.CustomImageLoader {
        id: albumArt
        width: 280; height: 280
        anchors { top: parent.top; topMargin: 118; horizontalCenter: parent.horizontalCenter }
        url: mediaplayerUtils.image === "" ? "qrc:/images/mini-music-player/no_image.png" : mediaplayerUtils.image
    }

    Text {
        id: songTitle
        color: Style.colorText
        text: obj.mediaTitle
        horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
        width: parent.width-80
        font { family: "Open Sans Bold"; weight: Font.Bold; pixelSize: 30 }
        lineHeight: 1
        anchors { top: albumArt.bottom; topMargin: 20; horizontalCenter: parent.horizontalCenter }
    }

    Text {
        id: artist
        color: Style.colorText
        text: obj.mediaArtist
        horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
        width: parent.width-80
        font: Style.fonts.button
        anchors { top: songTitle.bottom; horizontalCenter: parent.horizontalCenter }
    }

    Rectangle {
        id: progressBar
        visible: obj.isSupported(MediaPlayer.F_MEDIA_POSITION)
        width: parent.width-80; height: 4
        color: "#000000"
        anchors { top: artist.bottom; topMargin: 20; horizontalCenter: parent.horizontalCenter }

        Rectangle {
            width: parent.width*(obj.mediaProgress/obj.mediaDuration); height: parent.height
            color: Style.colorLine

            Behavior on width {
                NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
            }
        }
    }

    Item {
        id: prevButton
        width: 120; height: 120

        anchors { right: playButton.left; rightMargin: 30; verticalCenter: playButton.verticalCenter }

        Text {
            color: Style.colorText
            text: Style.icons.prev
            renderType: Text.NativeRendering
            width: 85; height: 85
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                Haptic.playEffect(Haptic.Click);
                obj.previous();
            }
        }
    }

    Item {
        id: playButton
        width: 120; height: 120

        anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom; bottomMargin: 80 }

        Text {
            color: Style.colorText
            text: Style.icons.pause
            renderType: Text.NativeRendering
            width: 85; height: 85
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
            opacity: obj.state === 3 ? 1 : 0

            Behavior on opacity {
                NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
            }
        }

        Text {
            color: Style.colorText
            text: Style.icons.play
            renderType: Text.NativeRendering
            width: 85; height: 85
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
            opacity: obj.state === 3 ? 0 : 1

            Behavior on opacity {
                NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
            }
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                Haptic.playEffect(Haptic.Click);
                if (obj.state === 3) {
                    obj.pause();
                } else {
                    obj.play();
                }
            }
        }
    }

    Item {
        id: nextButton
        width: 120; height: 120

        anchors { left: playButton.right; leftMargin: 30; verticalCenter: playButton.verticalCenter }

        Text {
            color: Style.colorText
            text: Style.icons.next
            renderType: Text.NativeRendering
            width: 85; height: 85
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                Haptic.playEffect(Haptic.Click);
                obj.next();
            }
        }
    }
}
