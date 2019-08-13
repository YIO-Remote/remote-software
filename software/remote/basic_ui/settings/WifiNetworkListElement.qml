import QtQuick 2.11
import QtQuick.Controls 2.4

Rectangle {
    id: element
    width: parent.width
    height: 80
    color: selected ? colorHighlight : colorLine
    radius: cornerRadius

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
        color: colorBackground
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

//    Image {
//        asynchronous: true
//        source: rssi > -60 ? "qrc:/images/icon-wifi-3.png" : ( rssi < -60 && rssi > -80 ? "qrc:/images/icon-wifi-2.png" : "qrc:/images/icon-wifi-1.png")
//        anchors.right: parent.right
//        anchors.rightMargin: 10
//        anchors.verticalCenter: parent.verticalCenter
//    }

    Rectangle {
        id: line
        width: parent.width
        height: 1
        visible: !selected
        color: colorBackground
//        opacity: 0.15
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            parent.clicked(parent.buttonId)
        }
    }
}
