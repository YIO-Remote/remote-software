import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

Item {
    id: miniMediaPlayer
    width: 480
    height: 100

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"


    states: [
        State { name: "closed";
            PropertyChanges {target: miniMediaPlayer; y: 0; height: 100 }
            ParentChange { target: miniMediaPlayer; parent: loader_main.item.miniMediaPlayer }
            PropertyChanges {target: loader_main; state: "visible" }
        },
        State { name: "open";
            PropertyChanges {target: miniMediaPlayer; y: 130; height: 670 }
            ParentChange { target: miniMediaPlayer; parent: contentWrapper }
            PropertyChanges {target: loader_main; state: "hidden" }
        }
    ]
    transitions: [
        Transition {to: "closed";
            PropertyAnimation { target: miniMediaPlayer; properties: "y, height"; easing.type: Easing.OutExpo; duration: 500 }
            ParentAnimation {
                NumberAnimation { properties: "y"; easing.type: Easing.OutExpo; duration: 500 }
            }
        },
        Transition {to: "open";
            PropertyAnimation { target: miniMediaPlayer; properties: "y, height"; easing.type: Easing.OutExpo; duration: 500 }
            ParentAnimation {
                NumberAnimation { properties: "y"; easing.type: Easing.OutExpo; duration: 500 }
            }
        }
    ]


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    layer.enabled: true
    layer.effect: OpacityMask {
        maskSource:
            Rectangle {
            id: opacityMask
            width: miniMediaPlayer.width
            height: miniMediaPlayer.height
            radius: miniMediaPlayer.state == "closed" ? 0 : cornerRadius
        }
    }

    property var players: []
    property int currPlaying: 0
    property var mainNav

    onCurrPlayingChanged: {
        if (currPlaying == 0) {
            loader_main.item.miniMediaPlayer.height = 0;
            loader_main.item.miniMediaPlayer.miniMediaPlayerLoader.source = "";
            loader_main.item.miniMediaPlayer.miniMediaPlayerLoader.active = false;
            players = [];
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO MEDIA PLAYERS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Component.onCompleted: {
        var e = entities.getByType("media_player");
        for (var i=0; i<e.length; i++) {
            if (e[i].state == 2 || e[i].state == 3) {
                players.push(e[i]);
                currPlaying++;
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
                currPlaying++;
            }
        }
        var tmp = players;
        players = tmp;
    }

    function handleStop(entity) {
        for (var i=0; i<players.length; i++) {
            if (players[i] == entities.get(entity)) {
                var e = entities.get(entity);
                e.playing.disconnect(handlePlay);
                e.stopped.disconnect(handleStop);
                players.splice(i, 1);
                currPlaying--;
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
            id: mediaPlayersRepeater
            model: players.length

            Item {
                id: player
                width: 480
                property alias player: player

                property var obj: players[index]

                states: [State {
                    name: "open"
                    when: miniMediaPlayer.state == "open"
                    PropertyChanges {target: title; opacity: 0 }
                    PropertyChanges {target: artist; opacity: 0 }
                },
                State {
                    name: "closed"
                    when: miniMediaPlayer.state == "closed"
                    PropertyChanges {target: title; opacity: 1 }
                    PropertyChanges {target: artist; opacity: 1 }
                }]

                transitions: [
                    Transition {
                        to: "open"
                        PropertyAnimation { target: title; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                    },
                    Transition {
                        to: "closed"
                        PropertyAnimation { target: title; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                    }
                ]

                Rectangle {
                    id: comp
                    anchors.fill: parent
                    color: colorBackground

                    Image {
                        id: bgImage
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectCrop
                        asynchronous: true
                        source: players[index].mediaImage == "" ? "qrc:/images/mini-music-player/no_image.png" : players[index].mediaImage

                        onStatusChanged: {
                            if (bgImage.status == Image.Error) {
                                bgImage.source = "";
                                bgImage.source = players[index].mediaImage == "" ? "qrc:/images/mini-music-player/no_image.png" : players[index].mediaImage;
                            }
                        }
                    }

                    GaussianBlur {
                        id: blur
                        anchors.fill: bgImage
                        source: bgImage
                        radius: 10
                        samples: 10
                    }
                }

                Image {
                    id: noise
                    anchors.fill: parent
                    asynchronous: true
                    fillMode: Image.Stretch
                    source: "qrc:/images/mini-music-player/noise.png"
                }

                Blend {
                    anchors.fill: comp
                    source: comp
                    foregroundSource: noise
                    mode: "multiply"
                }

                Rectangle {
                    anchors.fill: noise
                    color: colorBackground
                    opacity: 0.7
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
                    source: players[index].mediaImage == "" ? "qrc:/images/mini-music-player/no_image.png" : players[index].mediaImage

                    onStatusChanged: {
                        if (image.status == Image.Error) {
                            image.source = "";
                            image.source = players[index].mediaImage == "" ? "qrc:/images/mini-music-player/no_image.png" : players[index].mediaImage;
                        }
                    }

                    opacity: miniMediaPlayer.state == "closed" ? 1 : 0

                    Behavior on opacity {
                        NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
                    }
                }

                Item {
                    id: textContainer
                    height: childrenRect.height

                    anchors.left: image.right
                    anchors.leftMargin: 20
                    anchors.verticalCenter: image.verticalCenter
                    anchors.topMargin: 290

                    Text {
                        id: title
                        color: colorText
                        text: players[index].mediaTitle
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        wrapMode: Text.NoWrap
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
                        wrapMode: Text.NoWrap
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

                //                Rectangle {
                //                    width: parent.width - 30
                //                    height: 2
                //                    color: colorLight
                //                    opacity: (800-mainNav.y)/mainNav.height

                //                    anchors.bottom: parent.bottom
                //                    anchors.horizontalCenter: parent.horizontalCenter
                //                }

                MouseArea {
                    anchors.fill: parent
                    enabled: miniMediaPlayer.state == "closed" ? true : false

                    onClicked: {
                        miniMediaPlayer.state = "open";
                    }
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
