import QtQuick 2.11
import QtQuick.Controls 2.5

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
            var ssid = settingsLauncher.launch("/usr/bin/yio-remote/wifi_ssid.sh").trim();
            if (ssid == "") {
                wifiSSIDText.text = "Select WiFi network";
            } else {
                wifiSSIDText.text = ssid;
                wifiNetworkSelected = ssid;
            }
        }
    }

    Component.onCompleted: timer.start()

    property var wifiNetworks: []
    property var wifiNetworksRSSI: []
    property var wifiNetworkSelected
    property var wifiNetworkSelectedRSSI

    function addNetworks() {
        var comp = Qt.createComponent("qrc:/WifiNetworkListElement.qml");

        for (var i=0; i<wifiNetworks.length; i++) {
            var obj = comp.createObject(flowWifiList, {ssid: wifiNetworks[i], rssi: wifiNetworksRSSI[i], buttonId: i});
        }
    }

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
        color: colorDark

        anchors.top: header.bottom
        anchors.topMargin: 20

        Item {
            id: wifiSSID
            width: parent.width
            height: 50
            anchors.top: parent.top
            anchors.topMargin: 20

            Text {
                id: wifiSSIDText
                color: colorText
                text: ""
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                font.family: "Open Sans"
                font.weight: Font.Normal
                font.pixelSize: 27
                lineHeight: 1
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    haptic.playEffect("click");

                    // Start wifi network scan
                    var tmp = mainLauncher.launch("/usr/bin/yio-remote/wifi_network_list.sh");
                    tmp = tmp.split('\n');
                    for (var i=0; i<tmp.length-1; i++) {
                        var wifitmp = tmp[i].split(',')
                        wifiNetworks[i] = wifitmp[1];
                        wifiNetworksRSSI[i] = wifitmp[0]
                    }

                    // open popup that displays the list
                    popup.open();


                }
            }

        }


        Popup {
            id: popup
            x: 0
            y: 0
            width: section.width
            height: 400
            modal: true
            focus: true
            clip: true
            closePolicy: Popup.CloseOnPressOutside

            enter: Transition {
                NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; easing.type: Easing.OutExpo; duration: 300 }
                NumberAnimation { property: "width"; from: 0; to: section.width; easing.type: Easing.OutBack; duration: 300 }
                NumberAnimation { property: "height"; from: 0; to: 400; easing.type: Easing.OutBack; duration: 300 }
            }

            exit: Transition {
                NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; easing.type: Easing.InExpo; duration: 300 }
                NumberAnimation { property: "width"; from: section.width; to: 0; easing.type: Easing.InExpo; duration: 300 }
                NumberAnimation { property: "height"; from: 400; to: 0; easing.type: Easing.InExpo; duration: 300 }
            }

            background: Rectangle {
                anchors.fill: parent
                color: colorLine
                radius: cornerRadius
            }

            Flickable {
                id: flickableWifiList
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                height: parent.height
                maximumFlickVelocity: 4000
                flickDeceleration: 1000
                clip: true
                contentHeight: flowWifiList.height
                boundsBehavior: Flickable.DragAndOvershootBounds
                flickableDirection: Flickable.VerticalFlick

                ScrollBar.vertical: ScrollBar {
                    opacity: 0.5
                }

                Flow {
                    id: flowWifiList
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    spacing: 0
                }
            }

        }


        Rectangle {
            id: line0
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: wifiSSID.bottom
            anchors.topMargin: 20
        }

        Text {
            id: wifiSignalText
            color: colorText
            text: qsTr("Wi-Fi signal strength") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line0.bottom
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
