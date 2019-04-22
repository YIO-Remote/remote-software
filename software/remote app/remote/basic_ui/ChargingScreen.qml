import QtQuick 2.11
import QtGraphicalEffects 1.0
import "qrc:/scripts/helper.js" as JSHelper

Rectangle {
    id: chargingScreen
    width: parent.width
    height: parent.height
    color: colorBackground

    state: "hidden"

    states: [
        State { name: "visible"; PropertyChanges {target: chargingScreen; opacity: 1; visible: true} },
        State { name: "hidden"; PropertyChanges {target: chargingScreen; opacity: 0; visible: false} }
    ]
    transitions: [
        Transition {to: "hidden"; PropertyAnimation { target: chargingScreen; properties: "opacity, visible"; easing.type: Easing.InExpo; duration: 1000 }},
        Transition {to: "visible"; SequentialAnimation {
                PropertyAnimation { target: chargingScreen; properties: "visible"; duration: 1 }
                PauseAnimation { duration: 1000 }
                PropertyAnimation { target: chargingScreen; properties: "opacity"; easing.type: Easing.OutExpo; duration: 1000 }
            }}
    ]

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // BATTERY ICON
    //////////////////////////////////////////////////////////////////////////////////////////////////
    Item {
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        // battery fill
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: batteryFrame.left
            anchors.leftMargin: 11
            id: icon_battery
            width: JSHelper.mapValues(battery_voltage,3.5,4.2,4,144)
            height: 78
            color: colorGreen
            radius: 3
        }
        // battery frame
        Rectangle {
            id: batteryFrame
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: 166
            height: 100
            color: "#00000000"
            border.width: 4
            border.color: colorLine
            radius: 12

            Image {
                asynchronous: true
                id: icon
                width: 23
                height: 60
                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/chargingscreen/icon-battery-lightning.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                ColorOverlay {
                    visible: !darkMode
                    anchors.fill: parent
                    source: parent
                    color: colorText
                }
            }
        }
        // battery notch
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: batteryFrame.right
            anchors.leftMargin: -4
            width: 30
            height: 56
            color: "#00000000"
            radius: 4
            border.width: 4
            border.color: colorLine
        }
    }

    Text {
        color: colorText
        text: qsTr("Charging") + translateHandler.emptyString
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 80
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 40
        horizontalAlignment: Text.AlignHCenter
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 32
        lineHeight: 0.8
    }
}
