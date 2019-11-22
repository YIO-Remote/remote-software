import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

import MediaPlayerUtils 1.0

Item {
    id: miniMediaPlayer
    width: 480
    height: 90
    anchors.bottom: parent.bottom

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"


    states: [
        State { name: "closed";
            PropertyChanges {target: miniMediaPlayer; height: 90 }
            ParentChange { target: miniMediaPlayer; parent: loader_main.item.miniMediaPlayer; scale: 1 }
            PropertyChanges {target: loader_main; state: "visible" }
        },
        State { name: "open";
            PropertyChanges {target: miniMediaPlayer; height: 670 }
            ParentChange { target: miniMediaPlayer; parent: contentWrapper }
            PropertyChanges {target: loader_main; state: "hidden" }
        }
    ]
    transitions: [
        Transition {to: "closed";
            SequentialAnimation {
                PauseAnimation { duration: 200 }
                ParallelAnimation {
                    PropertyAnimation { target: loader_main; properties: "state"; duration: 1 }
                    PropertyAnimation { target: miniMediaPlayer; properties: "height"; easing.type: Easing.OutBack; easing.overshoot: 0.7; duration: 300 }
                }
                ParentAnimation {
                    NumberAnimation { properties: "scale"; easing.type: Easing.OutBack; easing.overshoot: 0.7; duration: 300 }
                }
            }
        },
        Transition {to: "open";
            ParallelAnimation {
                PropertyAnimation { target: miniMediaPlayer; properties: "height"; easing.type: Easing.OutBack; easing.overshoot: 1.2; duration: 300 }
                ParentAnimation {
                    NumberAnimation { properties: "scale"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 300 }
                }
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

            Behavior on radius {
                NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Connections {
        target: buttonHandler
        enabled: miniMediaPlayer.state == "open" && (standbyControl.mode == "on" || standbyControl.mode == "dim")

        onButtonPress: {
            switch (button) {
            case "dpad middle":
                entities.mediaplayersPlaying[mediaPlayers.currentIndex].play();
                break;
            case "dpad right":
                if (mediaPlayers.currentIndex < mediaPlayers.count-1) {
                    mediaPlayers.currentIndex += 1;
                } else {
                    haptic.playEffect("buzz");
                }
                break;
            case "dpad left":
                if (mediaPlayers.currentIndex > 0) {
                    mediaPlayers.currentIndex -= 1;
                } else {
                    haptic.playEffect("buzz");
                }
                break;
            case "top right":
                miniMediaPlayer.state = "closed";
                break;
            }
        }
    }

    Connections {
        target: buttonHandler

        onButtonPress: {
            switch (button) {
            case "volume up":
                buttonTimeout.stop();
                buttonTimeout.volumeUp = true;
                buttonTimeout.start();
                break;
            case "volume down":
                buttonTimeout.stop();
                buttonTimeout.volumeUp = false;
                buttonTimeout.start();
                break;
            }
        }

        onButtonRelease: {
            buttonTimeout.stop();
        }
    }

    Timer {
        id: buttonTimeout
        interval: 300
        repeat: true
        running: false
        triggeredOnStart: true

        property bool volumeUp: false

        onTriggered: {
            if (volumeUp) {
                if (volume.state != "visible") {
                    volume.volumePosition = mediaPlayers.currentItem.player.obj.volume;
                    volume.state = "visible";
                }
                var newvolume = mediaPlayers.currentItem.player.obj.volume + 0.02;
                if (newvolume > 1) newvolume = 1;
                mediaPlayers.currentItem.player.obj.setVolume(newvolume);
                volume.volumePosition = newvolume;
            } else {
                if (volume.state != "visible") {
                    volume.volumePosition = mediaPlayers.currentItem.player.obj.volume;
                    volume.state = "visible";
                }
                newvolume = mediaPlayers.currentItem.player.obj.volume - 0.02;
                if (newvolume < 0) newvolume = 0;
                mediaPlayers.currentItem.player.obj.setVolume(newvolume);
                volume.volumePosition = newvolume;
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
            model: entities.mediaplayersPlaying.length

            Item {
                id: player
                width: 480

                // include mediaplayer utils
                MediaPlayerUtils {
                    id: mediaplayerUtils
                }

                property alias player: player

                property var obj: entities.mediaplayersPlaying[index]

                state: "closed"

                states: [State {
                        name: "open"
                        when: miniMediaPlayer.state == "open"
                        PropertyChanges {target: title; opacity: 0 }
                        PropertyChanges {target: artist; opacity: 0 }
                        PropertyChanges {target: closeButton; opacity: 1 }
                        PropertyChanges {target: titleOpen; y: 380; opacity: 1 }
                        PropertyChanges {target: artistOpen; opacity: 0.8 }
                        PropertyChanges {target: indicator; opacity: 1 }
                        PropertyChanges {target: speaker; opacity: 1 }
                        PropertyChanges {target: playButton; opacity: 1; anchors.bottomMargin: 80 }
                        PropertyChanges {target: prevButton; opacity: 1 }
                        PropertyChanges {target: nextButton; opacity: 1 }
                        PropertyChanges {target: sourceText; opacity: 1 }
                        PropertyChanges {target: bgImage; opacity: 1; visible: true; anchors.topMargin: 86; scale: 1 }
                        PropertyChanges {target: image; opacity: 0 }
                    },
                    State {
                        name: "closed"
                        when: miniMediaPlayer.state == "closed"
                        PropertyChanges {target: title; opacity: 1 }
                        PropertyChanges {target: artist; opacity: 1 }
                        PropertyChanges {target: closeButton; opacity: 0 }
                        PropertyChanges {target: titleOpen; y: 420; opacity: 0 }
                        PropertyChanges {target: artistOpen; opacity: 0 }
                        PropertyChanges {target: indicator; opacity: 0 }
                        PropertyChanges {target: speaker; opacity: 0 }
                        PropertyChanges {target: playButton; opacity: 0; anchors.bottomMargin: 40 }
                        PropertyChanges {target: prevButton; opacity: 0 }
                        PropertyChanges {target: nextButton; opacity: 0 }
                        PropertyChanges {target: sourceText; opacity: 0 }
                        PropertyChanges {target: bgImage; opacity: 0; visible: false; anchors.topMargin: 126; scale: 0.8 }
                        PropertyChanges {target: image; opacity: 1 }
                    }]

                transitions: [
                    Transition {
                        to: "open"
                        SequentialAnimation {
                            ParallelAnimation {
                                PropertyAnimation { target: title; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: closeButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: indicator; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: image; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                SequentialAnimation {
                                    PauseAnimation { duration: 300 }
                                    ParallelAnimation {
                                        PropertyAnimation { target: titleOpen; properties: "y, opacity"; easing.type: Easing.OutBack; easing.overshoot: 1; duration: 400 }
                                        PropertyAnimation { target: sourceText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                        SequentialAnimation {
                                            //                                            PauseAnimation { duration: 200 }
                                            PropertyAnimation { target: artistOpen; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                            ParallelAnimation {
                                                PropertyAnimation { target: playButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                                PropertyAnimation { target: playButton; properties: "anchors.bottomMargin"; easing.type: Easing.OutBack; easing.overshoot: 1.4; duration: 300 }
                                                PropertyAnimation { target: prevButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                                PropertyAnimation { target: nextButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                            }
                                        }
                                        PropertyAnimation { target: speaker; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                        SequentialAnimation {
                                            PropertyAnimation { target: bgImage; properties: "visible"; duration: 1 }
                                            ParallelAnimation {
                                                PropertyAnimation { target: bgImage; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
                                                PropertyAnimation { target: bgImage; properties: "anchors.topMargin, scale"; easing.type: Easing.OutBack; easing.overshoot: 1.4; duration: 400 }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    },
                    Transition {
                        to: "closed"
                        SequentialAnimation {
                            PropertyAnimation { target: bgImage; properties: "opacity, anchors.topMargin, scale"; easing.type: Easing.OutExpo; duration: 200 }
                            PropertyAnimation { target: bgImage; properties: "visible"; duration: 1 }
                            ParallelAnimation {
                                PropertyAnimation { target: title; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: closeButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: titleOpen; properties: "y, opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: artistOpen; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: indicator; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: speaker; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: playButton; properties: "opacity, anchors.bottomMargin"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: prevButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: nextButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: sourceText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: image; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                            }
                        }
                    }
                ]

                Rectangle {
                    id: comp
                    anchors.fill: parent
                    color: mediaplayerUtils.pixelColor

                    Behavior on color {
                        ColorAnimation { duration: 300 }
                    }

                    property var m_image: entities.mediaplayersPlaying[index].mediaImage

                    onM_imageChanged: {
                        mediaplayerUtils.imageURL = entities.mediaplayersPlaying[index].mediaImage
                    }

                    CustomImageLoader {
                        id: bgImage
                        width: 280
                        height: 280
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 86
                        url: entities.mediaplayersPlaying[index].mediaImage == "" ? "qrc:/images/mini-music-player/no_image.png" : mediaplayerUtils.image //utils.miniMusicPlayerImage
                    }
                }

                CustomImageLoader {
                    id: image
                    width: 90
                    height: width
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: 0
                    }
                    url: entities.mediaplayersPlaying[index].mediaImage == "" ? "qrc:/images/mini-music-player/no_image.png" : mediaplayerUtils.smallImage
                }

                Item {
                    id: textContainer
                    height: childrenRect.height

                    anchors.left: image.right
                    anchors.leftMargin: 20
                    anchors.verticalCenter: image.verticalCenter

                    Text {
                        id: title
                        color: colorText
                        text: entities.mediaplayersPlaying[index].friendly_name
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
                        text: entities.mediaplayersPlaying[index].mediaTitle
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

                MouseArea {
                    anchors.fill: parent
                    enabled: miniMediaPlayer.state == "closed" ? true : false

                    onClicked: {
                        miniMediaPlayer.state = "open";
                    }
                }

                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // OPEN STATE ELEMENTS
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                Text {
                    id: sourceText
                    color: colorText
                    text: entities.mediaplayersPlaying[index].source
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 27
                    anchors {
                        top: parent.top
                        topMargin: 20
                        left: parent.left
                        leftMargin: 20
                    }
                }


                Text {
                    id: titleOpen
                    color: colorText
                    text: entities.mediaplayersPlaying[index].mediaTitle
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
                }

                Text {
                    id: artistOpen
                    color: colorText
                    text: entities.mediaplayersPlaying[index].mediaArtist
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    width: parent.width-80
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 27
                    lineHeight: 1
                    anchors.top: titleOpen.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Item {
                    id: speaker
                    width: childrenRect.width
                    anchors {
                        bottom: parent.bottom
                        bottomMargin: 80
                        horizontalCenter: parent.horizontalCenter
                    }

                    Image {
                        id: speakerIcon
                        asynchronous: true
                        source: "qrc:/images/mini-music-player/icon-speaker.png"
                    }

                    Text {
                        color: colorText
                        text: entities.mediaplayersPlaying[index].friendly_name
                        verticalAlignment: Text.AlignVCenter
                        font.family: "Open Sans"
                        font.weight: Font.Normal
                        font.pixelSize: 27
                        lineHeight: 1
                        anchors {
                            left: speakerIcon.right
                            leftMargin: 20
                            verticalCenter: speakerIcon.verticalCenter
                        }
                    }
                }
            }
        }

    }

    Text {
        id: closeButton
        color: colorText
        text: "\uE916"
        renderType: Text.NativeRendering
        width: 70
        height: 70
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font {family: "icons"; pixelSize: 80 }
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10

        MouseArea {
            width: parent.width + 20
            height: parent.height + 20
            anchors.centerIn: parent
            enabled: miniMediaPlayer.state == "open"

            onClicked: {
                haptic.playEffect("click");
                miniMediaPlayer.state = "closed"
            }
        }
    }

    Item {
        id: prevButton
        width: 120
        height: 120

        anchors {
            right: playButton.left
            rightMargin: 30
            verticalCenter: playButton.verticalCenter
        }

        Text {
            color: colorText
            text: "\uE909"
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            enabled: miniMediaPlayer.state == "open"

            onClicked: {
                haptic.playEffect("click");
                entities.mediaplayersPlaying[mediaPlayers.currentIndex].previous();
            }
        }
    }

    Item {
        id: playButton
        width: 120
        height: 120

        property bool isPlaying: entities.mediaplayersPlaying[mediaPlayers.currentIndex] && entities.mediaplayersPlaying[mediaPlayers.currentIndex].state == 3 ? true : false

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 80
        }

        Text {
            color: colorText
            text: "\uE905"
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
            opacity: playButton.isPlaying ? 1 : 0

            Behavior on opacity {
                NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
            }
        }

        Text {
            color: colorText
            text: "\uE906"
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
            opacity: playButton.isPlaying ? 0 : 1

            Behavior on opacity {
                NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
            }
        }

        MouseArea {
            anchors.fill: parent
            enabled: miniMediaPlayer.state == "open"

            onClicked: {
                haptic.playEffect("click");
                entities.mediaplayersPlaying[mediaPlayers.currentIndex].play();
            }
        }
    }

    Item {
        id: nextButton
        width: 120
        height: 120

        anchors {
            left: playButton.right
            leftMargin: 30
            verticalCenter: playButton.verticalCenter
        }

        Text {
            color: colorText
            text: "\uE904"
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.centerIn: parent
        }

        MouseArea {
            anchors.fill: parent
            enabled: miniMediaPlayer.state == "open"

            onClicked: {
                haptic.playEffect("click");
                entities.mediaplayersPlaying[mediaPlayers.currentIndex].next();
            }
        }
    }

    PageIndicator {
        id: indicator

        count: mediaPlayers.count
        currentIndex: mediaPlayers.currentIndex

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter

        delegate: Rectangle {
            width: 8
            height: 8
            radius: height/2
            color: colorText
            opacity: index == mediaPlayers.currentIndex ? 1 : 0.3
        }
    }
}
