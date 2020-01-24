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
import StandbyControl 1.0

import "qrc:/basic_ui" as BasicUI

import Haptic 1.0
import ButtonHandler 1.0

import Entity.MediaPlayer 1.0
import MediaPlayerUtils 1.0

Item {
    id: miniMediaPlayer
    width: 480
    height: 90
    anchors.bottom: parent.bottom

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"


    states: [
        State { name: "closed";
            PropertyChanges {target: miniMediaPlayer; height: 90 }
            ParentChange { target: miniMediaPlayer; parent: loader_main.item.miniMediaPlayer; scale: 1 }
            PropertyChanges {target: loader_main; state: "visible" }
        },
        State { name: "open";
            PropertyChanges {target: miniMediaPlayer; height: 670 }
            ParentChange { target: miniMediaPlayer; parent: contentWrapper }
            PropertyChanges {target: loader_main; state: "hidden" }
        }
    ]
    transitions: [
        Transition {to: "closed";
            SequentialAnimation {
                PauseAnimation { duration: 200 }
                ParallelAnimation {
                    PropertyAnimation { target: loader_main; properties: "state"; duration: 1 }
                    PropertyAnimation { target: miniMediaPlayer; properties: "height"; easing.type: Easing.OutBack; easing.overshoot: 0.7; duration: 300 }
                }
                ParentAnimation {
                    NumberAnimation { properties: "scale"; easing.type: Easing.OutBack; easing.overshoot: 0.7; duration: 300 }
                }
            }
        },
        Transition {to: "open";
            ParallelAnimation {
                PropertyAnimation { target: miniMediaPlayer; properties: "height"; easing.type: Easing.OutBack; easing.overshoot: 1.2; duration: 300 }
                ParentAnimation {
                    NumberAnimation { properties: "scale"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 300 }
                }
            }
        }
    ]


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    layer.enabled: true
    layer.effect: OpacityMask {
        maskSource:
            Rectangle {
            id: opacityMask
            width: miniMediaPlayer.width
            height: miniMediaPlayer.height
            radius: miniMediaPlayer.state == "closed" ? 0 : Style.cornerRadius

            Behavior on radius {
                NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Connections {
        target: ButtonHandler
        enabled: miniMediaPlayer.state == "open" && (StandbyControl.mode === StandbyControl.ON || StandbyControl.mode === StandbyControl.DIM)

        onButtonPressed: {
            switch (button) {
            case ButtonHandler.DPAD_MIDDLE:
                entities.mediaplayersPlaying[mediaPlayers.currentIndex].play();
                break;
            case ButtonHandler.DPAD_RIGHT:
                if (mediaPlayers.currentIndex < mediaPlayers.count-1) {
                    mediaPlayers.currentIndex += 1;
                } else {
                    Haptic.playEffect(Haptic.Buzz);
                }
                break;
            case ButtonHandler.DPAD_LEFT:
                if (mediaPlayers.currentIndex > 0) {
                    mediaPlayers.currentIndex -= 1;
                } else {
                    Haptic.playEffect(Haptic.Buzz);
                }
                break;
            case ButtonHandler.TOP_RIGHT:
                miniMediaPlayer.state = "closed";
                break;
            }
        }
    }

    Connections {
        target: ButtonHandler
        enabled: loader_main.state === "visible" || miniMediaPlayer.state == "open" ? true : false

        onButtonPressed: {
            switch (button) {
            case ButtonHandler.VOLUME_UP:
                buttonTimeout.stop();
                buttonTimeout.volumeUp = true;
                buttonTimeout.start();
                break;
            case ButtonHandler.VOLUME_DOWN:
                buttonTimeout.stop();
                buttonTimeout.volumeUp = false;
                buttonTimeout.start();
                break;
            }
        }

        onButtonReleased: {
            buttonTimeout.stop();
        }
    }

    Timer {
        id: buttonTimeout
        interval: 300
        repeat: true
        running: false
        triggeredOnStart: true

        property bool volumeUp: false

        onTriggered: {
            if (volumeUp) {
                if (volume.state !== "visible") {
                    volume.volumePosition = mediaPlayers.currentItem.player.obj.volume;
                    volume.state = "visible";
                }
                var newvolume = mediaPlayers.currentItem.player.obj.volume + 2;
                if (newvolume > 100) newvolume = 100;
                mediaPlayers.currentItem.player.obj.setVolume(newvolume);
                volume.volumePosition = newvolume;
            } else {
                if (volume.state !== "visible") {
                    volume.volumePosition = mediaPlayers.currentItem.player.obj.volume;
                    volume.state = "visible";
                }
                newvolume = mediaPlayers.currentItem.player.obj.volume - 2;
                if (newvolume < 0) newvolume = 0;
                mediaPlayers.currentItem.player.obj.setVolume(newvolume);
                volume.volumePosition = newvolume;
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Style.colorBackground
    }

    SwipeView {
        id: mediaPlayers
        anchors.fill: parent

        Repeater {
            id: mediaPlayersRepeater
            model: entities.mediaplayersPlaying.length

            Item {
                id: player
                width: 480

                // include mediaplayer utils
                MediaPlayerUtils {
                    id: mediaplayerUtils
                }

                property alias player: player

                property var obj: entities.mediaplayersPlaying[index]

                state: "closed"

                states: [
                    State {
                        name: "open"
                        when: miniMediaPlayer.state == "open"
                        PropertyChanges {target: title; opacity: 0 }
                        PropertyChanges {target: artist; opacity: 0 }
                        PropertyChanges {target: closeButton; opacity: 1 }
                        PropertyChanges {target: titleOpen; y: 380; opacity: 1 }
                        PropertyChanges {target: artistOpen; opacity: 0.8 }
                        PropertyChanges {target: indicator; opacity: 1 }
                        PropertyChanges {target: speaker; opacity: 1 }
                        PropertyChanges {target: playButton; opacity: 1; anchors.bottomMargin: 80 }
                        PropertyChanges {target: prevButton; opacity: 1 }
                        PropertyChanges {target: nextButton; opacity: 1 }
                        PropertyChanges {target: sourceText; opacity: 1 }
                        PropertyChanges {target: bgImage; opacity: 1; visible: true; anchors.topMargin: 86; scale: 1 }
                        PropertyChanges {target: image; opacity: 0 }
                    },
                    State {
                        name: "closed"
                        when: miniMediaPlayer.state == "closed"
                        PropertyChanges {target: title; opacity: 1 }
                        PropertyChanges {target: artist; opacity: 1 }
                        PropertyChanges {target: closeButton; opacity: 0 }
                        PropertyChanges {target: titleOpen; y: 420; opacity: 0 }
                        PropertyChanges {target: artistOpen; opacity: 0 }
                        PropertyChanges {target: indicator; opacity: 0 }
                        PropertyChanges {target: speaker; opacity: 0 }
                        PropertyChanges {target: playButton; opacity: 0; anchors.bottomMargin: 40 }
                        PropertyChanges {target: prevButton; opacity: 0 }
                        PropertyChanges {target: nextButton; opacity: 0 }
                        PropertyChanges {target: sourceText; opacity: 0 }
                        PropertyChanges {target: bgImage; opacity: 0; visible: false; anchors.topMargin: 126; scale: 0.8 }
                        PropertyChanges {target: image; opacity: 1 }
                    }]

                transitions: [
                    Transition {
                        to: "open"
                        SequentialAnimation {
                            ParallelAnimation {
                                PropertyAnimation { target: title; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: closeButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: indicator; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: image; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                SequentialAnimation {
                                    PauseAnimation { duration: 300 }
                                    ParallelAnimation {
                                        PropertyAnimation { target: titleOpen; properties: "y, opacity"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 400 }
                                        PropertyAnimation { target: sourceText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                        SequentialAnimation {
                                            //                                            PauseAnimation { duration: 200 }
                                            PropertyAnimation { target: artistOpen; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                            ParallelAnimation {
                                                PropertyAnimation { target: playButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                                PropertyAnimation { target: playButton; properties: "anchors.bottomMargin"; easing.type: Easing.OutBack; easing.overshoot: 1.4; duration: 300 }
                                                PropertyAnimation { target: prevButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                                PropertyAnimation { target: nextButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                            }
                                        }
                                        PropertyAnimation { target: speaker; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                        SequentialAnimation {
                                            PropertyAnimation { target: bgImage; properties: "visible"; duration: 1 }
                                            ParallelAnimation {
                                                PropertyAnimation { target: bgImage; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
                                                PropertyAnimation { target: bgImage; properties: "anchors.topMargin, scale"; easing.type: Easing.OutBack; easing.overshoot: 1.4; duration: 400 }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    },
                    Transition {
                        to: "closed"
                        SequentialAnimation {
                            PropertyAnimation { target: bgImage; properties: "opacity, anchors.topMargin, scale"; easing.type: Easing.OutExpo; duration: 200 }
                            PropertyAnimation { target: bgImage; properties: "visible"; duration: 1 }
                            ParallelAnimation {
                                PropertyAnimation { target: title; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: closeButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: titleOpen; properties: "y, opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: artistOpen; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: indicator; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: speaker; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: playButton; properties: "opacity, anchors.bottomMargin"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: prevButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: nextButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: sourceText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: image; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                            }
                        }
                    }
                ]

                Rectangle {
                    id: comp
                    anchors.fill: parent
                    color: mediaplayerUtils.pixelColor

                    Behavior on color {
                        ColorAnimation { duration: 300 }
                    }

                    property var m_image: obj ? obj.mediaImage : ""

                    onM_imageChanged: {
                        if (obj) {
                            mediaplayerUtils.imageURL = obj.mediaImage
                        }
                    }

                    CustomImageLoader {
                        id: bgImage
                        width: 280
                        height: 280
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 86
                        url: obj && obj.mediaImage === "" ? "qrc:/images/mini-music-player/no_image.png" : mediaplayerUtils.image //utils.miniMusicPlayerImage
                    }
                }

                CustomImageLoader {
                    id: image
                    width: 90
                    height: width
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: 0
                    }
                    url: obj && obj.mediaImage === "" ? "qrc:/images/mini-music-player/no_image.png" : mediaplayerUtils.smallImage
                }

                Item {
                    id: textContainer
                    height: childrenRect.height

                    anchors.left: image.right
                    anchors.leftMargin: 20
                    anchors.verticalCenter: image.verticalCenter

                    Text {
                        id: title
                        color: Style.colorText
                        text: obj ? obj.friendly_name : ""
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        wrapMode: Text.NoWrap
                        width: 304
                        font.family: "Open Sans Regular"
                        font.weight: Font.Normal
                        font.pixelSize: 25
                        lineHeight: 1
                    }

                    Text {
                        id: artist
                        color: Style.colorText
                        text: obj ? obj.mediaTitle : ""
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        wrapMode: Text.NoWrap
                        width: 304
                        font.family: "Open Sans Regular"
                        font.weight: Font.Normal
                        font.pixelSize: 20
                        lineHeight: 1
                        anchors.top: title.bottom
                        anchors.topMargin: -2
                        opacity: 0.6
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: miniMediaPlayer.state == "closed" ? true : false

                    onClicked: {
                        miniMediaPlayer.state = "open";
                    }
                }

                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // OPEN STATE ELEMENTS
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                Text {
                    id: sourceText
                    color: Style.colorText
                    text: obj ? obj.source : ""
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    font.family: "Open Sans Regular"
                    font.weight: Font.Normal
                    font.pixelSize: 27
                    anchors {
                        top: parent.top
                        topMargin: 20
                        left: parent.left
                        leftMargin: 20
                    }
                }


                Text {
                    id: titleOpen
                    color: Style.colorText
                    text: obj ? obj.mediaTitle : ""
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    width: parent.width-80
                    font.family: "Open Sans Regular"
                    font.weight: Font.Bold
                    font.styleName: "Bold"
                    font.pixelSize: 30
                    lineHeight: 1
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    id: artistOpen
                    color: Style.colorText
                    text: obj ? obj.mediaArtist : ""
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    width: parent.width-80
                    font.family: "Open Sans Regular"
                    font.weight: Font.Normal
                    font.pixelSize: 27
                    lineHeight: 1
                    anchors.top: titleOpen.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Item {
                    id: speaker
                    width: childrenRect.width
                    anchors {
                        bottom: parent.bottom
                        bottomMargin: 80
                        horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        id: speakerIcon
                        color: Style.colorText
                        text: Style.icons.speaker
                        renderType: Text.NativeRendering
                        width: 60
                        height: 60
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        font {family: "icons"; pixelSize: 80 }
                    }

                    Text {
                        color: Style.colorText
                        text: obj ? obj.friendly_name : ""
                        verticalAlignment: Text.AlignVCenter
                        font.family: "Open Sans Regular"
                        font.weight: Font.Normal
                        font.pixelSize: 27
                        lineHeight: 1
                        anchors { left: speakerIcon.right; verticalCenter: speakerIcon.verticalCenter }
                    }
                }
            }
        }

    }

    Text {
        id: closeButton
        color: Style.colorText
        text: Style.icons.down_arrow
        renderType: Text.NativeRendering
        width: 70
        height: 70
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font {family: "icons"; pixelSize: 80 }
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10

        MouseArea {
            width: parent.width + 20
            height: parent.height + 20
            anchors.centerIn: parent
            enabled: miniMediaPlayer.state == "open"

            onClicked: {
                Haptic.playEffect(Haptic.Click);
                miniMediaPlayer.state = "closed"
            }
        }
    }

    Item {
        id: prevButton
        width: 120
        height: 120

        anchors {
            right: playButton.left
            rightMargin: 30
            verticalCenter: playButton.verticalCenter
        }

        Text {
            color: Style.colorText
            text: Style.icons.prev
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            enabled: miniMediaPlayer.state == "open"

            onClicked: {
                Haptic.playEffect(Haptic.Click);
                entities.mediaplayersPlaying[mediaPlayers.currentIndex].previous();
            }
        }
    }

    Item {
        id: playButton
        width: 120
        height: 120

        property bool isPlaying: entities.mediaplayersPlaying[mediaPlayers.currentIndex] && entities.mediaplayersPlaying[mediaPlayers.currentIndex].state === MediaPlayer.PLAYING ? true : false

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 80
        }

        Text {
            color: Style.colorText
            text: Style.icons.pause
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
            opacity: playButton.isPlaying ? 1 : 0

            Behavior on opacity {
                NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
            }
        }

        Text {
            color: Style.colorText
            text: Style.icons.play
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
            opacity: playButton.isPlaying ? 0 : 1

            Behavior on opacity {
                NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
            }
        }

        MouseArea {
            anchors.fill: parent
            enabled: miniMediaPlayer.state == "open"

            onClicked: {
                Haptic.playEffect(Haptic.Click);
                if (entities.mediaplayersPlaying[mediaPlayers.currentIndex].state === MediaPlayer.PLAYING ) {
                    entities.mediaplayersPlaying[mediaPlayers.currentIndex].pause();
                } else {
                    entities.mediaplayersPlaying[mediaPlayers.currentIndex].play();
                }
            }
        }
    }

    Item {
        id: nextButton
        width: 120
        height: 120

        anchors {
            left: playButton.right
            leftMargin: 30
            verticalCenter: playButton.verticalCenter
        }

        Text {
            color: Style.colorText
            text: Style.icons.next
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            enabled: miniMediaPlayer.state == "open"

            onClicked: {
                Haptic.playEffect(Haptic.Click);
                entities.mediaplayersPlaying[mediaPlayers.currentIndex].next();
            }
        }
    }

    PageIndicator {
        id: indicator

        count: mediaPlayers.count
        currentIndex: mediaPlayers.currentIndex

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter

        delegate: Rectangle {
            width: 8
            height: 8
            radius: height/2
            color: Style.colorText
            opacity: index == mediaPlayers.currentIndex ? 1 : 0.3
        }
    }
}
