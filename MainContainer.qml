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

import "qrc:/basic_ui" as BasicUI

Item {
    id: main_container
    width: parent.width
    height: parent.height
    clip: true
    enabled: loader_main.state === "visible" ? true : false
    layer.enabled: true

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO ALL INTEGRATIONS ONCE THE UI IS LOADED
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Component.onCompleted: {
        for (var i=0; i<integrations.list.length; i++) {
            integrations.list[i].connect();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: buttonHandler
        enabled: loader_main.state === "visible" && standbyControl.mode === "on" ? true : false

        onButtonPress: {
            switch (button) {
            case "dpad right":
                if (loader_main.item.mainNavigationSwipeview.currentIndex < loader_main.item.mainNavigationSwipeview.count-1) {
//                    loader_main.item.mainNavigationSwipeview.currentIndex += 1;
                    loader_main.item.mainNavigationSwipeview.incrementCurrentIndex();
                    //                    mainNavigation.mainNavigationListView.currentIndex += 1;
                } else {
                    haptic.playEffect("buzz");
                }
                break;
            case "dpad left":
                if (loader_main.item.mainNavigationSwipeview.currentIndex > 0) {
//                    loader_main.item.mainNavigationSwipeview.currentIndex -= 1;
                    loader_main.item.mainNavigationSwipeview.decrementCurrentIndex();
                    //                    mainNavigation.mainNavigationListView.currentIndex -= 1;
                } else {
                    haptic.playEffect("buzz");
                }
                break;
            case "dpad up":
                var newpos = mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY - 200;
                if (newpos <=0 && mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY == 0) {
                    haptic.playEffect("buzz");
                }
                if (newpos <= 0) {
                    newpos = 0;
                }
                mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY = newpos;
                break;
            case "dpad down":
                newpos = mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY + 200;
                if (newpos >= (mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentHeight - mainNavigationSwipeview.currentItem.mainNavigationLoader.item.height) && mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY == (mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentHeight - mainNavigationSwipeview.currentItem.mainNavigationLoader.item.height)) {
                     haptic.playEffect("buzz");
                }
                if (newpos >= (mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentHeight - mainNavigationSwipeview.currentItem.mainNavigationLoader.item.height)) {
                    newpos = mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentHeight - mainNavigationSwipeview.currentItem.mainNavigationLoader.item.height;
                }
                mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY = newpos;
                break;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // AUTO ROOM SELECTION BASED ON BLUETOOTH TAGS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: bluetoothArea
        enabled: config.settings.bluetootharea

        onCurrentAreaChanged: {
            var p = config.pages //config.ui_config.profiles[config.profile].pages;

            if (mainNavigation.menuConfig[mainNavigation.menuConfig.currentIndex].name != bluetoothArea.currentArea) {
                for (var i=0; i<p.length; i++) {
                    if (p[i].name == bluetoothArea.currentArea) {
                        mainNavigationSwipeview.currentIndex = i;
                    }
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN CONTAINER CONTENT
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias mainNavigationSwipeview: mainNavigationSwipeview
    property int itemsLoaded: 0
    property bool startUp: false

    signal loadedItems(int items)

    SwipeView {
        id: mainNavigationSwipeview
        width: parent.width
        height: parent.height-miniMediaPlayer.height
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter

        currentIndex: 0 //mainNavigation.menuConfig.count-1

        Repeater {
            id: mainNavigationRepeater
            model: mainNavigation.menuConfig

            Loader {
                id: mainNavigationLoader
                asynchronous: true

                property alias mainNavigationLoader: mainNavigationLoader

                function determinePageToLoad(type) {
                    if (type === "favorites") {
                        mainNavigationLoader.source = "qrc:/basic_ui/pages/Favorites.qml";
                    } else if (type === "settings") {
                        mainNavigationLoader.source = "qrc:/basic_ui/pages/Settings.qml";
                    } else {
                        mainNavigationLoader.setSource("qrc:/basic_ui/pages/Page.qml", { "page": page });
                    }
                }

                Component.onCompleted: {
                    determinePageToLoad(page);
                }

                onStatusChanged: {
                    if (mainNavigationLoader.status == Loader.Ready) {
                        itemsLoaded += 1;
                        main_container.loadedItems(itemsLoaded);
                    }
                }
            }
        }

        onCurrentIndexChanged: {
            // change navigation index after startup
            if (mainNavigationSwipeview.count == mainNavigation.menuConfig.count && !startUp) {
                startUp = true
                mainNavigationSwipeview.setCurrentIndex(0);
            }

            if (startUp) {
                mainNavigation.mainNavigationListView.currentIndex = currentIndex;
            }

            if (itemsLoaded >= 3) {
                if (mainNavigation.mainNavigationListView.count !== 0 && !mainNavigation.mainNavigationListView.currentItem && !mainNavigation.mainNavigationListView.currentItem.held) {
                    mainNavigation.mainNavigationListView.currentIndex = currentIndex       ;
                    mainNavigation.mainNavigationListView.positionViewAtIndex(currentIndex, ListView.Center)
                }
            }

            // change the statusbar title
            if (mainNavigation.mainNavigationListView.count !== 0 && mainNavigationSwipeview.currentItem.mainNavigationLoader.item && mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY < 130) {
                statusBar.title = "";
            } else if (mainNavigation.mainNavigationListView.count !== 0 && mainNavigationSwipeview.currentItem.mainNavigationLoader.item) {
                statusBar.title = mainNavigationSwipeview.currentItem.mainNavigationLoader.item.title;
            }

            // change statusbar opacity
            if (mainNavigation.mainNavigationListView.count !== 0 && mainNavigationSwipeview.currentItem.mainNavigationLoader.item && mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY < 10) {
                statusBar.bg.opacity = 0;
            } else if (mainNavigation.mainNavigationListView.count !== 0 && mainNavigationSwipeview.currentItem.mainNavigationLoader.item) {
                statusBar.bg.opacity = 1;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MINI MEDIA PLAYER
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias miniMediaPlayer: miniMediaPlayer
    Item {
        id: miniMediaPlayer
        width: parent.width
        height: 0
        anchors.bottom: parent.bottom

        property alias miniMediaPlayerLoader: miniMediaPlayerLoader

        Loader {
            id: miniMediaPlayerLoader
            active: false
            anchors.fill: parent
        }

        Connections {
            target: entities

            onMediaplayersPlayingChanged: {
                if (!miniMediaPlayerLoader.active && entities.mediaplayersPlaying.length !== 0) {
                    miniMediaPlayer.height = 90;
                    miniMediaPlayerLoader.setSource("qrc:/basic_ui/MiniMediaPlayer.qml")
                    miniMediaPlayerLoader.active = true;
                }

                if (entities.mediaplayersPlaying.length === 0 && miniMediaPlayer.miniMediaPlayerLoader.active) {
                    loader_main.state = "visible";
                    miniMediaPlayer.height = 0;
                    miniMediaPlayer.miniMediaPlayerLoader.active = false;
                    miniMediaPlayer.miniMediaPlayerLoader.source = "";
                }
            }
        }

        Behavior on height {
            NumberAnimation { duration: 400; easing.type: Easing.OutExpo }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN NAVIGATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias mainNavigation: mainNavigation

    BasicUI.MainNavigation {
        id: mainNavigation
        anchors.bottom: miniMediaPlayer.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATUS BAR
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias statusBar: statusBar

    BasicUI.StatusBar {
        id: statusBar
    }

}
