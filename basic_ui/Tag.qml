import QtQuick 2.0

Rectangle {
    id: main
    width: title.implicitWidth + 40
    height: 40
    color: selected ? colorLine : colorBackgroundTransparent
    radius: height/2
    border.color: colorLine
    border.width: 2
    opacity: selected ? 1 : 0.5

    Behavior on opacity {
        PropertyAnimation {
            duration: 300
            easing.type: Easing.OutExpo
        }
    }

    property bool selected: false
    property alias tag: title.text

    Text {
        id: title
        color: selected ? colorBackground : colorText
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.family: "Open Sans"
        font.pixelSize: 20
        lineHeight: 1
        anchors.centerIn: parent
    }

}
