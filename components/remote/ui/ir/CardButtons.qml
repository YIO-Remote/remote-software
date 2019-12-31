import QtQuick 2.11

import Entity.Remote 1.0

Item {
    width: parent.width - 60
    anchors.horizontalCenter: parent.horizontalCenter

    Rectangle {
        id: bg
        visible: obj.isSupported(Remote.F_POWER_TOGGLE) || (obj.isSupported(Remote.F_POWER_ON) && obj.isSupported(Remote.F_POWER_OFF))
        width: 110
        height: 60
        radius: height/2
        color: colorRed
        anchors.top: parent.top
        anchors.left: parent.left

        Text {
            color: colorLine
            text: "\uE908"
            renderType: Text.NativeRendering
            width: 70
            height: 70
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 60 }
            anchors.centerIn: parent
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                haptic.playEffect("click");
                if (obj.isSupported(Remote.F_POWER_TOGGLE)) {
                    obj.powerToggle();
                } else if (obj.isSupported(Remote.F_POWER_ON) && obj.isSupported(Remote.F_POWER_OFF)) {
                    if (obj.isOn)
                        obj.powerOff();
                    else
                        obj.powerOn();
                }
            }
        }
    } // Rectangle end

    Button {
        visible: obj.isSupported(Remote.F_SOURCE)
        anchors.top: parent.top
        anchors.right: parent.right
        title: qsTr("Source") + translateHandler.emptyString
        mouseArea.onClicked: {
            obj.source();
        }
    }

    Button {
        visible: obj.isSupported(Remote.F_INFO)
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        title: qsTr("Info") + translateHandler.emptyString
        mouseArea.onClicked: {
            obj.info();
        }
    }

    Button {
        visible: obj.isSupported(Remote.F_MENU)
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        title: qsTr("Menu") + translateHandler.emptyString
        mouseArea.onClicked: {
            obj.menu();
        }
    }

    Button {
        visible: obj.isSupported(Remote.F_GUIDE)
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        title: qsTr("Guide") + translateHandler.emptyString
        mouseArea.onClicked: {
            obj.guide();
        }
    }

}



