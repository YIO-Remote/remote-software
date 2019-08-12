import QtQuick 2.11
import QtQuick.Controls 2.4

import Launcher 1.0

import "qrc:/scripts/helper.js" as JSHelper
import "qrc:/basic_ui" as BasicUI

Item {
    width: parent.width
    height: header.height + section.height + 20

    Launcher {
        id: remoteConfigLauncher
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DISPLAY
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text {
        id: header
        color: colorText
        text: qsTr("Remote configuration") + translateHandler.emptyString
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: section
        width: parent.width
        height: childrenRect.height + 40 //remoteConfigText.height + smallText.height + 60
        radius: cornerRadius
        color: colorDark

        anchors.top: header.bottom
        anchors.topMargin: 20

        Text {
            id: remoteConfigText
            color: colorText
            text: qsTr("Remote configuration") + translateHandler.emptyString
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
            id: remoteConfigButton

            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: remoteConfigText.verticalCenter

            checked: false
            mouseArea.onClicked: {
                if (remoteConfigButton.checked) {
                    remoteConfigLauncher.launch("systemctl start lighttpd.service");
                    remoteConfigButton.checked = true;
                } else {
                    remoteConfigLauncher.launch("systemctl stop lighttpd.service");
                    remoteConfigButton.checked = false;
                }
            }
        }


        Text {
            id: smallText
            color: colorText
            opacity: 0.5
            text: qsTr("Use your browser to configure your YIO remote or download and upload backups. Navigate your internet browser to:\n\n") + "http://" + remoteConfigLauncher.launch("/usr/bin/yio-remote/wifi_ip.sh").trim() + translateHandler.emptyString
            wrapMode: Text.WordWrap
            width: parent.width - 40 - remoteConfigButton.width
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: remoteConfigButton.bottom
            anchors.topMargin: 10
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }
    }
}
