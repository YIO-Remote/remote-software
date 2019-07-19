import QtQuick 2.12
import QtQuick.Controls 2.4

Item {
    width: 480
    height: 100

    property var obj
    property var players: []
    property var mainNav

    property alias mediaImage: image.source

    onObjChanged: {
        players.push(obj);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Connections {
        target: buttonHandler
        enabled: loader_main.state === "visible" && standbyControl.mode === "on" ? true : false

        onButtonPress: {
            switch (button) {
            case "volume up":
                if (volume.state != "visible") {
                    volume.volumePosition = obj.volume;
                    volume.state = "visible";
                }
                var newvolume = obj.volume + 0.02;
                obj.setVolume(newvolume);
                volume.volumePosition = newvolume;
                break;
            case "volume down":
                if (volume.state != "visible") {
                    volume.volumePosition = obj.volume;
                    volume.state = "visible";
                }
                var newvolume = obj.volume - 0.02;
                obj.setVolume(newvolume);
                volume.volumePosition = newvolume;
                break;
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: colorBackground
    }

    SwipeView {
        id: mediaPlayers
        anchors.fill: parent

        Repeater {
            model: players
        }

    }

    Image {
        id: image
        width: 60
        height: 60
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: 15
        }
        fillMode: Image.PreserveAspectFit
        asynchronous: true
        source: obj.mediaImage == "" ? "qrc:/images/mini-music-player/no_image.png" : obj.mediaImage
    }

    Item {
        height: childrenRect.height

        anchors.left: image.right
        anchors.leftMargin: 20
        anchors.verticalCenter: image.verticalCenter

        Text {
            id: title
            color: colorText
            text: obj.mediaTitle
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            width: 304
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 25
            lineHeight: 1
        }

        Text {
            id: artist
            color: colorText
            text: obj.mediaArtist
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            width: 304
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
            anchors.top: title.bottom
            anchors.topMargin: -5
            opacity: 0.6
        }
    }

    Rectangle {
        width: parent.width - 30
        height: 2
        color: colorLight
        opacity: (800-mainNav.y)/mainNav.height

        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    MouseArea {
        anchors.fill: parent
        propagateComposedEvents: true

        onClicked: {
            console.debug("OPEN mini player");
        }
    }
}
