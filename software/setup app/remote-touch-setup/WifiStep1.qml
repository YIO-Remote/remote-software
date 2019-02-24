import QtQuick 2.9
import QtQuick.Controls 2.2

Item {
    function addNetworks() {
        var comp = Qt.createComponent("qrc:/WifiNetworkListElement.qml");

        for (var i=0; i<wifiNetworks.length; i++) {
            var obj = comp.createObject(flowWifiList, {ssid: wifiNetworks[i], rssi: wifiNetworksRSSI[i], buttonId: i});
            obj.clicked.connect(buttonClicked)
            wifiButtons[i] = obj
        }
    }

    Text {
        id: text_wifi
        color: colorText
        text: "Wifi"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 100
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Light
        font.styleName: "Light"
        font.pixelSize: 78
    }

    Text {
        color: colorText
        opacity: 0.4
        text: "Please select your network from the list"
        horizontalAlignment: Text.AlignHCenter
        anchors.top: text_wifi.bottom
        anchors.topMargin: 0
        anchors.horizontalCenter: text_wifi.horizontalCenter
        width: parent.width-120
        wrapMode: Text.WordWrap
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 28
        lineHeight: 1
    }

    Flickable {
        id: flickableWifiList
        anchors.top: text_wifi.bottom
        anchors.topMargin: 124
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width-40
        height: 280
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

    Text {
        color: colorText
        opacity: 0.4
        text: "Can't see your network? Tap here"
        horizontalAlignment: Text.AlignHCenter
        anchors.top: flickableWifiList.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: flickableWifiList.horizontalCenter
        width: parent.width-160
        wrapMode: Text.WordWrap
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 28
        lineHeight: 1

        MouseArea {
            anchors.fill: parent

            onClicked: {
                loaderWifiStep2.source = "qrc:/WifiStep3.qml"
                mainSwipeview.currentIndex += 1
            }
        }
    }

    CustomButton {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60
        buttonText: "Next"

        mouseArea.onClicked: {
            if (wifiSelected) {
                loaderWifiStep2.source = "qrc:/WifiStep2.qml"
                mainSwipeview.currentIndex += 1
            }
        }
    }

    Image {
        asynchronous: true
        width: 30
        height: 50
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/icon-back.png"
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 15

        MouseArea {
            width: parent.width + 40
            height: width
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            onClicked: {
                mainSwipeview.currentIndex -= 1
            }
        }
    }


}
