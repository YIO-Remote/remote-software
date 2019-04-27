import QtQuick 2.11
import QtQuick.Controls 2.4

Flickable {
    id: itemFlickable
    property string type

    width: parent.width
    height: parent.height
    maximumFlickVelocity: 4000
    flickDeceleration: 2000
    clip: true
    contentHeight: repeaterFlow.height
    boundsBehavior: Flickable.DragAndOvershootBounds
    flickableDirection: Flickable.VerticalFlick

    ScrollBar.vertical: ScrollBar {
        opacity: 0.5
    }

    Flow {
        id: repeaterFlow
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10

        Component.onCompleted: {
            // load room entities
            // go through all entities, if it matches the room, create a component
            ////            for (var i=0  i<loaded_entities.length; i++) {
            //                for (var j=0; j<loaded_components[type].entities.length; j++) {
            //                    if (loaded_components[type].entities[j].type == loaded_entities[index]) {
            //                        // load entity button
            //                        var comp = Qt.createComponent("qrc:/components/"+ loaded_entities[i] +"/Button.qml");
            //                        var obj = comp.createObject(repeaterFlow, {entityID: j});
            //                    }
            //                }
            ////            }
            //        }
            for (var i=0; i<loaded_components[type].entities.length; i++) {
                var comp = Qt.createComponent("qrc:/components/"+ type +"/Button.qml");
                var obj = comp.createObject(repeaterFlow, {entityID: i});
            }
        }
    }
}
