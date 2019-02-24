import QtQuick 2.9
import QtQuick.Controls 2.2

Rectangle {
    id: element
    width: parent.width
    height: 56
    color: selected ? colorHighlight : colorBackground
    radius: selected ? 12 : 0

    property alias ssid: textSSID.text
    property alias line: line
    property alias mouseArea: mouseArea
    property bool selected: false
    property int buttonId
    property var rssi

    signal clicked(int buttonId);

    Behavior on color {
        PropertyAnimation { duration: 400 }
    }

    Text {
        id: textSSID
        color: colorText
        text: "Network SSID"
        width: 370
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 4
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 28
        lineHeight: 1
    }

    Image {
        asynchronous: true
        width: 30
        height: 30
        fillMode: Image.PreserveAspectFit
        source: rssi > -60 ? "qrc:/images/icon-wifi-3.png" : ( rssi < -60 && rssi > -80 ? "qrc:/images/icon-wifi-2.png" : "qrc:/images/icon-wifi-1.png")
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        id: line
        width: parent.width
        height: 1
        visible: !selected
        color: colorLine
        opacity: 0.15
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            parent.clicked(parent.buttonId)
//            if (!selected) {
//                selected = true
//            } else {
//                selected = false
//            }
        }
    }
}
