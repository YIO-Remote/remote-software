import QtQuick 2.0

Rectangle {
    id: blindButton

    property var entity_id
    property var state
    property int brightness

    property bool favorite: false

    width: parent.width
    height: 125
    color: colorMedium
    radius: cornerRadius

}
