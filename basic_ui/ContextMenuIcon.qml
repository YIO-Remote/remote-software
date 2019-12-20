import QtQuick 2.11

Rectangle {
    id: circleButton
    width: 30
    height: 30
    color: colorLine
    radius: width/2

    property var colorBg: colorBackground
    property alias mouseArea: mouseArea


    Rectangle {
        width: 4
        height: 4
        color: colorBg
        radius: width/2
        anchors.left: parent.left
        anchors.leftMargin: 8
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle {
        width: 4
        height: 4
        color: colorBg
        radius: width/2
        anchors.centerIn: parent
    }

    Rectangle {
        width: 4
        height: 4
        color: colorBg
        radius: width/2
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
        id: mouseArea
        width: parent.width+20
        height: parent.height+20
        anchors.centerIn: parent

        onClicked: {
            haptic.playEffect("click");
        }
    }
}
