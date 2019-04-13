import QtQuick 2.0

Rectangle {
    id: lightButton

    property var entity_id
    property var friendly_name
    property var lstate
    property int brightness

    property bool favorite: false

    width: parent.width
    height: 125
    color: colorMedium
    radius: cornerRadius

    Text {
        id: title
        color: colorText
        text: friendly_name
        wrapMode: Text.WordWrap
        width: parent.width
        anchors.left: parent.left
        anchors.leftMargin: 20
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

}
