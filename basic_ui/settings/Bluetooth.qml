import QtQuick 2.11
import QtQuick.Controls 2.5

import "qrc:/scripts/helper.js" as JSHelper
import "qrc:/basic_ui" as BasicUI

Item {
    width: parent.width
    height: header.height + section.height + 20

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DISPLAY
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text {
        id: header
        color: colorText
        text: qsTr("Bluetooth") + translateHandler.emptyString
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: section
        width: parent.width
        height: childrenRect.height + 40 //bluetoothText.height + smallText.height + 60
        radius: cornerRadius
        color: colorDark

        anchors.top: header.bottom
        anchors.topMargin: 20

        Text {
            id: bluetoothText
            color: colorText
            text: qsTr("Area detection") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        BasicUI.CustomSwitch {
            id: bluetoothButton

            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: bluetoothText.verticalCenter

            checked: config.settings.bluetootharea
            mouseArea.onClicked: {
                if (bluetoothButton.checked) {
                 bluetoothArea.stopScan();
                 bluetoothArea.turnOff();
                } else {
                    bluetoothArea.startScan();
                }

                var tmp = config.config
                tmp.settings.bluetootharea = !tmp.settings.bluetootharea
                config.config = tmp;
                config.writeConfig();
            }
        }


        Text {
            id: smallText
            color: colorText
            opacity: 0.5
            text: qsTr("Automatically scans for bluetooth beacons associated with areas, finds the closest and displays devices in that area.") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            width: parent.width - 40 - bluetoothButton.width
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: bluetoothButton.bottom
            anchors.topMargin: 10
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }
    }
}
