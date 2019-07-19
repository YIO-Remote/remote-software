import QtQuick 2.11
import QtQuick.Controls 2.4

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
        height: childrenRect.height + 40 //softwareUpdateText.height + smallText.height + 60
        radius: cornerRadius
        color: colorDark

        anchors.top: header.bottom
        anchors.topMargin: 20

        Text {
            id: softwareUpdateText
            color: colorText
            text: qsTr("Auto update") + translateHandler.emptyString
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
            id: softwareUpdateButton

            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: softwareUpdateText.verticalCenter

            checked: config.settings.softwareupdate
            mouseArea.onClicked: {
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
