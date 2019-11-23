import QtQuick 2.0
import QtQuick.Controls 2.5

import "qrc:/basic_ui" as BasicUI

Item {
    width: parent.width
    height: parent.height

    Connections {
        target: card

        onLooseFocus: {
            searchTextField.focus = false;
        }
    }

    Rectangle {
        width: parent.width - 60
        height: 80
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.horizontalCenter: parent.horizontalCenter
        color: colorText
        radius: cornerRadius

        Text {
            id: searchIcon
            color: colorBackground
            text: "\uE90C"
            renderType: Text.NativeRendering
            width: 70
            height: 70
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    haptic.playEffect("click");
                    obj.search(searchTextField.text);
                    searchTextField.focus = false;
                }
            }
        }

        TextField {
            id: searchTextField
            cursorVisible: false
            width: parent.width-90
            height: parent.height
            anchors.left: searchIcon.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            placeholderText: qsTr("Search") + translateHandler.emptyString
            color: colorBackground
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27

            background: Rectangle {
                color: colorBackgroundTransparent
                border.width: 0
            }

            onAccepted: {
                obj.search(searchTextField.text)
            }

            onFocusChanged: {
                if (focus)
                    inputPanel.active = true
                else
                    inputPanel.active = false
            }
        }
    }
}
