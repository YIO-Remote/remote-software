import QtQuick 2.12
import QtQuick.Controls 2.12

import MediaPlayerUtils 1.0

import "qrc:/basic_ui" as BasicUI

Item {
    id: main
    width: parent.width
    height: parent.height

    property var albumModel


    // include mediaplayer utils
    MediaPlayerUtils {
        id: mediaplayerUtils
        imageURL: albumModel.imageUrl
    }

    Rectangle {
        anchors.fill: parent
        color: mediaplayerUtils.pixelColor

        Behavior on color {
            ColorAnimation { duration: 300 }
        }
    }

    property alias itemFlickable: itemFlickable

    Flickable {
        id: itemFlickable
        width: parent.width
        height: parent.height-100
        maximumFlickVelocity: 6000
        flickDeceleration: 1000
        contentHeight: 150 + image.height + title.height + artist.height + trackListView.height
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


        BasicUI.CustomImageLoader {
            id: image
            width: 280
            height: 280
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 100
            url: albumModel.imageUrl == "" ? "qrc:/images/mini-music-player/no_image.png" : albumModel.imageUrl
        }

        Text {
            color: colorText
            text: "\uE906"
            renderType: Text.NativeRendering
            width: 70
            height: 70
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: image

            MouseArea {
                width: parent.width + 20
                height: parent.height + 20
                anchors.centerIn: parent

                onClicked: {
                    haptic.playEffect("click");
                    obj.playMedia(albumModel.id, albumModel.type);
                }
            }
        }

        Text {
            id: title
            color: colorText
            text: albumModel.title
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: parent.width-80
            font.family: "Open Sans"
            font.weight: Font.Bold
            font.styleName: "Bold"
            font.pixelSize: 30
            lineHeight: 1
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: image.bottom
            anchors.topMargin: 20
        }

        Text {
            id: artist
            color: colorText
            text: albumModel.subtitle
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            width: parent.width-80
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
            anchors.top: title.bottom
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ListView {
            id: trackListView
            width: parent.width-60
            height: childrenRect.height
            spacing: 20
            interactive: false
            anchors.top: artist.bottom
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter

            model: albumModel.model

            delegate: trackThumbnail
        }

        Component {
            id: trackThumbnail

            Item {
                width: parent.width
                height: 80

                Text {
                    id: trackNumber
                    text: index+1
                    color: colorText
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    font.family: "Open Sans"
                    font.pixelSize: 25
                    lineHeight: 1
                }

                Text {
                    id: albumTitleText
                    text: item_title
                    elide: Text.ElideRight
                    width: parent.width-100
                    wrapMode: Text.NoWrap
                    color: colorText
                    anchors.left: parent.left
                    anchors.leftMargin: 45
                    anchors.top: parent.top
                    font.family: "Open Sans"
                    font.pixelSize: 25
                    lineHeight: 1
                }

                Text {
                    id: albumSubTitleText
                    text: item_subtitle
                    elide: Text.ElideRight
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
                        obj.playMedia(item_key, item_type);
                    }
                }

                BasicUI.ContextMenuIcon {
                    colorBg: mediaplayerUtils.pixelColor
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    mouseArea.onClicked: {
                        haptic.playEffect("click");
                        contextMenuLoader.setSource("qrc:/basic_ui/ContextMenu.qml", { "width": main.width, "id": item_key, "type": item_type, "list": item_commands })
                    }
                }
            }
        }
    }
}
