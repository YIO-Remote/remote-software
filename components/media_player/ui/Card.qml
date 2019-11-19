import QtQuick 2.11

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: card
    width: parent.width
    height: parent.height
    color: mediaplayerUtils.pixelColor == "#000000" ? colorDark : mediaplayerUtils.pixelColor

    Behavior on color {
        ColorAnimation { duration: 300 }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"

    states: [
        State {
            name: "closed"
            PropertyChanges {target: albumArt; opacity: 0 }
            PropertyChanges {target: songTitle; anchors.topMargin: 60; opacity: 0 }
            PropertyChanges {target: artist; opacity: 0 }
            PropertyChanges {target: playButton; opacity: 0 }
            PropertyChanges {target: prevButton; opacity: 0 }
            PropertyChanges {target: nextButton; opacity: 0 }
            PropertyChanges {target: progressBar; opacity: 0 }
        },
        State {
            name: "open"
            PropertyChanges {target: albumArt; opacity: 1 }
            PropertyChanges {target: songTitle; anchors.topMargin: 20; opacity: 1 }
            PropertyChanges {target: artist; opacity: 0.8 }
            PropertyChanges {target: playButton; opacity: 1 }
            PropertyChanges {target: prevButton; opacity: 1 }
            PropertyChanges {target: nextButton; opacity: 1 }
            PropertyChanges {target: progressBar; opacity: 1 }
        }
    ]

    transitions: [
        Transition {to: "closed";
            PropertyAnimation { target: albumArt; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: songTitle; properties: "opacity, anchors.topMargin"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: playButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: prevButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: nextButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: progressBar; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
        },
        Transition {to: "open";
            SequentialAnimation {
                PauseAnimation { duration: 200 }
                PropertyAnimation { target: albumArt; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                ParallelAnimation {
                    PropertyAnimation { target: songTitle; properties: "opacity, anchors.topMargin"; easing.type: Easing.OutExpo; duration: 300 }
                    PropertyAnimation { target: artist; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                    PropertyAnimation { target: progressBar; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                }
                ParallelAnimation {
                    PropertyAnimation { target: playButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                    PropertyAnimation { target: prevButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                    PropertyAnimation { target: nextButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                }
            }
        }
    ]

    Component.onCompleted: {
        card.state = "open";
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    Item {
        width: parent.width
        height: 125
        anchors.top: parent.top

        Text {
            id: title
            color: colorText
            text: obj.friendly_name
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            width: parent.width-232
            anchors.left: parent.left
            anchors.leftMargin: 106
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: obj.source === "" ? 0 : -15
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: info
            color: colorText
            opacity: 0.5
            text: obj.source
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            width: title.width
            anchors.left: title.left
            anchors.top: title.bottom
            anchors.topMargin: -2
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Text {
            color: colorText
            text: icon.text
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 100 }
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
        }

    }

    BasicUI.CustomImageLoader {
        id: albumArt
        width: 280
        height: 280
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 118
        url: mediaplayerUtils.image == "" ? "qrc:/images/mini-music-player/no_image.png" : mediaplayerUtils.image
    }

    Text {
        id: songTitle
        color: colorText
        text: obj.mediaTitle
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
        anchors.top: albumArt.bottom
        anchors.topMargin: 20
    }

    Text {
        id: artist
        color: colorText
        text: obj.mediaArtist
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
        width: parent.width-80
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
        anchors.top: songTitle.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Rectangle {
        id: progressBar
        width: parent.width-80
        height: 4
        color: "#000000"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: artist.bottom
        anchors.topMargin: 30
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
            enabled: card.state == "open"

            onClicked: {
                haptic.playEffect("click");
                obj.previous();
            }
        }
    }

    Item {
        id: playButton
        width: 120
        height: 120

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 60
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
            opacity: obj.state == 3 ? 1 : 0

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
            opacity: obj.state == 3 ? 0 : 1

            Behavior on opacity {
                NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
            }
        }

        MouseArea {
            anchors.fill: parent
            enabled: card.state == "open"

            onClicked: {
                haptic.playEffect("click");
                obj.play();
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
            enabled: card.state == "open"

            onClicked: {
                haptic.playEffect("click");
                obj.next();
            }
        }
    }

    Grid {
        id: bottomMenu
        width: childrenRect.width
        height: 60
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        spacing: 80

        Rectangle {
            width: 60
            height: 60
            color: "red"
        }

        Rectangle {
            width: 60
            height: 60
            color: "red"
        }

        Rectangle {
            width: 60
            height: 60
            color: "red"
        }
    }

}
