import QtQuick 2.11
import QtQuick.Controls 2.4

import Launcher 1.0

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
            wifiSignalValue.text = settingsLauncher.launch("/usr/bin/yio-remote/wifi_rssi.sh").trim();
        }
    }

    Component.onCompleted: timer.start()

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NETWORK
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Text {
        id: header
        color: colorText
        text: qsTr("Network") + translateHandler.emptyString
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
        color: colorMedium

        anchors.top: header.bottom
        anchors.topMargin: 20

        Text {
            id: wifiSignalText
            color: colorText
            text: qsTr("Wi-Fi signal strength") + translateHandler.emptyString
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
            id: wifiSignalValue
            color: colorText
            text: "-59"
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: wifiSignalText.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line6
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: wifiSignalText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: ipaddressText
            color: colorText
            text: qsTr("IP address") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line6.bottom
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            color: colorText
            text: settingsLauncher.launch("/usr/bin/yio-remote/wifi_ip.sh").trim()
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: ipaddressText.verticalCenter
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
            anchors.top: ipaddressText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: macaddressText
            color: colorText
            text: qsTr("MAC address") + translateHandler.emptyString
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
            color: colorText
            text: settingsLauncher.launch("cat /sys/class/net/wlan0/address").trim()
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: macaddressText.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }
    }
}
