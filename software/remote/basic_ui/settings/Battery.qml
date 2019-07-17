import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

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
        getHours();
    }

    property var hours: []

    function getHours() {
        if (hours === [] ) {
            hours.push(battery_data[battery_data.length-1].timestamp.getHours());
        }

        var tmp;
        tmp = hours;

        for (var i=battery_data.length-1; i>0; i--) {
            if ( hours.indexOf(battery_data[i].timestamp.getHours()) == -1 ) {
                tmp.push(battery_data[i].timestamp.getHours());
            }
        }
        hours = tmp;
    }

    Connections {
        target: applicationWindow

        onBatteryDataUpdated: {
            getHours();
        }

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
        color: colorDark

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
            color: colorHighlight1
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
            color: colorHighlight1
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

        Rectangle {
            id: line2
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: screenOnTimeTextData.bottom
            anchors.topMargin: 20
        }

        Text {
            id: batteryLevelText
            color: colorHighlight1
            text: qsTr("Battery level") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line2.bottom
            anchors.topMargin: 10
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Image {
            id: batteryLevelBG
            asynchronous: true
            width: 420
            height: 120
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/settings/battery-level.png"
            anchors.top: batteryLevelText.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter

            ColorOverlay {
                visible: !darkMode
                anchors.fill: parent
                source: parent
                color: colorText
                antialiasing: true
            }
        }

        Item {
            width: batteryLevelBG.width
            height: batteryLevelBG.height
            anchors.centerIn: batteryLevelBG

            Row {
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 13

                spacing: 4

                Repeater {
                    model: battery_data.length > 35 ? 36 : battery_data.length

                    Rectangle {
                        width: 6
                        height: 96 * battery_data[index].level
                        color: battery_data[index].power < 0 ? colorText : colorGreen
                        anchors.bottom: parent.bottom
                    }
                }
            }
        }

        Text {
            id: averagePowerText
            color: colorHighlight1
            text: qsTr("Average power") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: batteryLevelBG.bottom
            anchors.topMargin: 10
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Image {
            id: averagePowerBG
            asynchronous: true
            width: 420
            height: 120
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/settings/average-power.png"
            anchors.top: averagePowerText.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter

            ColorOverlay {
                visible: !darkMode
                anchors.fill: parent
                source: parent
                color: colorText
                antialiasing: true
            }
        }

        Item {
            width: averagePowerBG.width
            height: averagePowerBG.height
            anchors.centerIn: averagePowerBG

            Row {
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 13

                spacing: 4

                Repeater {
                    model: battery_data.length > 35 ? 36 : battery_data.length

                    Rectangle {
                        width: 6
                        height: 96 * (Math.abs(battery_data[index].power) / 6020)
                        color: battery_data[index].power < 0 ? colorText : colorGreen
                        anchors.bottom: parent.bottom
                    }
                }
            }
        }

        Item {
            width: 360
            height: 20
            anchors.top: averagePowerBG.bottom
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: 20

            RowLayout {
                layoutDirection: Qt.RightToLeft
                Layout.fillWidth: false
                spacing: 30
                anchors.left: parent.left

                Repeater {
                    model: hours.length > 7 ? 8 : (hours.length == 0 ? 1 : hours.length)

                    delegate: Text {
                        //: Battery level history data. When no data avaialable yet it shows this text under the empty graph.
                        text: hours.length == 0 ? (qsTr("No data available yet") + translateHandler.emptyString) : hours[hours.length-1-index]
                        color: colorHighlight1
                        font.family: "Open Sans"
                        font.weight: Font.Normal
                        font.pixelSize: 16
                        lineHeight: 1
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

        }
    }

}
