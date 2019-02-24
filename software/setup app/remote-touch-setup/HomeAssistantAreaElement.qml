import QtQuick 2.9
import QtQuick.Controls 2.2

Item {
    property alias roomTitle: roomTitle.text
    property alias buttonContainer: buttonContainer
    width: parent.width

    Text {
        id: roomTitle
        color: colorText
        text: "Room name"
        width: parent.width
        elide: Text.ElideRight
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 60
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 52
        lineHeight: 0.8
    }

    Flow {
        id: buttonContainer
        width: parent.width
        anchors.top: roomTitle.bottom
        anchors.topMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 6

        //buttons come here

    }
}
