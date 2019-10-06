import QtQuick 2.11
import QtQuick.Controls 2.5

import "qrc:/basic_ui" as BasicUI

Item {
    width: parent.width
    height: childrenRect.height

    property string groupID

    // HEADER
    Item {
        id: header
        width: parent.width
        height: 80

        Text {
            color: colorText
            text: qsTr(config.read.ui_config.groups[groupID].name) + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 32
            lineHeight: 1
        }

        property alias customSwitch: customSwitch

        BasicUI.CustomSwitch {
            id: customSwitch
            visible: config.read.ui_config.groups[groupID].switch
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter

            checked: false
            mouseArea.onClicked: {
            }
        }
    }

    // ENTITIES
    ListView {
        width: parent.width
        height: contentHeight
        anchors.top: header.bottom
        interactive: false
        spacing: 10

        model: config.read.ui_config.groups[groupID].entities
        delegate: entityDelegate
    }

    // LISTVIEW DELEGATE
    Component {
        id: entityDelegate

        Loader {
            id: entityLoader
            width: 460
            height: 125
            anchors.horizontalCenter: parent.horizontalCenter

            Component.onCompleted: {
                var e = entities.get(config.read.ui_config.groups[groupID].entities[index]);
                this.setSource("qrc:/components/"+ e.type +"/ui/Button.qml", { "obj": e });
            }
        }
    }

}
