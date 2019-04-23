import QtQuick 2.0

Rectangle {
    id: blindButton

    property int componentID
    property int entityID

//    property var entity_id: entities_blind[entityID].entity_id
//    property var friendly_name: entities_blind[entityID].friendly_name
//    property var lstate: entities_blind[entityID].state
//    property int brightness: entities_blind[entityID].brightness
//    property string integrationType: entities_blind[entityID].integration

    property bool favorite: false

    width: parent.width
    height: 125
    color: colorMedium
    radius: cornerRadius

}
