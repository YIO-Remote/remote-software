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


import Haptic 1.0

import "qrc:/basic_ui" as BasicUI
import "qrc:/components/remote/ui/ir"

Rectangle {
    id: main
    width: parent.width; height: parent.height
    color: Style.color.dark
    radius: Style.cornerRadius

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*property var tvchannelListBrowseModel
    property bool isCurrentItem: parent._currentItem
    property bool start: true*/

    /*onIsCurrentItemChanged: {
        if (isCurrentItem && start) {
            console.debug("LOAD USER TVPLAYLISTS");
            start = false;
            obj.getTVChannelList("all");
            obj.browseModelChanged.connect(onFirstLoadComplete);
        }
        if (!isCurrentItem) {
            tvChannelListListView.contentY = 0-120;
        }
    }*/


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    function onTextInput() {
        obj.textInputMediaPlayer(textInputTextField.text);
        textInputTextField.focus = false;
        textInputTextField.text = "";
    }
    /*function onFirstLoadComplete(model) {
        main.tvchannelListBrowseModel = model.model;
        obj.browseModelChanged.disconnect(onFirstLoadComplete);
    }

    function load(tvchannel, type) {
        swipeView.currentIndex++;
        if (type === "tvchannellist") {
            obj.getTVChannelList(tvchannel);
            obj.browseModelChanged.connect(onBrowseModelChanged);
        }
    }

   /* function onBrowseModelChanged(model) {
        if (channellistLoader) {
            if (channellistLoader.source != "qrc:/components/media_player/ui/MediaPlayerRemoteView.qml")
                channellistLoader.setSource("qrc:/components/media_player/ui/MediaPlayerRemoteView.qml", { "tvchannelModel": model })
            else if (channellistLoader.item) {
                channellistLoader.item.tvchannelModel = model;
                channellistLoader.item.itemFlickable.contentY = 0;
            }
        }
        obj.browseModelChanged.disconnect(onBrowseModelChanged);
    }*/


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //property alias swipeView: swipeView

    /*   SwipeView {
        id: swipeView
        width: parent.width; height: parent.height
        currentIndex: 0
        interactive: false
        clip: true*/

    //Item {

    /*Rectangle {
            id: powerButton
            visible: true
            width: 118; height: 60
            radius: height/2
            color: Style.color.red
            anchors { top: parent.top; left: parent.left }

            Text {
                color: Style.color.line
                text: Style.icon.power_on
                width: 70; height: 70
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font {family: "icons"; pixelSize: 60 }
                anchors.centerIn: parent
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    //if (obj.isSupported(Remote.F_POWER_TOGGLE)) {
                    obj.play();
                    /*} else if (obj.isSupported(Remote.F_POWER_ON) && obj.isSupported(Remote.F_POWER_OFF)) {
                            if (obj.isOn)
                                obj.powerOff();
                            else
                                obj.powerOn();
                        }
                }
            }
        }*/



    Rectangle {
        id: commandContainer
        width: 300; height: 80
        anchors { top: parent.top; topMargin: 100; horizontalCenter: parent.horizontalCenter }
        color: Style.color.text
        radius: Style.cornerRadius


        Text {
            id: textInputIcon
            color: Style.color.background
            text: Style.icon.down_arrow_bold
            renderType: Text.NativeRendering
            width: 70; height: 70
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors { left: parent.left; leftMargin: 10; verticalCenter: parent.verticalCenter }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    onTextInput();
                }
            }
        }

        TextField {
            id: textInputTextField
            cursorVisible: false
            width: parent.width-90; height: parent.height
            anchors { left: textInputIcon.right; leftMargin: 10; verticalCenter: parent.verticalCenter }
            placeholderText: "Textinput" + translateHandler.emptyString
            color: Style.color.background
            font { family: "Open Sans Regular"; pixelSize: 27 }

            background: Rectangle {
                color: Style.color.backgroundTransparent
                border.width: 0
            }

            onAccepted: {
                onTextInput();
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
    /*
        Grid {
            id: cursorField
            width: parent.width
            visible: true
            columns: 3
            anchors { top: searchContainer.bottom; topMargin: 30; horizontalCenter: parent.horizontalCenter }

            Text {
                text: " "
            }
            Button {
                visible: true//obj.isSupported(Remote.F_DIGIT_1)
                title: "▲"
                mouseArea.onClicked: { obj.channel(1); }
            }
            Text {
                text: " "
            }
            Button {
                visible: true//obj.isSupported(Remote.F_DIGIT_1)
                title: "◄"
                mouseArea.onClicked: { obj.channel(1); }
            }
            Button {
                visible: true//obj.isSupported(Remote.F_DIGIT_1)
                title: "OK"
                mouseArea.onClicked: { obj.channel(1); }
            }
            Button {
                visible: true//obj.isSupported(Remote.F_DIGIT_1)
                title: "►"
                mouseArea.onClicked: { obj.channel(1); }
            }
            Text {
                text: " "
            }
            Button {
                visible: true//obj.isSupported(Remote.F_DIGIT_1)
                title: "▼"
                mouseArea.onClicked: { obj.channel(1); }
            }
            Text {
                text: " "
            }
        }*/
    /*Rectangle {
            id: searchContainer
            width: parent.width - 60; height: 80
            anchors { top: cursorField.bottom; topMargin: 100; horizontalCenter: cursorField.horizontalCenter }
            //color: Style.color.text
            //radius: Style.cornerRadius
            TextField {
                id: searchTextField
                cursorVisible: true
                width: parent.width-90; height: parent.height
            }
            */
    /*Text {
                id: searchIcon
                color: Style.color.background
                text: Style.icon.search
                renderType: Text.NativeRendering
                width: 70; height: 70
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font {family: "icons"; pixelSize: 80 }
                anchors { top: cursorField.bottom; left: parent.left; leftMargin: 10; }

                /*MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        Haptic.playEffect(Haptic.Click);
                        onSearch();
                    }
                }*/
    //}

    /*TextField {

            cursorVisible: true
            width: parent.width-90; height: parent.height
            anchors { top: cursorField.bottom; left: searchIcon.right; leftMargin: 10;}
            //placeholderText: qsTr("Search") + translateHandler.emptyString
            //color: Style.color.background
            font { family: "Open Sans Regular"; pixelSize: 27 }

            /*background: Rectangle {
                    color: Style.color.backgroundTransparent
                    border.width: 0
                }*/

    /*onAccepted: {
                    onSearch();
                }

                onFocusChanged: {
                    if (focus) {
                        inputPanel.active = true
                        itemFlickable.contentY = 0;
                    } else
                        inputPanel.active = false
                }*/
    //  }
    // }


    //}

    /*property alias tvChannelListListView: tvChannelListListView
            ListView {
                id: tvChannelListListView
                width: parent.width; height: parent.height-100
                spacing: 20
                anchors { top: parent.top; horizontalCenter: parent.horizontalCenter }
                maximumFlickVelocity: 6000
                flickDeceleration: 1000
                boundsBehavior: Flickable.DragAndOvershootBounds
                flickableDirection: Flickable.VerticalFlick
                clip: true
                cacheBuffer: 3000

                delegate: playListThumbnail
                model: main.tvchannelListBrowseModel

                ScrollBar.vertical: ScrollBar {
                    opacity: 0.5
                }

                header: Component {
                    Item {
                        width: parent.width; height: 120

                        Text {
                            id: title
                            color: Style.color.text
                            text: "Remote"//qsTr("My playlists") + translateHandler.emptyString
                            font { family: "Open Sans Bold"; weight: Font.Bold; pixelSize: 40 }
                            lineHeight: 1
                            anchors { left: parent.left; leftMargin: 30; top: parent.top; topMargin: 30 }
                        }
                    }
                }

                populate: Transition {
                    id: popTransition
                    SequentialAnimation {
                        PropertyAction { property: "opacity"; value: 0 }
                        PauseAnimation { duration: popTransition.ViewTransition.index*100 }
                        NumberAnimation { properties: "opacity"; from: 0; to: 1; duration: 300; easing.type: Easing.InExpo }
                    }
                }
            }*/

    /*Component {
                id: playListThumbnail

                Item {
                    id: channelThumbnailItem
                    width: parent.width-60; height: 80
                    anchors.horizontalCenter: parent.horizontalCenter

                    Rectangle {
                        id: tvchannelImage
                        width: 80; height: 80

                        Image {
                            source: item_image
                            anchors.fill: parent
                            fillMode: Image.PreserveAspectCrop
                            asynchronous: true
                        }
                    }

                    Text {
                        id: tvchannelTimeText
                        text: item_time
                        elide: Text.ElideRight
                        width: itemFlickable.width-60-tvchannelImage.width-20-80
                        wrapMode: Text.NoWrap
                        color: Style.color.text
                        anchors { left: tvchannelImage.right; leftMargin: 20; top: tvchannelImage.top; topMargin: item_subtitle == "" ? 26 : 12 }
                        font { family: "Open Sans Regular"; pixelSize: 25 }
                        lineHeight: 1
                    }
                    Text {
                        id: tvchannelTitleText
                        text: item_title
                        elide: Text.ElideRight
                        width: itemFlickable.width-60-tvchannelImage.width-20-80
                        wrapMode: Text.NoWrap
                        color: Style.color.text
                        anchors { left: tvchannelImage.right; leftMargin: 20; top: tvchannelImage.top; topMargin: item_subtitle == "" ? 26 : 12 }
                        font { family: "Open Sans Regular"; pixelSize: 25 }
                        lineHeight: 1
                    }

                    Text {
                        id: tvchannelSubTitleText
                        text: item_subtitle
                        elide: Text.ElideRight
                        visible: item_subtitle == "" ? false : true
                        width: tvchannelTitleText.width
                        wrapMode: Text.NoWrap
                        color: Style.color.text
                        opacity: 0.6
                        anchors { left: tvchannelTitleText.left; top: tvchannelTitleText.bottom; topMargin: 5 }
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

                    BasicUI.ContextMenuIcon {
                        anchors { right: parent.right; verticalCenter: parent.verticalCenter }

                        mouseArea.onClicked: {
                            Haptic.playEffect(Haptic.Click);
                            contextMenuLoader.setSource("qrc:/basic_ui/ContextMenu.qml", { "width": itemFlickable.width, "id": item_key, "type": item_type, "list": item_commands })
                        }
                    }
                }
            }*/
    /*}

/* Item {

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
        }*/
    //}
}
//}
