import QtQuick 2.11

import Entity.Remote 1.0

Rectangle {
    id: bg
    width: 110
    height: 60
    radius: height/2
    border.color: colorLight
    border.width: 2
    color: colorBackgroundTransparent

    property alias title: title.text
    property alias mouseArea: mouseArea

    Text {
        id: title
        color: colorText
        width: parent.width-20
        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.centerIn: parent
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 24
        lineHeight: 1
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            haptic.playEffect("click");
        }
    }
} // Rectangle end
