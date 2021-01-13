/******************************************************************************
 *
 * Copyright (C) 2020 Michael Lörcher <MichaelLoercher@web.de>
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
//import QtQml.Models 2.2

import Haptic 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: main
    width: parent.width; height: parent.height
    color: Style.color.dark
    radius: Style.cornerRadius

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property var tvchannelListBrowseModel
    property bool isCurrentItem: parent._currentItem
    //property bool start: true


    onIsCurrentItemChanged: {
        //if (isCurrentItem && start) {
        //if (start){
        console.debug("LOAD USER EPG");
        //  start = false;
        obj.getMediaPlayerEPGView("all");
        obj.browseModelChanged.connect(onFirstLoadComplete);
        //}
        /*if (!isCurrentItem) {
            tvChannelListListView.contentY = 0-120;
        }*/
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    function onFirstLoadComplete(model) {
        main.tvchannelListBrowseModel = model.model;
        obj.browseModelChanged.disconnect(onFirstLoadComplete);
    }

    function load(tvchannel, type) {
        swipeView.currentIndex++;
        if (type === "epg") {
            obj.getMediaPlayerEPGView(tvchannel);
            obj.browseModelChanged.connect(onBrowseModelChanged);
        }
    }

    function onBrowseModelChanged(model) {
        if (channellistLoader) {
            if (channellistLoader.source != "qrc:/components/media_player/ui/EPGItemView.qml")
                channellistLoader.setSource("qrc:/components/media_player/ui/EPGItemView.qml", { "tvchannelModel": model })
            else if (channellistLoader.item) {
                channellistLoader.item.tvchannelModel = model;
                channellistLoader.item.itemFlickable.contentY = 0;
            }
        }
        obj.browseModelChanged.disconnect(onBrowseModelChanged);
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias swipeView: swipeView

    SwipeView {
        id: swipeView
        width: parent.width; height: parent.height
        opacity: 0.9
        currentIndex: 0
        interactive: false
        clip: true




        Item {
            Text {
                id: title
                color: Style.color.text
                text: qsTr("EPG List") + translateHandler.emptyString
                font { family: "Open Sans Bold"; weight: Font.Bold; pixelSize: 40 }
                lineHeight: 1
                anchors { left: parent.left; leftMargin: 30; top: parent.top; topMargin: 30 }
            }

            property alias epgItemListListView: epgItemListListView
            Flickable
            {
                anchors.top: title.bottom
                topMargin: 10

                id: fl
                width: parent.width; height: parent.height-160
                contentWidth: 2000
                contentHeight: 8000
                clip: true
                boundsBehavior: Flickable.DragAndOvershootBounds
                ScrollBar.horizontal: ScrollBar {
                    opacity: 0.5
                }
                ScrollBar.vertical: ScrollBar { opacity: 0.5
                }
                Repeater
                {
                    id:epgItemListListView
                    model: main.tvchannelListBrowseModel
                    delegate:epgItemListItem

                }
                Component {
                    id: epgItemListItem

                    Rectangle {
                        id: epgItemRectangle
                        x: 20 + item_xCoordinate
                        y: 10 + item_column * item_height
                        height: item_height
                        width: item_width
                        border.color: "#000000"
                        color: item_color
                        Text {
                            id: epgItemTitleText
                            text: item_title
                            elide: Text.ElideRight
                            width: epgItemRectangle.width
                            wrapMode: Text.NoWrap
                            color: "#000000"
                            anchors { left: parent.left; leftMargin: 0; verticalCenter: parent.verticalCenter}
                            font { family: "Open Sans Regular"; pixelSize: 14; bold: true }
                        }
                        MouseArea {
                            anchors.fill: parent

                            onClicked: {
                                Haptic.playEffect(Haptic.Click);
                                load(item_key, item_type);
                            }
                        }
                        BasicUI.ContextMenuIcon {
                            anchors { right: epgItemRectangle.right; verticalCenter: parent.verticalCenter }
                            width: 10
                            height: 10
                            mouseArea.onClicked: {
                                Haptic.playEffect(Haptic.Click);
                                contextMenuLoader.setSource("qrc:/basic_ui/ContextMenu.qml", { "width": itemFlickable.width, "id": item_key, "type": item_type, "list": item_commands })
                            }
                        }
                    }
                }
            }
        }

        Item {

            Loader {
                id: channellistLoader
                asynchronous: true
                anchors.fill: parent
            }

            Text {
                id: backButton
                color: Style.color.text
                text: Style.icon.left_arrow
                renderType: Text.NativeRendering
                width: 70; height: 70
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font {family: "icons"; pixelSize: 80 }
                anchors { left: parent.left; leftMargin: 10; top: parent.top; topMargin: 20 }

                MouseArea {
                    id: backButtonMouseArea
                    width: parent.width + 20; height: parent.height + 20
                    anchors.centerIn: parent

                    onClicked: {
                        Haptic.playEffect(Haptic.Click);
                        swipeView.currentIndex = 0;
                    }
                }
            }
        }
    }
}
