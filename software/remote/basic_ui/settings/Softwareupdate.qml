import QtQuick 2.11
import QtQuick.Controls 2.4

import "qrc:/scripts/helper.js" as JSHelper

Item {
    width: parent.width
    height: header.height + section.height + 20

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DISPLAY
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text {
        id: header
        color: colorText
        text: qsTr("Software update") + translateHandler.emptyString
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: section
        width: parent.width
        height: softwareUpdateText.height + smallText.height + 60
        radius: cornerRadius
        color: colorMedium

        anchors.top: header.bottom
        anchors.topMargin: 20

        Text {
            id: softwareUpdateText
            color: colorText
            text: qsTr("Keep the remote updated") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Switch {
            id: softwareUpdateButton
            implicitHeight: 36
            implicitWidth: 66
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: softwareUpdateText.verticalCenter
            checked: config.settings.softwareupdate

            indicator: Rectangle {
                x: softwareUpdateButton.visualPosition * (softwareUpdateButton.width - width)
                y: (softwareUpdateButton.height - height) / 2
                width: 36
                height: 36
                radius: cornerRadius
                color: softwareUpdateButton.checked ? colorSwitchOn : colorSwitch

                Behavior on x {
                    enabled: !softwareUpdateButton.pressed
                    SmoothedAnimation { velocity: 150 }
                }
            }

            background: Rectangle {
                radius: cornerRadius+2
                color: softwareUpdateButton.checked ? colorHighlight : colorSwitchBackground
            }

            onClicked: {
                var tmp = config;
                tmp.settings.softwareupdate = !tmp.settings.softwareupdate
                config = tmp;
                JSHelper.saveConfig();
            }
        }


        Text {
            id: smallText
            color: colorText
            opacity: 0.5
            text: qsTr("Automatically look for updates and update when a new software version is available.") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            width: parent.width - 40 - softwareUpdateButton.width
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: softwareUpdateButton.bottom
            anchors.topMargin: 10
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }
    }
}
