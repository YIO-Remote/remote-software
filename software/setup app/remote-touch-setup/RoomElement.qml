import QtQuick 2.9
import QtQuick.Controls 2.2

Rectangle {
    id: element
    width: parent.width
    height: 40
    color: colorBackgroundTransparent

    property alias room: roomName.text
    property bool selected: false
    property string roomSelected: roomName.text

    signal clicked(string roomSelected);

    Text {
        id: roomName
        color: colorText
        text: "Room"
        opacity: selected ? 1 : 0.5
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 4
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: "Neuzeit Grotesk"
        font.weight: !selected ? Font.Normal : Font.Bold
        font.styleName: !selected ? "Normal" : "Bold"
        font.pixelSize: 28
        lineHeight: 1
    }

    MouseArea {
        id: mouseArae
        anchors.fill: parent

        onClicked: {
//            if (!selected) {
//                selected = true
                parent.clicked(roomName.text)
//            }
        }
    }
}
