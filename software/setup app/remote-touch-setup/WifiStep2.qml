import QtQuick 2.9
import QtQuick.Controls 2.2

Item {
    Text {
        id: text_wifi2
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
        text: "Please enter your password"
        horizontalAlignment: Text.AlignHCenter
        anchors.top: text_wifi2.bottom
        anchors.topMargin: 0
        anchors.horizontalCenter: text_wifi2.horizontalCenter
        width: parent.width-120
        wrapMode: Text.WordWrap
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 28
        lineHeight: 1
    }

    WifiNetworkListElement {
        id: selectedWifi
        width: parent.width-40
        ssid: wifiNetworkSelected
        rssi: wifiNetworkSelectedRSSI
        selected: true
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: text_wifi2.bottom
        anchors.topMargin: 125
    }

    TextField {
        id: textfieldWifiPassword
        width: parent.width-40
        anchors.top: selectedWifi.bottom
        anchors.topMargin: 60
        anchors.horizontalCenter: parent.horizontalCenter
        height: 60
        echoMode: TextInput.PasswordEchoOnEdit
        placeholderText: "Password"
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhPreferLowercase | Qt.ImhSensitiveData | Qt.ImhNoPredictiveText

        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 28
        color: "#ffffff"

        background: Rectangle {
            implicitWidth: parent.width-40
            implicitHeight: 60
            color: colorButton
        }
    }

    CustomButton {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60
        buttonText: "Connect"

        mouseArea.onClicked: {
            if (textfieldWifiPassword != "") {
                connectionLoaderText.text = "Connecting ..."
                connectionLoader.state = "connecting";
                delayTimer.start();
            }
        }

        Timer {
            id: delayTimer
            running: false
            repeat: false

            interval: 2000

            onTriggered: {
                myLauncher.launch("/usr/bin/wifi_network_create.sh " + wifiNetworkSelected + " " + textfieldWifiPassword.text);
                myLauncher.launch("/usr/bin/wifi_network_join.sh");
                var wifiSuccess = myLauncher.launch("cat /sys/class/net/wlan0/operstate");
                if (wifiSuccess == "up\n") {
                    console.log("Successful Wifi connection");
                    connectionLoaderText.text = "Connected"
                    connectionLoader.state = "connected";
                    mainSwipeview.currentIndex += 1
                } else {
                    connectionLoaderText.text = "Failed to connect to the Wifi network"
                    connectionLoaderSmallText.text = "Make sure you have entered the credidentials corretly"
                    connectionLoader.state = "failed";
                    console.log("Failed Wifi connection");
                }
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
                wifiSelected = false
            }
        }
    }

}


