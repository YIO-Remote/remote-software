import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import "qrc:/scripts/helper.js" as JSHelper
import "qrc:/components" as Comp
import "qrc:/basic_ui" as BasicUI

import MediaPlayerUtils 1.0

Comp.ButtonBase {
    id: remoteButton
    icon: "\uE903"
    cardLoader.source: "qrc:/components/media_player/ui/Card.qml"

    // override default settings
    title.anchors.verticalCenterOffset: obj.source === "" ? 0 : -15
    button.enabled: true

    // include mediaplayer utils
    MediaPlayerUtils {
        id: mediaplayerUtils
    }

    // additional UI elements
    Text {
        id: info
        color: colorText
        opacity: 0.5
        text: obj.source
        elide: Text.ElideRight
        wrapMode: Text.WordWrap
        width: title.width
        anchors.left: parent.left
        anchors.leftMargin: title.x
        anchors.top: parent.top
        anchors.topMargin: title.y + title.height
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 20
        lineHeight: 1
        visible: remoteButton.state == "closed" ? true : false
    }

    // album art
    property string m_image: obj.mediaImage
    onM_imageChanged: {
        mediaplayerUtils.imageURL = obj.mediaImage
    }

    BasicUI.CustomImageLoader {
        id: image
        visible: remoteButton.state == "closed" ? true : false
        width: 80
        height: 80
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        url: mediaplayerUtils.smallImage == "" ? "" : mediaplayerUtils.smallImage

        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: Item {
                width: image.width
                height: image.height
                Rectangle {
                    anchors.fill: parent
                    radius: cornerRadius/2
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: buttonHandler
        enabled: state == "open" ? true : false

        onButtonPress: {
        }
    }
}
