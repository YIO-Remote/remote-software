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
import QtQml.Models 2.3
import Style 1.0

import Haptic 1.0

Item {
    id: mainNavigation
    width: parent.width; height: 70

    Rectangle {
        anchors.fill: parent
        color: Style.color.background
    }

    MouseArea {
        anchors.fill: parent
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MENU CONFIGURATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias menuConfig: menuConfig

    ListModel {
        id: menuConfig
    }

    Connections {
        target: config

        onProfileIdChanged: {
            console.debug("Profile Id changed. Reload UI.");
            loadmenuConfig();
        }
    }

    //: menu items that are in the bottom menu
    property var menuTranslations: [qsTr("Favorites") + translateHandler.emptyString, qsTr("Settings") + translateHandler.emptyString,
        qsTr("Lights") + translateHandler.emptyString, qsTr("Blinds") + translateHandler.emptyString, qsTr("Media") + translateHandler.emptyString
    ]

    function loadmenuConfig() {
        // clear the menuConfig
        menuConfig.clear();

        var pageNames = config.getProfilePages();

        for (var j = 0; j < pageNames.length; j++) {
            var c = {};
            c.page = pageNames[j];
            if (c.page === "favorites") {
                c.friendly_name = "Favorites";
            } else if (c.page === "settings") {
                c.friendly_name = "Settings";
            } else {
                c.friendly_name = config.getPage(c.page).name;
            }

            // add to listmodel
            menuConfig.append(c);
        }
    }

    function savemenuConfig() {
        console.debug("SAVE MENUCONFIG");

        var tmp = config.config;

        var newConfig = [];

        for (var i = 0; i < menuConfig.count; i++) {
            var found = false;

            for (var j = 0; j < tmp.ui_config.profiles[config.profileId].pages.length && !found; j++) {
                if (tmp.ui_config.profiles[config.profileId].pages[j] === menuConfig.get(i).page) {
                    newConfig.push(tmp.ui_config.profiles[config.profileId].pages[j]);
                    tmp.ui_config.profiles[config.profileId].pages.splice(j,1);
                    found = true;
                }
            }
        }

        // clear the config
        tmp.ui_config.profiles[config.profileId].pages  = [];

        // update the config
        tmp.ui_config.profiles[config.profileId].pages = newConfig;

        config.config = tmp;
    }

    Component.onCompleted: {
        loadmenuConfig()
        alignBackground()
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NAVIGATION ITEM
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Component {
        id: dragDelegate

        MouseArea {
            id: dragArea

            property alias dragArea: dragArea
            property bool selected: ListView.isCurrentItem
            property bool held: false

            width: content.width + 20; height: content.height + 20

            drag.target: held ? content : undefined
            drag.axis: Drag.XAxis

            onPressAndHold: {
                held = true;
                Haptic.playEffect(Haptic.Click);
                background.opacity = 0;
            }
            onReleased: {
                if (held) {
                    savemenuConfig()
                }
                Haptic.playEffect(Haptic.Click);

                held = false
                mainNavigationListView.currentIndex = index
                mainNavigationSwipeview.currentIndex = index

                alignBackground()
                backgroundOpacityChanger.start()
            }

            property ListView _listView: ListView.view

            property int scrollEdgeSize: 20
            property int _scrollingDirection: 0

            SmoothedAnimation {
                id: leftAnimation
                target: _listView
                property: "contentX"
                to: 0
                running: _scrollingDirection == -1
            }

            SmoothedAnimation {
                id: rightAnimation
                target: _listView
                property: "contentX"
                to: _listView.contentWidth - _listView.width
                running: _scrollingDirection == 1
            }

            states: [
                State {
                    when: dragArea.drag.active
                    name: "dragging"

                    PropertyChanges {
                        target: dragArea
                        _scrollingDirection: {
                            var xCoord = _listView.mapFromItem(dragArea, dragArea.mouseX, 0).x;

                            if (xCoord < scrollEdgeSize) {
                                -1;
                            } else if (xCoord > _listView.width - scrollEdgeSize) {
                                1;
                            } else {
                                0;
                            }
                        }
                    }
                }
            ]

            Rectangle {
                id: content
                width: buttonText.implicitWidth+30; height: 50
                color: Style.color.backgroundTransparent
                opacity: selected ? 1 : 0.3

                Drag.active: dragArea.held
                Drag.source: dragArea
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2

                states: State {
                    when: dragArea.held

                    ParentChange { target: content; parent: mainNavigation }
                    AnchorChanges {
                        target: content
                        anchors { horizontalCenter: undefined; verticalCenter: undefined }
                    }
                }

                Behavior on opacity {
                    NumberAnimation { duration: 200 }
                }

                Rectangle {
                    width: parent.width; height: parent.height
                    color: dragArea.held ? Style.color.highlight1 : ( selected ? Style.color.highlight2 : Style.color.backgroundTransparent )
                    radius: width / 2

                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }

                    Text {
                        id: buttonText
                        color: Style.color.text
                        text: qsTr(friendly_name) + translateHandler.emptyString
                        horizontalAlignment: Text.AlignHCenter
                        anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter; verticalCenterOffset: -5 }
                        font: Style.font.button
                        lineHeight: 0.8
                    }
                }

            }

            DropArea {
                anchors { fill: parent; margins: 10 }

                onEntered: {
                    visualModel.items.move(drag.source.DelegateModel.itemsIndex,dragArea.DelegateModel.itemsIndex)
                    menuConfig.move(drag.source.DelegateModel.itemsIndex, dragArea.DelegateModel.itemsIndex, 1)
                }
            }
        }
    }

    Rectangle {
        id: background
        width: mainNavigationListView.currentItem ? mainNavigationListView.currentItem.width-30 : 0; height: 50
        color: Style.color.highlight2
        radius: 25
        y: mainNavigationListView.currentItem ? mainNavigationListView.currentItem.y + 10 : 10

        Behavior on x {
            NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
        }
    }

    function alignBackground() {
        var newX = mapFromItem(mainNavigationListView.currentItem, 0, 0);
        background.x = newX.x
    }

    Timer {
        id: backgroundOpacityChanger
        interval: 300
        running: false
        repeat: false

        onTriggered: {
            background.opacity = 1;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN NAVIGATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias mainNavigationListView: mainNavigationListView

    ListView {
        id: mainNavigationListView
        width: parent.width-20; height: 50
        anchors.centerIn: parent
        clip: true
        boundsBehavior: Flickable.DragAndOvershootBounds
        flickableDirection: Flickable.HorizontalFlick
        orientation: ListView.Horizontal
        interactive: true
        focus: true
        highlightMoveDuration: 200
        currentIndex: 0
        spacing: 4

        model: visualModel

        onCurrentItemChanged: alignBackground()
        onContentXChanged: alignBackground()
        onFlickStarted: {
            background.opacity = 0;
            alignBackground()
        }
        onFlickEnded: {
            alignBackground()
            backgroundOpacityChanger.start()
        }
    }

    DelegateModel {
        id: visualModel
        model: menuConfig
        delegate: dragDelegate
    }
}
