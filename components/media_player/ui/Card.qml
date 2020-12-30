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
import Style 1.0

import Haptic 1.0
import ButtonHandler 1.0

import Entity.MediaPlayer 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: card
    width: parent.width; height: parent.height
    //    color: mediaplayerUtils.pixelColor === "#000000" ? Style.color.dark : mediaplayerUtils.pixelColor
    color: Style.color.dark
    radius: Style.cornerRadius

    Behavior on color {
        ColorAnimation { duration: 300 }
    }

    property alias parentCard: card

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: ButtonHandler
        enabled: mediaplayerButton.state === "open" ? true : false

        onButtonPressed: {
            switch (button) {
            case ButtonHandler.VOLUME_UP:
                volume.stop();
                volume.volumeUp = true;
                volume.start(obj.volume);
                break;
            case ButtonHandler.VOLUME_DOWN:
                volume.stop();
                volume.volumeUp = false;
                volume.start(obj.volume);
                break;
            case ButtonHandler.CHANNEL_UP:
                if (obj.isSupported(MediaPlayer.F_CHANNEL_UP)) {
                    obj.channelUp();
                }
                break;
            case ButtonHandler.CHANNEL_DOWN:
                if (obj.isSupported(MediaPlayer.F_CHANNEL_DOWN)) {
                    obj.channelDown();
                }
                break;
            case ButtonHandler.DPAD_UP:
                if (obj.isSupported(MediaPlayer.F_UP)) {
                    obj.cursorUp();
                }
                break;
            case ButtonHandler.DPAD_DOWN:
                if (obj.isSupported(MediaPlayer.F_DOWN)) {
                    obj.cursorDown();
                }
                break;
            case ButtonHandler.DPAD_LEFT:
                if (obj.isSupported(MediaPlayer.F_LEFT)) {
                    obj.cursorLeft();
                }
                break;
            case ButtonHandler.DPAD_RIGHT:
                if (obj.isSupported(MediaPlayer.F_RIGHT)) {
                    obj.cursorRight();
                }
                break;
            case ButtonHandler.DPAD_MIDDLE:
                if (obj.isSupported(MediaPlayer.F_OK)) {
                    obj.cursorOK();
                }
                break;
            case ButtonHandler.TOP_LEFT:
                if (obj.isSupported(MediaPlayer.F_MUTE)) {
                    obj.muteToggle();
                }
                break;
            case ButtonHandler.BOTTOM_LEFT:
                if (obj.isSupported(MediaPlayer.F_BACK)) {
                    obj.back();
                }
                break;
            case ButtonHandler.BOTTOM_RIGHT:
                if (obj.isSupported(MediaPlayer.F_MENU)) {
                    obj.menu();
                }
                break;
            }
        }

        onButtonReleased: {
            volume.stop();
        }
    }

    Connections {
        target: volume

        onVolumeChanged: {
            if (volume.volumeUp) {
                if (volume.state !== "visible") {
                    volume.state = "visible";
                }
                obj.setVolume(newVolume);
            } else {
                if (volume.state !== "visible") {
                    volume.state = "visible";
                }
                obj.setVolume(newVolume);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"

    states: [
        State {
            name: "open"
        },
        State {
            name: "closed"
            PropertyChanges {target: bottomMenu; opacity: 0; }
            PropertyChanges {target: cardSwipeView; opacity: 0; }
        }
    ]

    transitions: [
        Transition {
            to: "closed"
            PropertyAnimation { target: bottomMenu; properties: "opacity"; easing.type: Easing.OutExpo; duration: 200 }
            PropertyAnimation { target: cardSwipeView; properties: "opacity"; easing.type: Easing.OutExpo; duration: 200 }
        }
    ]

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property var features: []
    property bool startup: true

    Component.onCompleted: {
        card.state = "open";

        features.push("HOME");
        if (obj.isSupported(MediaPlayer.F_SEARCH))
            features.push("SEARCH");
        if (obj.isSupported(MediaPlayer.F_LIST))
            features.push("LIST");
        if (obj.isSupported(MediaPlayer.F_TVCHANNELLIST))
            features.push("TVCHANNELLIST");
        if (obj.isSupported(MediaPlayer.F_MEDIAPLAYERREMOTE))
            features.push("MEDIAPLAYERREMOTE");
        if (obj.isSupported(MediaPlayer.F_MEDIAPLAYEREPGVIEW))
            features.push("MEDIAPLAYEREPGVIEW");
        if (obj.isSupported(MediaPlayer.F_SPEAKERCONTROL))
            features.push("SPEAKER_CONTROL");

        cardRepeater.model = features;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SIGNALS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    signal looseFocus()


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    SwipeView {
        id: cardSwipeView
        width: parent.width; height: parent.height
        anchors.centerIn: parent
        currentIndex: 0

        onCurrentIndexChanged: {
            if (cardSwipeView.currentIndex != features.indexOf("SEARCH"))
                looseFocus();
            if (currentItem.item) {
                currentItem.item.swipeView.currentIndex = 0;
            }
        }

        Item { CardHome {} }

        Repeater {
            id: cardRepeater

            Loader {
                id: loader
                active: SwipeView.isCurrentItem || SwipeView.isPreviousItem || SwipeView.isNextItem
                asynchronous: true
                sourceComponent: {
                    if (card.features.indexOf("SEARCH")-1 == index ) {
                        return cardSearch;
                    } else if (card.features.indexOf("LIST")-1 == index ) {
                        return cardList;
                    } else if (card.features.indexOf("TVCHANNELLIST")-1 == index ) {
                        return cardTVList;
                    } else if (card.features.indexOf("MEDIAPLAYERREMOTE")-1 == index ) {
                        return cardMediaPlayerRemote;
                    } else if (card.features.indexOf("MEDIAPLAYEREPGVIEW")-1 == index ) {
                        return cardMediaPlayerEPGView;
                    } else if (card.features.indexOf("SPEAKER_CONTROL")-1 == index ) {
                        return cardSpeakerControl;
                    }
                }
                property bool _currentItem: SwipeView.isCurrentItem
            }
        }

        Component {
            id: cardHome
            CardHome {}
        }

        Component {
            id: cardSearch
            CardSearch {}
        }

        Component {
            id: cardList
            CardPlaylists {}
        }

        Component {
            id: cardTVList
            CardTVChannelList {}
        }

        Component {
            id: cardMediaPlayerRemote
            CardMediaPlayerRemote {}
        }

        Component {
            id: cardMediaPlayerEPGView
            CardMediaPlayerEPGView {}
        }

        // TODO: Speaker control UI is missing
        Component {
            id: cardSpeakerControl
            Rectangle {
                anchors.fill: parent
                color: "green"
            }
        }
    }

    // BOTTOM MENU
    Grid {
        id: bottomMenu
        width: childrenRect.width; height: 60
        anchors { bottom: parent.bottom; bottomMargin: 20; horizontalCenter: parent.horizontalCenter }
        spacing: {
            var i = 0;

            if (obj.isSupported(MediaPlayer.F_SEARCH))
                i++;
            if (obj.isSupported(MediaPlayer.F_LIST))
                i++;
            if (obj.isSupported(MediaPlayer.F_TVCHANNELLIST))
                i++;
            if (obj.isSupported(MediaPlayer.F_MEDIAPLAYERREMOTE))
                i++;
            if (obj.isSupported(MediaPlayer.F_MEDIAPLAYEREPGVIEW))
                i++;
            if (obj.isSupported(MediaPlayer.F_SPEAKERCONTROL))
                i++;

            if (i === 0)
                return 100;
            else if (i === 1)
                return 100;
            else if (i === 2)
                return 80;
            else if (i === 3)
                return 40;
            else if (i === 4)
                return 20;
        }

        // home
        Text {
            color: Style.color.text
            opacity: cardSwipeView.currentIndex === 0 ? 1 : 0.5
            text: Style.icon.home
            renderType: Text.NativeRendering
            width: 60; height: 60
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }

            MouseArea {
                anchors.fill: parent
                width: parent.width + 30; height: width

                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    //                    cardSwipeView.currentIndex = 0;
                    cardSwipeView.setCurrentIndex(0);
                }
            }
        }

        // search
        Text {
            visible: obj.isSupported(MediaPlayer.F_SEARCH) ? true : false
            color: Style.color.text
            opacity: cardSwipeView.currentIndex === features.indexOf("SEARCH") ? 1 : 0.5
            text: Style.icon.search
            renderType: Text.NativeRendering
            width: 60; height: 60
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }

            MouseArea {
                anchors.fill: parent
                width: parent.width + 30; height: width

                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    cardSwipeView.setCurrentIndex(features.indexOf("SEARCH"));
                }
            }
        }

        // playlists
        Text {
            visible: obj.isSupported(MediaPlayer.F_LIST) ? true : false
            color: Style.color.text
            opacity: cardSwipeView.currentIndex === features.indexOf("LIST") ? 1 : 0.5
            text: Style.icon.playlist
            renderType: Text.NativeRendering
            width: 60; height: 60
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }

            MouseArea {
                anchors.fill: parent
                width: parent.width + 30; height: width

                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    cardSwipeView.setCurrentIndex(features.indexOf("LIST"));
                }
            }
        }

        //TVChannelList
        Text {
            visible: obj.isSupported(MediaPlayer.F_TVCHANNELLIST) ? true : false
            color: Style.color.text
            opacity: cardSwipeView.currentIndex === features.indexOf("TVCHANNELLIST") ? 1 : 0.5
            text: Style.icon.playlist
            renderType: Text.NativeRendering
            width: 60; height: 60
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }

            MouseArea {
                anchors.fill: parent
                width: parent.width + 30; height: width

                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    cardSwipeView.setCurrentIndex(features.indexOf("TVCHANNELLIST"));
                }
            }
        }

        //MediaPlayerRemote
        Text {
            visible: obj.isSupported(MediaPlayer.F_MEDIAPLAYERREMOTE) ? true : false
            color: Style.color.text
            opacity: cardSwipeView.currentIndex === features.indexOf("MEDIAPLAYERREMOTE") ? 1 : 0.5
            text: Style.icon.remote
            renderType: Text.NativeRendering
            width: 60; height: 60
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }

            MouseArea {
                anchors.fill: parent
                width: parent.width + 30; height: width

                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    cardSwipeView.setCurrentIndex(features.indexOf("MEDIAPLAYERREMOTE"));
                }
            }
        }

        //EPGView
        Text {
            visible: obj.isSupported(MediaPlayer.F_MEDIAPLAYEREPGVIEW) ? true : false
            color: Style.color.text
            opacity: cardSwipeView.currentIndex === features.indexOf("MEDIAPLAYEREPGVIEW") ? 1 : 0.5
            text: Style.icon.remote
            renderType: Text.NativeRendering
            width: 60; height: 60
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }

            MouseArea {
                anchors.fill: parent
                width: parent.width + 30; height: width

                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    cardSwipeView.setCurrentIndex(features.indexOf("MEDIAPLAYEREPGVIEW"));
                }
            }
        }

        // speakers
        Text {
            visible: obj.isSupported(MediaPlayer.F_SPEAKER_CONTROL) ? true : false
            color: Style.color.text
            opacity: cardSwipeView.currentIndex === features.indexOf("SPEAKER_CONTROL") ? 1 : 0.5
            text: Style.icon.speaker
            renderType: Text.NativeRendering
            width: 60; height: 60
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }

            MouseArea {
                anchors.fill: parent
                width: parent.width + 30; height: width

                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    cardSwipeView.setCurrentIndex(features.indexOf("SPEAKER_CONTROL"));
                }
            }
        }
    }

    property alias contextMenuLoader: contextMenuLoader

    Loader {
        id: contextMenuLoader
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }

        onStatusChanged: {
            if (contextMenuLoader.status == Loader.Ready)
                contextMenuLoader.item.state = "open"
        }
    }
}
