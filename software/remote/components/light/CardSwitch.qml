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
            loaded_components.light.lightComponentIntegration[integrationType].toggle(entity_id);
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
        anchors.top: toggleSettings.bottom
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
        id: toggleSettings
        width: 340
        height: 340
        anchors.horizontalCenter: parent.horizontalCenter

        Rectangle {
            width: 340
            height: width
            radius: 170
            color: "#00000000"
            border.color: colorBackground
            border.width: 20
            anchors.top: parent.top
            anchors.topMargin: 100
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Rectangle {
            id: circleButton
            width: 300
            height: width
            color: lstate == "off" ? "#00000000" : colorHighlight
            radius: width/2
            anchors.top: parent.top
            anchors.topMargin: 120
            anchors.horizontalCenter: parent.horizontalCenter

            states: State {
                name: "pressed"
                when: mouseAreaCircle.pressed === true
                PropertyChanges {
                    target: circleButton
                    color: colorText
                }
            }

            transitions: [
              Transition {
                  from: ""; to: "pressed"; reversible: true
                  PropertyAnimation { target: circleButton
                                      properties: "color"; duration: 400 }
              }]

            MouseArea {
                id: mouseAreaCircle
                anchors.fill: parent

                onClicked: {
                    loaded_components.light.lightComponentIntegration[integrationType].toggle(entity_id);
                }
            }
        }

        Image {
            id: icon
            asynchronous: true
            width: 80
            height: 80
            fillMode: Image.PreserveAspectFit
            source: "qrc:/components/light/images/icon-light.png"
            anchors.horizontalCenter: toggleSettings.horizontalCenter
            anchors.verticalCenterOffset: 100
            anchors.verticalCenter: toggleSettings.verticalCenter

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
