import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: mediaPlayer
    width: parent.width
    height: parent.height
    color: colorDark

    // blind graphics draggable element

    property int volume: obj.volume

    property var percent
    property bool userMove: false
    property bool listOpen: false;

    Component.onCompleted: {
        percent = obj.volume
    }

    Item {
        width: parent.width
        height: parent.height
        anchors.centerIn: parent

        Column {
            anchors.top: parent.top

            spacing: 10

            Repeater {
                id: repeater
                model: userMove ? Math.round(percent*36/100) : Math.round(obj.volume*36/100)

                delegate: Rectangle {
                    width: mediaPlayer.width
                    height: 10
                    color: colorHighlight2
                }
            }
        }

        Rectangle {
            id: dragger_target
            width: parent.width
            height: parent.height
            color: colorBackgroundTransparent
            y: userMove ? Math.round(height*percent/100) : Math.round(obj.volume/100)
        }

        MouseArea {
            id: dragger
            anchors.fill: parent
            drag.target: dragger_target
            drag.axis: Drag.YAxis
            drag.minimumY: 0
            drag.maximumY: dragger_target.height

            onPositionChanged: {
                haptic.playEffect("bump");
                percent = Math.round(mouse.y/parent.height*100)
                if (percent < 0) percent = 0
                if (percent > 100) percent = 100
            }

            onPressed: {
                userMove = true;
            }

            onReleased: {
                obj.setVolume(percent);
            }
        }
    }

    onVolumeChanged: {
        if (userMove && volume === percent) {
            userMove = false;
        } else {
            percent = position;
        }
    }

    Text {
        id: icon
        color: colorText
        text: "\uE900"
        renderType: Text.NativeRendering
        width: 85
        height: 85
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font {family: "icons"; pixelSize: 100 }
        anchors {top: parent.top; topMargin: 20; left: parent.left; leftMargin: 20}
    }

    Text {
        id: percentage
        color: colorText
        text: userMove ? percent : obj.volume
        horizontalAlignment: Text.AlignLeft
        anchors { top: parent.top; topMargin: 100; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Light"; pixelSize: 180 }
    }

    Text {
        color: colorText
        text: "%"
        anchors { left: percentage.right; bottom: percentage.bottom; bottomMargin: 30 }
        font {family: "Open Sans Light"; pixelSize: 100 }
    }

    Text {
        id: title
        color: colorText
        text: obj.friendly_name
        wrapMode: Text.WordWrap
        width: parent.width-60
        anchors { top: percentage.bottom; topMargin: -40; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Regular"; pixelSize: 60 }
        lineHeight: 0.9
    }

    Text {
        id: areaText
        color: colorText
        opacity: 0.5
        text: obj.area
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
        width: parent.width-60
        anchors { top: title.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Regular"; pixelSize: 24 }
    }

    function getFeature(feature) {
        if (obj.supported_features.indexOf(feature) > -1) {
            return true;
        } else {
            return false;
        }
    }

    BasicUI.CustomButton {
        anchors { left: parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
        color: colorText
        buttonText: "   "
        visible: getFeature("NEXT");

        mouseArea.onClicked: {
            haptic.playEffect("click");
            obj.next()
        }

        Image {
            asynchronous: true
            anchors.centerIn: parent
            source: "qrc:/images/mini-music-player/icon-next.png"
        }
    }

    BasicUI.CustomButton {
        anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom; bottomMargin: 70 }
        color: colorText
        buttonText: "LIST"
        buttonTextColor: colorBackground
        visible: getFeature("SOURCE");
        mouseArea.onClicked: {
            haptic.playEffect("click");
            obj.browse("TOP");
            listOpen = true;
        }
    }

    BasicUI.CustomButton {
        anchors { right: parent.right; rightMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
        color: colorText
        buttonText: "   "
        visible: getFeature("PREVIOUS");

        mouseArea.onClicked: {
            haptic.playEffect("click");
            obj.previous()
        }

        Image {
            asynchronous: true
            anchors.centerIn: parent
            source: "qrc:/images/mini-music-player/icon-prev.png"
        }
    }


    Image {
        id: closeButton
        asynchronous: true
        width: 30
        height: 30
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/components/close_button.png"
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 20

        ColorOverlay {
            visible: !darkMode
            anchors.fill: parent
            source: parent
            color: colorText
            antialiasing: true
        }

        MouseArea {
            width: parent.width + 60
            height: parent.height + 60
            anchors.centerIn: parent

            onClicked: {
                haptic.playEffect("click");
                mediaButton.state = "closed"
            }
        }
    }

    Loader {
        id: listLoader
        width: mediaPlayer.width
        height: mediaPlayer.height
        asynchronous: true
        active: listOpen
        source: "qrc:/components/media_player/ui/CardList.qml"
        opacity: listLoader.status == Loader.Ready ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutExpo
            }
        }

    }

}
