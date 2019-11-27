import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

Flickable {
    id: itemFlickable

    // UUID of page
    property string page

    //: Name of the settings page
    property string title: qsTr(config.pages[page].name) + translateHandler.emptyString

    // image background handling
    property string img_url

    // get the URL from config JSON
    Component.onCompleted: {
        img_url = Qt.binding(function () {
            if (config.pages[page].image) {
                topImage.visible = true;
                return "file://" + config.pages[page].image;
            } else {
                topImage.visible = false;
                return "";
            }
        });
    }

    signal scrolledUp()
    signal scrolledDown()
    signal scrollupBegin()
    signal scrolldownBegin()

    onContentYChanged: {
        if (contentY > 10) {
            scrollupBegin();
        } else {
            scrolldownBegin()
        }

        if (contentY > 130) {
            scrolledUp();
        } else {
            scrolledDown();
        }
    }

    width: parent.width
    height: parent.height
    maximumFlickVelocity: 6000
    flickDeceleration: 1000
    contentHeight: groupContainer.height + titleContainer.height + mainNavigation.height + 20
    boundsBehavior: Flickable.DragAndOvershootBounds
    flickableDirection: Flickable.VerticalFlick

    Behavior on contentY {
        PropertyAnimation {
            duration: 300
            easing.type: Easing.OutExpo
        }
    }

    ScrollBar.vertical: ScrollBar {
        opacity: 0.5
    }

    Item {
        id: topImage
        width: parent.width
        height: 320
        anchors.top: parent.top

        Rectangle {
            id: comp
            anchors.fill: parent
            color: colorBackground

            Image {
                id: bgImage
                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                source: img_url
            }

            Rectangle {
                id: gradient
                width: parent.width
                height: 80
                anchors.bottom: parent.bottom

                LinearGradient {
                        anchors.fill: parent
                        start: Qt.point(0, 0)
                        end: Qt.point(0, 80)
                        gradient: Gradient {
                            GradientStop { position: 0.2; color: colorBackgroundTransparent }
                            GradientStop { position: 1.0; color: colorBackground }
                        }
                    }
            }

            Blend {
                anchors.fill: bgImage
                source: bgImage
                foregroundSource: gradient
                mode: "multiply"
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
            opacity: 0.5
        }
    }

    Item {
        id: titleContainer
        width: parent.width
        height: 240

        Text {
            id: titleText
            color: colorText
            text: title
            anchors.centerIn: parent
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 60
            lineHeight: 1
        }
    }

    Column {
        id: groupContainer
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: titleContainer.bottom
        spacing: 60

        Repeater {
            id: groupRepeater
            model: config.pages[page].groups

            Group {
                groupID: config.pages[page].groups[index]
            }
        }
    }
}
