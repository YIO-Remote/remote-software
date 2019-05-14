import QtQuick 2.11
import QtQuick.Controls 2.4

import Launcher 1.0

import "qrc:/scripts/helper.js" as JSHelper

Item {
    width: parent.width
    height: header.height + section.height + 20

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BATTERY
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Component.onCompleted: {
        screenOnTimeTextData.text = Qt.binding( function() { return JSHelper.secondsToHours(Math.round(standbyControl.screenOnTime/1000)) } )
        screenOffTimeTextData.text = Qt.binding( function () { return JSHelper.secondsToHours(Math.round(standbyControl.screenOffTime/1000)) } )
    }

    Text {
        id: header
        color: colorText
        text: qsTr("Battery") + translateHandler.emptyString
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: section
        width: parent.width
        height: childrenRect.height + 40
        radius: cornerRadius
        color: colorMedium

        anchors.top: header.bottom
        anchors.topMargin: 20

        Text {
            id: batteryhealthText
            color: colorText
            text: qsTr("Battery health") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            color: colorText
            text: battery_health + "%"
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: batteryhealthText.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line1
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: batteryhealthText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: screenOnTimeText
            color: colorHighlight
            text: qsTr("Screen on") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line1.bottom
            anchors.topMargin: 10
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Text {
            id: screenOnTimeTextData
            color: colorText
            horizontalAlignment: Text.AlignRight
            anchors.left: screenOnTimeText.left
            anchors.top: screenOnTimeText.bottom
            anchors.topMargin: 10
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: screenOffTimeText
            color: colorHighlight
            text: qsTr("Screen off") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            anchors.left: parent.left
            anchors.leftMargin: parent.width/2
            anchors.top: line1.bottom
            anchors.topMargin: 10
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Text {
            id: screenOffTimeTextData
            color: colorText
            horizontalAlignment: Text.AlignRight
            anchors.left: screenOffTimeText.left
            anchors.top: screenOffTimeText.bottom
            anchors.topMargin: 10
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }
    }

}
