import QtQuick 2.9
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

Rectangle {
    id: element
    width: parent.width
    height: !expanded ? 100 : 360
    color: selected ? colorHighlight : colorBackground
    radius: selected ? 12 : 0
    clip: true

    function roomClicked(roomSelected) {
        for (var i=0; i<addedRoomButtons.length; i++) {
            if (addedRoomButtons[i].room == roomSelected) {
                addedRoomButtons[i].selected = true;
                roomText = roomSelected;
                for (var k=0; k<selectedEntities.length; k++) {
                    if (selectedEntities[k].entity_id == entity_id.text) {
                        selectedEntities[k].room = roomText;
                    }
                }
            } else {
                addedRoomButtons[i].selected = false;
            }
        }
    }

    property var addedRoomButtons: []

    property alias friendly_name: friendly_name.text
    property alias entity_id: entity_id.text
    property string roomText
    property alias line: line
    property alias mouseArea: mouseArea
    property bool selected: false
    property bool expanded: false
    property int id
    property var type

    Behavior on color {
        PropertyAnimation { duration: 300 }
    }

    Behavior on height {
        PropertyAnimation { duration: 300; easing.type: Easing.OutExpo }
    }

    Text {
        id: friendly_name
        color: colorText
        text: "Friendly name"
        width: 360
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 28
        lineHeight: 1
    }

    Text {
        id: entity_id
        color: colorText
        opacity: 0.75
        text: "entity_id"
        width: 360
        anchors.top: friendly_name.bottom
        anchors.topMargin: 4
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 22
        lineHeight: 1
    }

    Text {
        id: room
        color: colorText
        opacity: 0.3
        text: "Room: " + roomText
        width: 360
        anchors.top: entity_id.bottom
        anchors.topMargin: 4
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 22
        lineHeight: 1
    }

    Rectangle {
        id: line
        width: parent.width
        height: 1
        visible: !selected
        color: colorLine
        opacity: 0.15
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Rectangle {
        id: lineExpanded
        width: parent.width-20
        height: 1
        visible: expanded
        color: colorBackground
        opacity: 0.5
        anchors.top: room.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Text {
        id: changeRoom
        color: colorText
        opacity: 0.75
        text: "Change room"
        anchors.top: lineExpanded.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 22
    }

    onExpandedChanged: {
        if (expanded) {
            var comp;
            var obj;

            sortJsonArrayByProperty(homeAssistantAreas, 'name');

            for (var i=0; i<homeAssistantAreas.length; i++) {
                comp = Qt.createComponent("qrc:/RoomElement.qml");
                obj = comp.createObject(roomsFlow, {room: homeAssistantAreas[i].name});
                obj.clicked.connect(roomClicked);
                if (roomText == homeAssistantAreas[i].name) {
                    obj.selected = true;
                }
                addedRoomButtons[i] = obj;
            }
        } else {
            // remove elements
        }
    }

    Flickable {
        id: flickableRooms
        width: parent.width
        height: 170
        anchors.top: changeRoom.bottom
        anchors.topMargin: 20
        clip: true
        visible: expanded
        contentHeight: roomsFlow.height
        boundsBehavior: Flickable.DragAndOvershootBounds
        flickableDirection: Flickable.VerticalFlick
        maximumFlickVelocity: 4000
        flickDeceleration: 400

        opacity: Math.max(0.5, 1.0 - Math.abs(verticalOvershoot) / height*4)

        Flow {
            id: roomsFlow
            width: parent.width-40
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 2

        }

        OpacityMask {
            source: mask
            maskSource: flickableRooms
        }

        OpacityMask {
            source: mask2
            maskSource: flickableRooms
        }

        LinearGradient {
            id: mask
            width: parent.width
            height: 60
            opacity: flickableRooms.atYBeginning ? 0 : 1
            gradient: Gradient {
                GradientStop { position: 0; color: colorHighlight }
                GradientStop { position: 1; color: "transparent"}
            }

            Behavior on opacity {
                PropertyAnimation { duration: 300 }
            }
        }

        LinearGradient {
            id: mask2
            width: parent.width
            height: 60
            opacity: flickableRooms.atYEnd ? 0 : 1
            anchors.bottom: parent.bottom
            gradient: Gradient {
                GradientStop { position: 1; color: colorHighlight }
                GradientStop { position: 0; color: "transparent"}
            }

            Behavior on opacity {
                PropertyAnimation { duration: 300 }
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: !expanded

        onClicked: {
            if (!selected) {
                selected = true;
                selectedEntitiesCounter += 1;
                var tmp = {}
                tmp.entity_id = entity_id.text
                tmp.type = type
                tmp.room = roomText
                selectedEntities.push(tmp)
                //                console.log(JSON.stringify(selectedEntities))
            } else {
                selected = false;
                selectedEntitiesCounter -= 1;
                for (var i=0; i<selectedEntities.length; i++) {
                    if (selectedEntities[i].entity_id == entity_id.text) {
                        selectedEntities.splice(i, 1);
                    }
                }
                //                console.log(JSON.stringify(selectedEntities))
            }
        }
    }

    Rectangle {
        id: expandButton
        width: 30
        height: 30
        radius: 15
        color: colorBackground
        visible: selected
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10

        Rectangle {
            id: dot
            width: 4
            height: width
            color: colorLine
            radius: 2
            anchors.centerIn: parent
        }

        Rectangle {
            width: 4
            height: width
            color: colorLine
            radius: 2
            anchors.verticalCenter: dot.verticalCenter
            anchors.right: dot.left
            anchors.rightMargin: 4
        }

        Rectangle {
            width: 4
            height: width
            color: colorLine
            radius: 2
            anchors.verticalCenter: dot.verticalCenter
            anchors.left: dot.right
            anchors.leftMargin: 4
        }

        MouseArea {
            width: parent.width + 30
            height: width
            anchors.centerIn: parent

            onClicked: {
                if (!expanded) {
                    expanded = true;
                } else {
                    expanded = false;
                }
            }
        }
    }
}
