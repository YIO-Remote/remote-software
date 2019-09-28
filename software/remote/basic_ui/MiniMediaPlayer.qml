import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

Item {
    id: miniMediaPlayer
    width: 480
    height: 80
    anchors.bottom: parent.bottom

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"


    states: [
        State { name: "closed";
            PropertyChanges {target: miniMediaPlayer; height: 80 }
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
                ParallelAnimation {
                    PropertyAnimation { target: loader_main; properties: "state"; duration: 1 }
                    PropertyAnimation { target: miniMediaPlayer; properties: "height"; easing.type: Easing.OutExpo; duration: 300 }
                    ParentAnimation {
                        NumberAnimation { properties: "scale"; easing.type: Easing.OutExpo; duration: 300 }
                    }
                }
        },
        Transition {to: "open";
            ParallelAnimation {
                PropertyAnimation { target: miniMediaPlayer; properties: "height"; easing.type: Easing.OutExpo; duration: 300 }
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

    onCurrPlayingChanged: {
        if (currPlaying == 0) {
            loader_main.state = "visible";
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
            if (e[i].state == 3) {
                players.push(e[i]);
                currPlaying++;
            }
        }
        var tmp = players;
        players = tmp;
    }

    function add(name) {
        // check if we are runnig timers to kill players
        for (var t=0; t<runningTimers.length; t++) {
            if (runningTimers[t].name == name) {
                runningTimers[t].stop();
                runningTimers[t].destroy();
                runningTimers.splice(t, 1);
            }
        }

        // add new players
        var e = entities.get(name)
        var chg = false;

        if (players.indexOf(e) < 0) {
            if (e.supported_features.indexOf("PLAY") > -1 || e.supported_features.indexOf("PAUSE") > -1) {
                playButton.visible = true;
            } else {
                playButton.visible = false;
            }
            if (e.supported_features.indexOf("PREVIOUS") > -1) {
                prevButton.visible = true;
            } else {
                prevButton.visible = false;
            }
            if (e.supported_features.indexOf("NEXT") > -1) {
                nextButton.visible = true;
            } else {
                nextButton.visible = false;
            }

            players.push(e);
            currPlaying++;

            chg = true;
        }

        if (chg) {
            var tmp = players;
            players = tmp;
        }
    }

    Component {
        id: singleShot
        Timer {
            id: singleShotTimer
            running: true
            repeat: false

            property var action
            property var name

            onTriggered: {
                action()
                for (var i=0; i<runningTimers.length; i++) {
                    if (runningTimers[i].name == name) {
                        runningTimers.splice(i,1);
                    }
                }
                this.destroy(200)
            }
        }
    }

    property var runningTimers: []

    function remove(name) {
        // if stopped playing, remove the player after 30 seconds
        var obj = singleShot.createObject(miniMediaPlayer, { name: name, action: function() { removePlayer(name) }, interval: 60000 });
        runningTimers.push(obj);
    }

    function removePlayer(name) {
        for (var i=0; i<players.length; i++) {
            if (players[i] == entities.get(name)) {
                var e = entities.get(name);
                players.splice(i, 1);
                currPlaying--;
                var tmp = players;
                players = tmp;
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
                players[mediaPlayers.currentIndex].play();
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
            switch (button) {
            case "volume up":
                buttonTimeout.stop();
                break;
            case "volume down":
                buttonTimeout.stop();
                break;
            }
        }
    }

    Timer {
        id: buttonTimeout
        interval: 250
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
            model: players.length

            Item {
                id: player
                width: 480
                property alias player: player

                property var obj: players[index]

                state: "closed"

                states: [State {
                        name: "open"
                        when: miniMediaPlayer.state == "open"
                        PropertyChanges {target: title; opacity: 0 }
                        PropertyChanges {target: artist; opacity: 0 }
                        PropertyChanges {target: closeButton; opacity: 1 }
                        PropertyChanges {target: blur; radius: 0 }
                        PropertyChanges {target: overlay; opacity: 0.5 }
                        PropertyChanges {target: titleOpen; y: 200; opacity: 1 }
                        PropertyChanges {target: artistOpen; opacity: 0.8 }
                        PropertyChanges {target: indicator; opacity: 1 }
                        PropertyChanges {target: speaker; opacity: 1 }
                        PropertyChanges {target: playButton; opacity: 1 }
                        PropertyChanges {target: prevButton; opacity: 1 }
                        PropertyChanges {target: nextButton; opacity: 1 }
                        PropertyChanges {target: sourceText; opacity: 1 }
                    },
                    State {
                        name: "closed"
                        when: miniMediaPlayer.state == "closed"
                        PropertyChanges {target: title; opacity: 1 }
                        PropertyChanges {target: artist; opacity: 1 }
                        PropertyChanges {target: closeButton; opacity: 0 }
                        PropertyChanges {target: blur; radius: 10 }
                        PropertyChanges {target: overlay; opacity: 0.7 }
                        PropertyChanges {target: titleOpen; y: 366; opacity: 0 }
                        PropertyChanges {target: artistOpen; opacity: 0 }
                        PropertyChanges {target: indicator; opacity: 0 }
                        PropertyChanges {target: speaker; opacity: 0 }
                        PropertyChanges {target: playButton; opacity: 0 }
                        PropertyChanges {target: prevButton; opacity: 0 }
                        PropertyChanges {target: nextButton; opacity: 0 }
                        PropertyChanges {target: sourceText; opacity: 0 }
                    }]

                transitions: [
                    Transition {
                        to: "open"
                        SequentialAnimation {
                            ParallelAnimation {
                                PropertyAnimation { target: title; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: closeButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: overlay; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                PropertyAnimation { target: indicator; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                SequentialAnimation {
                                    PauseAnimation { duration: 300 }
                                    ParallelAnimation {
                                        PropertyAnimation { target: titleOpen; properties: "y, opacity"; easing.type: Easing.OutExpo; duration: 500 }
                                        PropertyAnimation { target: sourceText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                        SequentialAnimation {
                                            //                                            PauseAnimation { duration: 200 }
                                            PropertyAnimation { target: artistOpen; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                            ParallelAnimation {
                                                PropertyAnimation { target: playButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 500 }
                                                SequentialAnimation {
                                                    PauseAnimation { duration: 100 }
                                                    ParallelAnimation {
                                                        PropertyAnimation { target: prevButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                                        PropertyAnimation { target: nextButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                                    }
                                                }
                                            }
                                        }
                                        PropertyAnimation { target: speaker; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                                    }
                                }
                            }
                        }
                    },
                    Transition {
                        to: "closed"
                        PropertyAnimation { target: title; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: closeButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: overlay; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: titleOpen; properties: "y, opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: artistOpen; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: indicator; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: speaker; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: playButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: prevButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: nextButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        PropertyAnimation { target: sourceText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
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
                            if (image.status == Image.Error) {
                                image.source = players[index].mediaImage == "" ? "qrc:/images/mini-music-player/no_image.png" : players[index].mediaImage
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
                    fillMode: Image.PreserveAspectCrop
                    source: "qrc:/images/mini-music-player/noise.png"
                }

                Blend {
                    anchors.fill: comp
                    source: comp
                    foregroundSource: noise
                    mode: "multiply"
                }

                Rectangle {
                    id: overlay
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
                            image.source = players[index].mediaImage == "" ? "qrc:/images/mini-music-player/no_image.png" : players[index].mediaImage
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

                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // OPEN STATE ELEMENTS
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                Text {
                    id: sourceText
                    color: colorText
                    text: players[index].source
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
                    text: players[index].mediaTitle
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
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
                    text: players[index].mediaArtist
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    wrapMode: Text.WordWrap
                    width: parent.width-80
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 27
                    lineHeight: 1
                    anchors.top: titleOpen.bottom
                    anchors.topMargin: 10
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
                        text: players[index].friendly_name
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

    Item {
        id: closeButton
        width: 60
        height: width

        anchors.top: parent.top
        anchors.right: parent.right

        Image {
            asynchronous: true
            anchors {
                top: parent.top
                topMargin: 20
                right: parent.right
                rightMargin: 20
            }

            source: "qrc:/images/mini-music-player/icon-close.png"
        }

        MouseArea {
            anchors.fill: parent
            enabled: miniMediaPlayer.state == "open" ? true : false

            onClicked: {
                haptic.playEffect("click");
                miniMediaPlayer.state = "closed";
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

        Image {
            anchors.centerIn: parent
            asynchronous: true
            source: "qrc:/images/mini-music-player/icon-prev.png"
        }

        MouseArea {
            anchors.fill: parent
            enabled: miniMediaPlayer.state == "open"

            onClicked: {
                haptic.playEffect("click");
                players[mediaPlayers.currentIndex].previous();
            }
        }
    }

    Rectangle {
        id: playButton
        width: 120
        height: 120
        radius: height/2
        color: colorLight

        property bool isPlaying: players[mediaPlayers.currentIndex].state == 3 ? true : false

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 140
        }

        Item {
            width: childrenRect.width
            height: childrenRect.height
            anchors.centerIn: parent

            Rectangle {
                width: 4
                height: 30
                color: colorLine
                x: 0
                y: playButton.isPlaying ? 0 : 18
                rotation: playButton.isPlaying ? 0 : 45

                Behavior on y {
                    NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
                }

                Behavior on rotation {
                    NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
                }
            }

            Rectangle {
                width: 4
                height: 30
                color: colorLine
                x: playButton.isPlaying ? 10 : 0
                y: 0
                rotation: playButton.isPlaying ? 0 : -45

                Behavior on x {
                    NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
                }

                Behavior on rotation {
                    NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            enabled: miniMediaPlayer.state == "open"

            onClicked: {
                haptic.playEffect("click");
                players[mediaPlayers.currentIndex].play();
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

        Image {
            anchors.centerIn: parent
            asynchronous: true
            source: "qrc:/images/mini-music-player/icon-next.png"
        }

        MouseArea {
            anchors.fill: parent
            enabled: miniMediaPlayer.state == "open"

            onClicked: {
                haptic.playEffect("click");
                players[mediaPlayers.currentIndex].next();
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

    //    MouseArea {
    //        anchors.fill: parent
    //        propagateComposedEvents: true

    //        onClicked: {
    //            console.debug("OPEN mini player");
    //        }
    //    }
}
