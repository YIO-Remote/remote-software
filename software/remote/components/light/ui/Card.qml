import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    width: parent.width
    height: parent.height
    color: colorMedium

    MouseArea {
        anchors.fill: parent
    }

    BasicUI.CustomButtonState {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60
        buttonText: lstate == "off" ? "Turn on" : "Turn off"
        buttonState: lstate == "off" ? false : true

        mouseArea.onClicked: {
            loaded_components.light.componentIntegration[integrationType].toggle(entity_id);
        }
    }

    Text {
        id: title
        color: colorText
        text: friendly_name
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideNone
        wrapMode: Text.WordWrap
        width: parent.width-40
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: brightnessSettings.bottom
        anchors.topMargin: 120
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 30
        lineHeight: 1
    }

    Text {
        id: areaText
        color: colorText
        opacity: 0.5
        text: area
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
        width: parent.width-100
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: title.bottom
        anchors.topMargin: 10
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 24
        lineHeight: 1
    }


    Item {
        id: brightnessSettings
        width: 340
        height: 340
        anchors.horizontalCenter: parent.horizontalCenter

        property bool moving: false

        Rectangle {
            width: 340
            height: width
            radius: 170
            color: "#00000000"
            border.color: colorBackground
            border.width: 20
            anchors.centerIn: progressCircle_purple
        }

        BasicUI.ProgressCircle {
            id: progressCircle_purple
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 100
            rotation: 180

            size: 330
            colorCircle: colorHighlight
            colorCircleGrad: colorHighlight
            colorBackground: colorBackground
            showBackground: false
            arcBegin: 40
            arcEnd: (40+280*Math.round(dial.position*100)/100)
            animationDuration: !brightnessSettings.moving ? 1000 : 0
            lineWidth: 10
        }

        BasicUI.ProgressCircle {
            id: progressCircle_white
            anchors.horizontalCenter: progressCircle_purple.horizontalCenter
            anchors.verticalCenter: progressCircle_purple.verticalCenter
            rotation: 180

            size: 321
            colorCircle: colorLine
            colorCircleGrad: colorBackgroundTransparent
            showBackground: false
            arcBegin: progressCircle_purple.arcEnd > 140 ? progressCircle_purple.arcEnd - 90 : 40
            arcEnd: progressCircle_purple.arcEnd
            animationDuration: 0
            lineWidth: 2
        }

        Dial {
            id: dial
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 100

            from: 0
            to: 100
            enabled: true
            spacing: 10

            width: 340
            height: 340

            value: brightness

            onMoved: {
                brightnessSettings.moving = true
                change_brightness_timer.running = true
            }

            Timer { // this timer changes the light brightness
                id: change_brightness_timer
                interval: 1000
                repeat: false
                running: false

                onTriggered: {
                    change_brightness_timer.running = false;
                    brightnessSettings.moving = false;
                    loaded_components.light.componentIntegration[integrationType].setBrightness(entity_id, Math.round(dial.position*100));
                }
            }

            background: Rectangle {
                x: dial.width / 2 - width / 2
                y: dial.height / 2 - height / 2
                width: Math.max(64, Math.min(dial.width, dial.height))
                height: width
                color: "transparent"
            }

            handle: Rectangle {
                id: handleItem
                x: dial.background.x + dial.background.width / 2 - width / 2
                y: dial.background.y + dial.background.height / 2 - height / 2
                width: 32
                height: 32

                gradient: Gradient {
                    GradientStop { position: 1.0; color: colorBackground }
                    GradientStop { position: 0.0; color: colorButton }
                }

                radius: 16
                antialiasing: true
                transform: [
                    Translate {
                        y: -Math.min(dial.background.width, dial.background.height) * 0.4 + handleItem.height / 2
                    },
                    Rotation {
                        angle: dial.angle
                        origin.x: handleItem.width / 2
                        origin.y: handleItem.height / 2
                    }
                ]
            }
        }

        Text {
            id: percentText
            color: colorText
            text: Math.round(dial.position*100) + "%"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.NoWrap
            anchors.horizontalCenter: progressCircle_purple.horizontalCenter
            anchors.verticalCenter: progressCircle_purple.verticalCenter
            anchors.verticalCenterOffset: -40
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 80
            lineHeight: 1
        }

        Image {
            id: icon
            asynchronous: true
            width: 80
            height: 80
            fillMode: Image.PreserveAspectFit
            source: "qrc:/components/light/images/icon-light.png"
            anchors.horizontalCenter: progressCircle_purple.horizontalCenter
            anchors.top: percentText.bottom
//            anchors.topMargin: 10

            ColorOverlay {
                visible: !darkMode
                anchors.fill: parent
                source: parent
                color: colorText
                antialiasing: true
            }
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
                lightButton.state = "closed"
                loader_main.state = "visible"
                var tmp = mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY;
                mainNavigationSwipeview.currentItem.mainNavigationLoader.active = false;
                mainNavigationSwipeview.currentItem.mainNavigationLoader.active = true;
                mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY = tmp;
            }
        }
    }
}
