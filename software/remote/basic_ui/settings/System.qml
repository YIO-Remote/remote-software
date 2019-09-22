import QtQuick 2.11
import QtQuick.Controls 2.5

import Launcher 1.0

import "qrc:/basic_ui" as BasicUI

Item {
    width: parent.width
    height: header.height + section.height + 20

    Launcher {
        id: settingsLauncher
    }

    Timer {
        id: timer
        running: false
        repeat: true
        interval: 10000
        triggeredOnStart: true

        onTriggered: {
            uptimeValue.text = settingsLauncher.launch("/usr/bin/yio-remote/uptime.sh").trim();
            temperatureValue.text = parseInt(settingsLauncher.launch("cat /sys/class/thermal/thermal_zone0/temp"))/1000 + "ºC";
        }
    }

    Component.onCompleted: timer.start()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SYSTEM
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Text {
        id: header
        color: colorText
        text: qsTr("System") + translateHandler.emptyString
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: section
        width: parent.width
        height: childrenRect.height + 40
        radius: cornerRadius
        color: colorDark

        anchors.top: header.bottom
        anchors.topMargin: 20

        Text {
            id: uptimeText
            color: colorText
            text: qsTr("Uptime") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: uptimeValue
            color: colorText
            text: "0h"
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: uptimeText.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line7
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: uptimeText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: temperatureText
            color: colorText
            text: qsTr("CPU temperature") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line7.bottom
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: temperatureValue
            color: colorText
            text: "36ºC"
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: temperatureText.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line8
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: temperatureText.bottom
            anchors.topMargin: 20
        }

        BasicUI.CustomButton {
            id: buttonReboot
            buttonText: qsTr("Reboot") + translateHandler.emptyString
            anchors.top: line8.bottom
            anchors.topMargin: 30
            anchors.left: parent.left
            anchors.leftMargin: (parent.width - (buttonReboot.width + buttonShutdown.width + 40))/2

            mouseArea.onClicked: {
                settingsLauncher.launch("fbv -d 1 /bye.png")
                console.debug("now reboot")
                settingsLauncher.launch("reboot");
            }
        }

        BasicUI.CustomButton {
            id: buttonShutdown
            buttonText: qsTr("Shutdown") + translateHandler.emptyString
            anchors.top: line8.bottom
            anchors.topMargin: 30
            anchors.left: buttonReboot.right
            anchors.leftMargin: 40

            mouseArea.onClicked: {
                loadingScreen.source = "qrc:/basic_ui/ClosingScreen.qml";
                loadingScreen.active = true;
            }
        }
    }
}
