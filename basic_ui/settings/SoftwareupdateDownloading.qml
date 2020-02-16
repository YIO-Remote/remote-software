import QtQuick 2.11
import Style 1.0

import "qrc:/basic_ui" as BasicUI

import Haptic 1.0
import SoftwareUpdate 1.0

Rectangle {
    id: container
    width: parent.width; height: parent.height
    color: Style.colorBackground

    Connections {
        target: SoftwareUpdate
        onDownloadComplete: {
            titleText.text = qsTr("Download completed") + translateHandler.emptyString
            smalltext.text = SoftwareUpdate.bytesTotal + " bytes"
            updateButton.opacity = 1;
        }
    }

    Rectangle {
        id: progressBg
        width: parent.width - 60; height: 4
        color: Style.colorDark
        anchors { top: parent.top; topMargin: 380; horizontalCenter: parent.horizontalCenter }
    }

    Rectangle {
        width: SoftwareUpdate.bytesReceived/SoftwareUpdate.bytesTotal * progressBg.width
        height: 4
        color: Style.colorLine
        anchors { left: progressBg.left; top: progressBg.top }

        Behavior on width {
            NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
        }
    }

    Text {
        id: titleText
        color: Style.colorText
        text: qsTr("Downloading") + translateHandler.emptyString
        horizontalAlignment: Text.AlignHCenter
        anchors { top: progressBg.bottom; topMargin: 30; horizontalCenter: parent.horizontalCenter }
        font: Style.buttonFont
    }

    Text {
        id: smalltext
        color: Style.colorText
        opacity: 0.5
        text: SoftwareUpdate.downloadSpeed
        horizontalAlignment: Text.AlignHCenter
        anchors { top: titleText.bottom; topMargin: 10; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 20 }
        lineHeight: 1
    }

    Item {
        width: childrenRect.width; height: childrenRect.height
        anchors { bottom: parent.bottom; bottomMargin: 60; horizontalCenter: parent.horizontalCenter }

        BasicUI.CustomButton {
            id: updateButton
            opacity: 0
            buttonText: qsTr("Update") + translateHandler.emptyString
            mouseArea.onClicked: {
                Haptic.playEffect(Haptic.Click);
                // now perform update
            }

            Behavior on opacity {
                NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
            }
        }

        BasicUI.CustomButton {
            id: cancelButton
            anchors { left: updateButton.right; leftMargin: updateButton.opacity == 0 ? -updateButton.width : 30 }
            buttonText: qsTr("Cancel") + translateHandler.emptyString
            mouseArea.onClicked: {
                Haptic.playEffect(Haptic.Click);
                loader_second.source = "";
            }

            Behavior on opacity {
                NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
            }

            Behavior on anchors.leftMargin {
                NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
            }
        }
    }
}
