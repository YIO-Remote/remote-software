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

import "qrc:/basic_ui" as BasicUI

Rectangle {
    width: parent.width; height: parent.height
    color: Style.colorDark
    radius: Style.cornerRadius

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property bool isCurrentItem: parent._currentItem

    onIsCurrentItemChanged: {
        if (!isCurrentItem) {
            searchResults.visible = false;
            itemFlickable.contentY = 0;
            if (obj.searchModel)
                obj.searchModel.clear();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    function onSearch() {
        obj.search(searchTextField.text);
        searchResultsTitle.text = searchTextField.text;
        obj.recentSearches.insert(0, {"searchString":searchTextField.text});
        searchTextField.focus = false;
        searchTextField.text = "";
        searchResults.visible = true;
        itemFlickable.contentY = 290 + recentSearches.height;
    }

    function load(album, type) {
        swipeView.currentIndex++;
        if (type === "album") {
            obj.getAlbum(album);
            obj.browseModelChanged.connect(onBrowseModelChanged);
        } else if (type === "playlist") {
            obj.getPlaylist(album);
            obj.browseModelChanged.connect(onBrowseModelChanged);
        }
    }

    function onBrowseModelChanged(model) {
        if (model.count !== 0 && albumLoader) {
            if (albumLoader.source != "qrc:/components/media_player/ui/AlbumView.qml")
                albumLoader.setSource("qrc:/components/media_player/ui/AlbumView.qml", { "albumModel": model })
            else if (albumLoader.item)
                albumLoader.item.albumModel = model;
        }
        obj.browseModelChanged.disconnect(onBrowseModelChanged);
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias albumLoader: albumLoader
    property alias swipeView: swipeView

    SwipeView {
        id: swipeView
        width: parent.width; height: parent.height
        currentIndex: 0
        interactive: false
        clip: true

        Item {
            Flickable {
                id: itemFlickable
                width: parent.width; height: parent.height-100
                maximumFlickVelocity: 6000
                flickDeceleration: 1000
                contentHeight: 260 + searchContainer.height + recentSearches.height + searchResults.height
                boundsBehavior: Flickable.DragAndOvershootBounds
                flickableDirection: Flickable.VerticalFlick
                clip: true

                Behavior on contentY {
                    PropertyAnimation {
                        duration: 300
                        easing.type: Easing.OutExpo
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    opacity: 0.5
                }

                Connections {
                    target: card
                    onLooseFocus: {
                        searchTextField.focus = false;
                    }
                }

                Text {
                    id: title
                    color: Style.colorText
                    text: qsTr("Search") + translateHandler.emptyString
                    anchors { left: parent.left; leftMargin: 30; top: parent.top; topMargin: 30 }
                    font { family: "Open Sans Bold"; pixelSize: 40 }
                    lineHeight: 1
                }

                Rectangle {
                    id: searchContainer
                    width: parent.width - 60; height: 80
                    anchors { top: parent.top; topMargin: 100; horizontalCenter: parent.horizontalCenter }
                    color: Style.colorText
                    radius: Style.cornerRadius

                    Text {
                        id: searchIcon
                        color: Style.colorBackground
                        text: Style.icons.search
                        renderType: Text.NativeRendering
                        width: 70; height: 70
                        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                        font {family: "icons"; pixelSize: 80 }
                        anchors { left: parent.left; leftMargin: 10; verticalCenter: parent.verticalCenter }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                Haptic.playEffect(Haptic.Click);
                                onSearch();
                            }
                        }
                    }

                    TextField {
                        id: searchTextField
                        cursorVisible: false
                        width: parent.width-90; height: parent.height
                        anchors { left: searchIcon.right; leftMargin: 10; verticalCenter: parent.verticalCenter }
                        placeholderText: qsTr("Search") + translateHandler.emptyString
                        color: Style.colorBackground
                        font { family: "Open Sans Regular"; pixelSize: 27 }

                        background: Rectangle {
                            color: Style.colorBackgroundTransparent
                            border.width: 0
                        }

                        onAccepted: {
                            onSearch();
                        }

                        onFocusChanged: {
                            if (focus) {
                                inputPanel.active = true
                                itemFlickable.contentY = 0;
                            } else
                                inputPanel.active = false
                        }
                    }
                }

                Item {
                    id: recentSearches
                    width: parent.width - 60; height: childrenRect.height
                    anchors { top: searchContainer.bottom; topMargin: 40; horizontalCenter: parent.horizontalCenter }
                    state: obj.recentSearches.count === 0 ? "hidden" : "visible"

                    states:[
                        State {
                            name: "hidden"
                            PropertyChanges { target: recentSearches; opacity: 0; height: 0; visible: false }
                        },
                        State {
                            name: "visible"
                            PropertyChanges { target: recentSearches; opacity: 1; height: childrenRect.height; visible: true }
                        }
                    ]

                    transitions: [
                        Transition {
                            to: "hidden"
                            SequentialAnimation {
                                PropertyAnimation { target: recentSearches; properties: "opacity, height"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: recentSearches; properties: "visible"; duration: 1 }
                            }
                        },
                        Transition {
                            to: "visible"
                            SequentialAnimation {
                                PropertyAnimation { target: recentSearches; properties: "visible"; duration: 1 }
                                PropertyAnimation { target: recentSearches; properties: "opacity, height"; easing.type: Easing.OutExpo; duration: 300 }
                            }
                        }
                    ]

                    Text {
                        id: recentSearchesTitle
                        color: Style.colorText
                        text: qsTr("Recent searches") + translateHandler.emptyString
                        anchors { left: parent.left; top: parent.top }
                        font: Style.buttonFont
                    }

                    Text {
                        color: Style.colorText
                        opacity: 0.5
                        text: qsTr("Clear") + translateHandler.emptyString
                        anchors { right: parent.right; top: parent.top }
                        font: Style.buttonFont

                        visible: obj.recentSearches.count > 0 ? true : false
                        enabled: visible

                        MouseArea {
                            width: parent.width+20; height: parent.height+20

                            onClicked: {
                                Haptic.playEffect(Haptic.Click);
                                obj.recentSearches.clear();
                            }
                        }
                    }

                    ListView {
                        model: obj.recentSearches
                        anchors { top: recentSearchesTitle.bottom; topMargin: 40 }
                        height: childrenRect.height
                        interactive: false

                        delegate: Item {
                            width: recentSearches.width; height: 60

                            Text {
                                color: Style.colorText
                                text: searchString
                                font { family: "Open Sans Regular"; pixelSize: 25 }
                                lineHeight: 1
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    Haptic.playEffect(Haptic.Click);
                                    obj.search(searchString);
                                    searchResultsTitle.text = searchString;
                                    itemFlickable.contentY = 230 + recentSearches.height;
                                    searchResults.visible = true;
                                }
                            }

                        }

                        add: Transition {
                            PropertyAnimation { properties: "opacity"; from: 0; to: 1; duration: 400; easing.type: Easing.OutExpo }
                        }

                        displaced: Transition {
                            PropertyAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBounce }
                        }
                    }
                }

                Item {
                    id: searchResults
                    visible: false
                    width: parent.width-60; height: childrenRect.height
                    anchors { top: recentSearches.bottom; topMargin: 40; horizontalCenter: parent.horizontalCenter }

                    Text {
                        id: searchResultsTitle
                        color: Style.colorText
                        anchors { left: parent.left; top: parent.top }
                        font { family: "Open Sans Bold"; pixelSize: 30 }
                        lineHeight: 1
                    }

                    Flow {
                        id: tags
                        width: parent.width
                        anchors { top: searchResultsTitle.bottom; topMargin: 30 }
                        visible: searchResultsList.count != 0 ? true : false
                        spacing: 20

                        property var modelHeights: ({})

                        BasicUI.Tag {
                            id: tagAll
                            tag: "All"
                            selected: {
                                var selectedItem = true;
                                for (var i=0; i<tagRepeater.count; i++) {
                                    if (tagRepeater.itemAt(i) && !tagRepeater.itemAt(i).selected) {
                                        selectedItem = false;
                                    }
                                }
                                return selectedItem;
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (tagAll.selected) {
                                        for (var i=0; i<tagRepeater.count; i++) {
                                            tagRepeater.itemAt(i).selected = false;
                                        }
                                    } else {
                                        for (i=0; i<tagRepeater.count; i++) {
                                            tagRepeater.itemAt(i).selected = true;
                                        }
                                    }
                                }
                            }
                        }

                        Repeater {
                            id: tagRepeater
                            model: obj.searchModel

                            BasicUI.Tag {
                                id: tagItem
                                tag: {
                                    if (title === "albums")
                                        return qsTr("Albums") + translateHandler.emptyString
                                    else if (title === "tracks")
                                        return qsTr("Tracks") + translateHandler.emptyString
                                    else if (title === "artists")
                                        return qsTr("Artists") + translateHandler.emptyString
                                    else if (title === "playlists")
                                        return qsTr("Playlists") + translateHandler.emptyString
                                }
                                selected: true

                                onSelectedChanged: {
                                    if (!selected) {
                                        searchResultsList.currentIndex = index;

                                        tags.modelHeights[index] = searchResultsList.currentItem.height;

                                        searchResultsList.currentItem.visible = false;
                                        searchResultsList.currentItem.height = 0 - searchResultsList.spacing;
                                    } else {
                                        searchResultsList.currentIndex = index;

                                        searchResultsList.currentItem.height = tags.modelHeights[index];
                                        searchResultsList.currentItem.visible = true;
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        if (selected) {
                                            selected = false;
                                        } else {
                                            selected = true;
                                        }
                                    }
                                }
                            }
                        }

                    }

                    ListView {
                        id: searchResultsList
                        width: parent.width; height: childrenRect.height
                        anchors { top: tags.bottom; topMargin: 40 }
                        interactive: false
                        spacing: 40

                        model: obj.searchModel

                        delegate:
                            Item {
                            width: parent.width; height: childrenRect.height

                            // Title
                            Text {
                                id: searchResultsListHeader
                                text: {
                                    if (title === "albums")
                                        return qsTr("Albums") + translateHandler.emptyString
                                    else if (title === "tracks")
                                        return qsTr("Tracks") + translateHandler.emptyString
                                    else if (title === "artists")
                                        return qsTr("Artists") + translateHandler.emptyString
                                    else if (title === "playlists")
                                        return qsTr("Playlists") + translateHandler.emptyString
                                }
                                color: Style.colorText
                                anchors { left: parent.left; top: parent.top }
                                font { family: "Open Sans Regular"; pixelSize: 40 }
                                lineHeight: 1
                            }

                            // Listview
                            Loader {
                                id: listViewLoader
                                anchors { top: searchResultsListHeader.bottom; topMargin: 40; horizontalCenter: parent.horizontalCenter }
                                sourceComponent: {
                                    if (title === "albums")
                                        return albumList
                                    else if (title === "tracks")
                                        return trackList
                                    else if (title === "artists")
                                        return trackList
                                    else if (title === "playlists")
                                        return albumList
                                }

                                onStatusChanged: if (listViewLoader.status == Loader.Ready && listViewLoader.item) {
                                                     item.model = item_data;
                                                 }
                            }
                        }
                    }
                }

                Component {
                    id: albumList

                    ListView {
                        id: albumListView
                        width: itemFlickable.width-60; height: 260
                        orientation: ListView.Horizontal
                        spacing: 30
                        clip: true

                        delegate: albumThumbnail

                        populate: Transition {
                            id: popTransition
                            SequentialAnimation {
                                PropertyAction { property: "opacity"; value: 0 }
                                PauseAnimation { duration: popTransition.ViewTransition.index*100 }
                                NumberAnimation { properties: "opacity"; from: 0; to: 1; duration: 300; easing.type: Easing.InExpo }
                            }
                        }
                    }
                }

                Component {
                    id: albumThumbnail

                    Item {
                        width: childrenRect.width; height: childrenRect.height

                        Rectangle {
                            id: albumImage
                            width: 160; height: 160
                            color: Style.colorBackground

                            Image {
                                source: item_image
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                            }
                        }

                        Text {
                            id: albumTitleText
                            text: item_title
                            elide: Text.ElideRight
                            width: albumImage.width
                            wrapMode: Text.NoWrap
                            color: Style.colorText
                            anchors { left: parent.left; top: albumImage.bottom; topMargin: 20 }
                            font { family: "Open Sans Regular"; pixelSize: 25 }
                            lineHeight: 1
                        }

                        Text {
                            id: albumSubTitleText
                            text: item_subtitle
                            elide: Text.ElideRight
                            width: albumImage.width
                            wrapMode: Text.NoWrap
                            color: Style.colorText
                            opacity: 0.6
                            anchors { left: parent.left; top: albumTitleText.bottom; topMargin: 5 }
                            font { family: "Open Sans Regular"; pixelSize: 20 }
                            lineHeight: 1
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                Haptic.playEffect(Haptic.Click);
                                load(item_key, item_type);
                            }
                        }
                    }
                }

                Component {
                    id: trackList

                    ListView {
                        id: trackListView
                        width: itemFlickable.width-60; height: childrenRect.height
                        spacing: 20
                        interactive: false
                        delegate: trackThumbnail
                    }
                }

                Component {
                    id: trackThumbnail

                    Item {
                        id: trackThumbnailItem
                        width: parent.width; height: 80

                        Rectangle {
                            id: albumImage
                            width: 80; height: 80

                            Image {
                                source: item_image
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                            }
                        }

                        Text {
                            id: albumTitleText
                            text: item_title
                            elide: Text.ElideRight
                            width: itemFlickable.width-60-albumImage.width-20-60
                            wrapMode: Text.NoWrap
                            color: Style.colorText
                            anchors { left: albumImage.right; leftMargin: 20; top: albumImage.top; topMargin: item_subtitle == "" ? 26 : 12 }
                            font { family: "Open Sans Regular"; pixelSize: 25 }
                            lineHeight: 1
                        }

                        Text {
                            id: albumSubTitleText
                            text: item_subtitle
                            elide: Text.ElideRight
                            visible: item_subtitle == "" ? false : true
                            width: albumTitleText.width
                            wrapMode: Text.NoWrap
                            color: Style.colorText
                            opacity: 0.6
                            anchors { left: albumTitleText.left; top: albumTitleText.bottom; topMargin: 5 }
                            font { family: "Open Sans Regular"; pixelSize: 20 }
                            lineHeight: 1
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                Haptic.playEffect(Haptic.Click);
                                obj.playMedia(item_key, item_type);
                            }
                        }

                        BasicUI.ContextMenuIcon {
                            anchors { right: parent.right; verticalCenter: parent.verticalCenter }

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
                id: albumLoader
                asynchronous: true
                anchors.fill: parent
            }

            Text {
                id: backButton
                color: Style.colorText
                text: Style.icons.left_arrow
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
