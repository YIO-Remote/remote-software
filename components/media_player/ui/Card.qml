import QtQuick 2.11
import QtQuick.Controls 2.5

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: card
    width: parent.width
    height: parent.height
    color: mediaplayerUtils.pixelColor === "#000000" ? colorDark : mediaplayerUtils.pixelColor

    Behavior on color {
        ColorAnimation { duration: 300 }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Connections {
        target: buttonHandler
        enabled: mediaplayerButton.state == "open" ? true : false

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
                    volume.volumePosition = obj.volume;
                    volume.state = "visible";
                }
                var newvolume = obj.volume + 2;
                if (newvolume > 100) newvolume = 100;
                obj.setVolume(newvolume);
                volume.volumePosition = newvolume;
            } else {
                if (volume.state != "visible") {
                    volume.volumePosition = obj.volume;
                    volume.state = "visible";
                }
                newvolume = obj.volume - 2;
                if (newvolume < 0) newvolume = 0;
                obj.setVolume(newvolume);
                volume.volumePosition = newvolume;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //    state: "closed"

    //    states: [
    //        State {
    //            name: "closed"
    //            PropertyChanges {target: albumArt; opacity: 0 }
    //            PropertyChanges {target: songTitle; anchors.topMargin: 60; opacity: 0 }
    //            PropertyChanges {target: artist; opacity: 0 }
    //            PropertyChanges {target: playButton; opacity: 0 }
    //            PropertyChanges {target: prevButton; opacity: 0 }
    //            PropertyChanges {target: nextButton; opacity: 0 }
    //            PropertyChanges {target: progressBar; opacity: 0 }
    //        },
    //        State {
    //            name: "open"
    //            PropertyChanges {target: albumArt; opacity: 1 }
    //            PropertyChanges {target: songTitle; anchors.topMargin: 20; opacity: 1 }
    //            PropertyChanges {target: artist; opacity: 0.8 }
    //            PropertyChanges {target: playButton; opacity: 1 }
    //            PropertyChanges {target: prevButton; opacity: 1 }
    //            PropertyChanges {target: nextButton; opacity: 1 }
    //            PropertyChanges {target: progressBar; opacity: 1 }
    //        }
    //    ]

    //    transitions: [
    //        Transition {to: "closed";
    //            PropertyAnimation { target: albumArt; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
    //            PropertyAnimation { target: songTitle; properties: "opacity, anchors.topMargin"; easing.type: Easing.OutExpo; duration: 300 }
    //            PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
    //            PropertyAnimation { target: playButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
    //            PropertyAnimation { target: prevButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
    //            PropertyAnimation { target: nextButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
    //            PropertyAnimation { target: progressBar; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
    //        },
    //        Transition {to: "open";
    //            SequentialAnimation {
    //                //                PauseAnimation { duration: 200 }
    //                PropertyAnimation { target: albumArt; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
    //                ParallelAnimation {
    //                    PropertyAnimation { target: songTitle; properties: "opacity, anchors.topMargin"; easing.type: Easing.OutExpo; duration: 300 }
    //                    PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
    //                    PropertyAnimation { target: progressBar; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
    //                }
    //                ParallelAnimation {
    //                    PropertyAnimation { target: playButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
    //                    PropertyAnimation { target: prevButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
    //                    PropertyAnimation { target: nextButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
    //                }
    //            }
    //        }
    //    ]

    //    Component.onCompleted: {
    //        card.state = "open";
    //    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property var features: []
    property bool startup: true

    Component.onCompleted: {
        features.push("HOME");
        if (obj.supported_features.indexOf("SEARCH") > -1)
            features.push("SEARCH");
        if (obj.supported_features.indexOf("LIST") > -1)
            features.push("LIST");
        if (obj.supported_features.indexOf("SPEAKER_CONTROL") > -1)
            features.push("SPEAKER_CONTROL");

        cardRepeater.model = features;
    }


    SwipeView {
        id: cardSwipeView
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        currentIndex: 0

        Item {
            CardHome {}
        }

        Repeater {
            id: cardRepeater

            Loader {
                asynchronous: true
                sourceComponent: {
                    if (card.features.indexOf("SEARCH")-1 == index ) {
                        return cardSearch;
                    }
                    else if (card.features.indexOf("LIST")-1 == index ) {
                        return cardList;
                    }
                    else if (card.features.indexOf("SPEAKER_CONTROL")-1 == index ) {
                        return cardSpeakerControl;
                    }
                }
            }
        }

        Component {
            id: cardHome
            CardHome {}
        }

        Component {
            id: cardSearch
            Rectangle {
                anchors.fill: parent
                color: "red"
            }
        }

        Component {
            id: cardList
            Rectangle {
                anchors.fill: parent
                color: "blue"
            }
        }

        Component {
            id: cardSpeakerControl
            Rectangle {
                anchors.fill: parent
                color: "green"
            }
        }
    }

    // BOTTOM MENU
    Grid {
        id: bottomMenu
        width: childrenRect.width
        height: 60
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        spacing: {
            var i = 0;

            if (obj.supported_features.indexOf("SEARCH") > -1)
                i++;
            if (obj.supported_features.indexOf("LIST") > -1)
                i++;
            if (obj.supported_features.indexOf("SPEAKER_CONTROL") > -1)
                i++;

            if (i === 0)
                return 100;
            else if (i === 1)
                return 100;
            else if (i === 2)
                return 80;
            else if (i === 3)
                return 40;
        }

        // home
        Text {
            color: colorText
            opacity: cardSwipeView.currentIndex === 0 ? 1 : 0.5
            text: "\uE900"
            renderType: Text.NativeRendering
            width: 60
            height: 60
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }

            MouseArea {
                anchors.fill: parent
                width: parent.width + 30
                height: width

                onClicked: {
                    cardSwipeView.currentIndex = 0;
                }
            }
        }

        // search
        Text {
            visible: obj.supported_features.indexOf("SEARCH") > -1 ? true : false
            color: colorText
            opacity: cardSwipeView.currentIndex === features.indexOf("SEARCH") ? 1 : 0.5
            text: "\uE90C"
            renderType: Text.NativeRendering
            width: 60
            height: 60
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }

            MouseArea {
                anchors.fill: parent
                width: parent.width + 30
                height: width

                onClicked: {
                    cardSwipeView.currentIndex = features.indexOf("SEARCH");
                }
            }
        }

        // playlists
        Text {
            visible: obj.supported_features.indexOf("LIST") > -1 ? true : false
            color: colorText
            opacity: cardSwipeView.currentIndex === features.indexOf("LIST") ? 1 : 0.5
            text: "\uE907"
            renderType: Text.NativeRendering
            width: 60
            height: 60
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }

            MouseArea {
                anchors.fill: parent
                width: parent.width + 30
                height: width

                onClicked: {
                    cardSwipeView.currentIndex = features.indexOf("LIST");
                }
            }
        }

        // speakers
        Text {
            visible: obj.supported_features.indexOf("SPEAKER_CONTROL") > -1 ? true : false
            color: colorText
            opacity: cardSwipeView.currentIndex === features.indexOf("SPEAKER_CONTROL") ? 1 : 0.5
            text: "\uE90D"
            renderType: Text.NativeRendering
            width: 60
            height: 60
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }

            MouseArea {
                anchors.fill: parent
                width: parent.width + 30
                height: width

                onClicked: {
                    cardSwipeView.currentIndex = features.indexOf("SPEAKER_CONTROL");
                }
            }
        }
    }

}
