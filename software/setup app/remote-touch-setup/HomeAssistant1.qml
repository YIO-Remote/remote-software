import QtQuick 2.9
import QtQuick.Controls 2.2

Item {
    Text {
        id: text_homeassistant
        color: colorText
        width: 300
        text: "Home Assistant"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 100
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Light
        font.styleName: "Light"
        font.pixelSize: 78
    }

    Text {
        color: colorText
        opacity: 0.4
        text: "Enter the server’s IP address and bearer token"
        horizontalAlignment: Text.AlignHCenter
        anchors.top: text_homeassistant.bottom
        anchors.topMargin: 0
        anchors.horizontalCenter: text_homeassistant.horizontalCenter
        width: parent.width-120
        wrapMode: Text.WordWrap
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 28
        lineHeight: 1
    }

    TextField {
        id: textfieldHomeAssistantIP
        width: parent.width-40
        anchors.top: text_homeassistant.bottom
        anchors.topMargin: 140
        anchors.horizontalCenter: parent.horizontalCenter
        height: 60
        echoMode: TextInput.Normal
        placeholderText: "hostname/IP address:port"
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

    TextField {
        id: textfieldHomeAssistantPassword
        width: parent.width-40
        anchors.top: textfieldHomeAssistantIP.bottom
        anchors.topMargin: 80
        anchors.horizontalCenter: parent.horizontalCenter
        height: 60
        echoMode: TextInput.PasswordEchoOnEdit
        placeholderText: "token"
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
            homeAssistantIP = textfieldHomeAssistantIP.text;
            homeAssistantToken = textfieldHomeAssistantPassword.text;
            connectionLoaderText.text = "Connecting ..."
            connectionLoaderSmallText.text = "";
            connectionLoader.state = "connecting";
            socket.active = true;
//            mainSwipeview.currentIndex += 1
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
                loaderWifiStep2.source = "qrc:/WifiStep2.qml"
            }
        }
    }

}


