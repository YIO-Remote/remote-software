import QtQuick 2.12
import QtQuick.Controls 2.12

import "qrc:/basic_ui" as BasicUI

Rectangle {
    width: parent.width
    height: parent.height
    color: colorDark

    property var playListModel

    Component.onCompleted: {
        console.debug("PLAYLIST COMPLETED");
        obj.getPlaylist("user");
        obj.browseModelChanged.connect(onBrowseModelChanged);
    }

    function onBrowseModelChanged(model) {
        if (model.count != 0) {
            playListModel = model.model;
            obj.browseModelChanged.disconnect(onBrowseModelChanged);
        }
    }

    function load(album, type) {
        swipeView.currentIndex++;
        if (type === "playlist") {
            obj.getPlaylist(album);
            obj.browseModelChanged.connect(function(model) {
                if (model.count != 0 && playlistLoader) {
                    if (playlistLoader.source != "qrc:/components/media_player/ui/AlbumView.qml")
                        playlistLoader.setSource("qrc:/components/media_player/ui/AlbumView.qml", { "albumModel": model })
                    else if (playlistLoader.item)
                        playlistLoader.item.albumModel = model;
                }
            });
        }
    }

    property alias swipeView: swipeView

    SwipeView {
        id: swipeView
        width: parent.width
        height: parent.height
        currentIndex: 0
        interactive: false
        clip: true

        Item {

            Flickable {
                id: itemFlickable
                width: parent.width
                height: parent.height-100
                maximumFlickVelocity: 6000
                flickDeceleration: 1000
                contentHeight: 30 + title.height + 40 + playListListView.height
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

                Text {
                    id: title
                    color: colorText
                    text: qsTr("My playlists") + translateHandler.emptyString
                    anchors.left: parent.left
                    anchors.leftMargin: 30
                    anchors.top: parent.top
                    anchors.topMargin: 30
                    font.family: "Open Sans Bold"
                    font.pixelSize: 40
                    lineHeight: 1
                }

                property alias playListListView: playListListView

                ListView {
                    id: playListListView
                    width: parent.width-60
                    height: childrenRect.height
                    spacing: 20
                    anchors.top: title.bottom
                    anchors.topMargin: 40
                    anchors.horizontalCenter: parent.horizontalCenter
                    interactive: false

                    delegate: playListThumbnail
                    model: playListModel
                }


                Component {
                    id: playListThumbnail

                    Item {
                        id: trackThumbnailItem
                        width: parent.width
                        height: 80 //childrenRect.height

                        Rectangle {
                            id: albumImage
                            width: 80
                            height: 80

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
                            color: colorText
                            anchors.left: albumImage.right
                            anchors.leftMargin: 20
                            anchors.top: albumImage.top
                            anchors.topMargin: item_subtitle == "" ? 26 : 12
                            font.family: "Open Sans"
                            font.pixelSize: 25
                            lineHeight: 1
                        }

                        Text {
                            id: albumSubTitleText
                            text: item_subtitle
                            elide: Text.ElideRight
                            visible: item_subtitle == "" ? false : true
                            width: albumTitleText.width
                            wrapMode: Text.NoWrap
                            color: colorText
                            opacity: 0.6
                            anchors.left: albumTitleText.left
                            anchors.top: albumTitleText.bottom
                            anchors.topMargin: 5
                            font.family: "Open Sans"
                            font.pixelSize: 20
                            lineHeight: 1
                        }

                        MouseArea {
                            anchors.fill: parent

                            onClicked: {
                                haptic.playEffect("click");
                                load(item_key, item_type);
                            }
                        }

                        BasicUI.ContextMenuIcon {
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter

                            mouseArea.onClicked: {
                                haptic.playEffect("click");
                                contextMenuLoader.setSource("qrc:/basic_ui/ContextMenu.qml", { "width": itemFlickable.width, "id": item_key, "type": item_type, "list": item_commands })
                            }
                        }
                    }
                }
            }
        }

        Item {

            Loader {
                id: playlistLoader
                asynchronous: true
                anchors.fill: parent
            }

            Text {
                id: backButton
                color: colorText
                text: "\uE917"
                renderType: Text.NativeRendering
                width: 70
                height: 70
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font {family: "icons"; pixelSize: 80 }
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 20

                MouseArea {
                    id: backButtonMouseArea
                    width: parent.width + 20
                    height: parent.height + 20
                    anchors.centerIn: parent

                    onClicked: {
                        haptic.playEffect("click");
                        swipeView.currentIndex = 0;
                    }
                }
            }

        }

    }
}
