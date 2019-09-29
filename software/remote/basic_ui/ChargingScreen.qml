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

    onStateChanged: {
        if (state == "visible") {
            resetClock.start();
            showClock.start();
        } else {
            resetClock.start();
        }
    }

    property alias resetClock: resetClock

    ParallelAnimation {
        id: resetClock
        running: false

        PropertyAnimation { target: batteryIcon; properties: "x"; to: (parent.width-implicitWidth)/2; duration: 1 }
        PropertyAnimation { target: batteryIcon; properties: "y"; to: (parent.height-implicitHeight)/2; duration: 1 }
        PropertyAnimation { target: batteryIcon; properties: "scale"; to: 1; duration: 1 }
        PropertyAnimation { target: chargeText; properties: "opacity"; to: 1; duration: 1 }
        PropertyAnimation { target: timeText; properties: "anchors.bottomMargin"; to: -400; duration: 1 }
    }

    property alias showClock: showClock

    SequentialAnimation {
        id: showClock
        running: false

        PauseAnimation {duration: 5000}
        ParallelAnimation {
            PropertyAnimation { target: batteryIcon; properties: "x"; to: 100; easing.type: Easing.OutExpo; duration: 600 }
            PropertyAnimation { target: batteryIcon; properties: "y"; to: 150; easing.type: Easing.OutExpo; duration: 600 }
            PropertyAnimation { target: batteryIcon; properties: "scale"; to: 0.5; easing.type: Easing.OutExpo; duration: 600 }
            PropertyAnimation { target: chargeText; properties: "opacity"; to: 0; easing.type: Easing.OutExpo; duration: 600 }
            PropertyAnimation { target: timeText; properties: "anchors.bottomMargin"; to: 40; easing.type: Easing.OutExpo; duration: 600 }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // BATTERY ICON
    //////////////////////////////////////////////////////////////////////////////////////////////////
    Item {
        id: batteryIcon
//        anchors.verticalCenter: parent.verticalCenter
//        anchors.horizontalCenter: parent.horizontalCenter
        x: (parent.width-implicitWidth)/2
        y: (parent.height-implicitHeight)/2

        // battery notch
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: batteryFrame.top
            anchors.bottomMargin: -12
            width: 83
            height: 56
            color: "#00000000"
            radius: 16
            border.width: 6
            border.color: colorLine
        }

        // battery frame
        Rectangle {
            id: batteryFrame
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: 151
            height: 218
            color: colorBackground
            border.width: 6
            border.color: colorLine
            radius: 32

            // battery fill
            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 23
                width: 106
                height: battery_level*172
                color: colorGreen
                radius: 8
            }

            Image {
                asynchronous: true
                id: icon
                width: 44
                height: 94
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
    }

    Text {
        id: chargeText
        color: colorText
        text: Math.round(battery_level * 100) + qsTr("% Charged") + translateHandler.emptyString
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 230
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 40
        horizontalAlignment: Text.AlignHCenter
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 32
        lineHeight: 0.8
    }

    Text {
        id: timeText
        color: colorText
        text: loader_main.item ? loader_main.item.statusBar.timeText.text : ""
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -400
        font.family: "Open Sans Light"
        font.pixelSize: 150
        lineHeight: 1
    }
}
