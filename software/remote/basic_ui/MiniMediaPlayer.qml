import QtQuick 2.11
import QtQuick.Controls 2.4

Item {
    width: 480
    height: 100

    property var players: []
    property var mainNav

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO MEDIA PLAYERS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Component.onCompleted: {
        var e = entities.getByType("media_player");
        for (var i=0; i<e.length; i++) {
            if (e[i].state == 2 || e[i].state == 3) {
                players.push(e[i]);
            }

            e[i].playing.connect(handlePlay);
            e[i].stopped.connect(handleStop);
        }
        var tmp = players;
        players = tmp;
    }

    function handlePlay(entity) {
        for (var i=0; i<players.length; i++) {
            if (players[i] != entities.get(entity)) {
                players.push(entities.get(entity));
            }
        }
        var tmp = players;
        players = tmp;
    }

    function handleStop(entity) {
        for (var i=0; i<players.length; i++) {
            if (players[i] == entities.get(entity)) {
                players.splice(i, 1);
            }
        }
        var tmp = players;
        players = tmp;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Connections {
        target: buttonHandler

        onButtonPress: {
            switch (button) {
            case "volume up":
                if (volume.state != "visible") {
                    volume.volumePosition = mediaPlayers.currentItem.player.obj.volume;
                    volume.state = "visible";
                }
                var newvolume = mediaPlayers.currentItem.player.obj.volume + 0.02;
                mediaPlayers.currentItem.player.obj.setVolume(newvolume);
                volume.volumePosition = newvolume;
                break;
            case "volume down":
                if (volume.state != "visible") {
                    volume.volumePosition = mediaPlayers.currentItem.player.obj.volume;
                    volume.state = "visible";
                }
                var newvolume = mediaPlayers.currentItem.player.obj.volume - 0.02;
                mediaPlayers.currentItem.player.obj.setVolume(newvolume);
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
            model: players.length

            Item {
                id: player
                property alias player: player

                property var obj: players[index]

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
                    source: players[index].mediaImage == "" ? "qrc:/images/mini-music-player/no_image.png" : players[index].mediaImage
                }

                Item {
                    height: childrenRect.height

                    anchors.left: image.right
                    anchors.leftMargin: 20
                    anchors.verticalCenter: image.verticalCenter

                    Text {
                        id: title
                        color: colorText
                        text: players[index].mediaTitle
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
                        text: players[index].friendly_name
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap
                        width: 304
                        font.family: "Open Sans"
                        font.weight: Font.Normal
                        font.pixelSize: 20
                        lineHeight: 1
                        anchors.top: title.bottom
                        anchors.topMargin: -2
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
            }
        }

    }

    //    MouseArea {
    //        anchors.fill: parent
    //        propagateComposedEvents: true

    //        onClicked: {
    //            console.debug("OPEN mini player");
    //        }
    //    }
}
