import QtQuick 2.11
import QtGraphicalEffects 1.0

Item {
    id: statusBar
    width: parent.width
    height: 40

    Rectangle {
        width: parent.width
        height: parent.height
        color: colorBackground
        opacity: 0.7

        MouseArea {
            anchors.fill: parent

            onClicked: {
                updaterOpen.state = "closed";
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // UPDATE PANEL
    //////////////////////////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: updaterContainer
        width: parent.width
        height: 380
        visible: false
        anchors.top: parent.top
        anchors.topMargin: -20
        color: colorBackgroundTransparent
        radius: 18

        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: Item {
                width: updaterContainer.width
                height: updaterContainer.height
                Rectangle {
                    anchors.fill: parent
                    radius: 18
                }
            }
        }

        MouseArea {
            anchors.fill: parent
        }

        Rectangle {
            id: updaterOpen
            width: 22
            height: width
            radius: width/2
            color: colorRed
            x: updater.x
            y: updater.y+20

            state: "closed"

            states: [
                State {
                    name: "closed"
                    PropertyChanges {target: updaterOpen; scale: 1}
                    PropertyChanges {target: updaterText; opacity: 1}
                    PropertyChanges {target: updateOpenText; opacity: 0}
                    PropertyChanges {target: updateVersionText; opacity: 0}
                    PropertyChanges {target: updateButton; opacity: 0}
                    PropertyChanges {target: statusBar; height: 40}
                    PropertyChanges {target: updaterContainer; visible: false}
                },
                State {
                    name: "open"
                    PropertyChanges {target: updaterOpen; scale: 50}
                    PropertyChanges {target: updaterText; opacity: 0}
                    PropertyChanges {target: updateOpenText; opacity: 1}
                    PropertyChanges {target: updateVersionText; opacity: 1}
                    PropertyChanges {target: updateButton; opacity: 1}
                    PropertyChanges {target: statusBar; height: 800}
                    PropertyChanges {target: updaterContainer; visible: true}
                }
            ]

            transitions: [
                Transition {
                    to: "closed"
                    SequentialAnimation {
                        ParallelAnimation {
                            PropertyAnimation { target: updaterOpen; properties: "scale"; easing.type: Easing.OutExpo; duration: 300 }
                            PropertyAnimation { target: updaterText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                            PropertyAnimation { target: updateOpenText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                            PropertyAnimation { target: updateVersionText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                            PropertyAnimation { target: updateButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                        }
                        PropertyAnimation { target: updaterContainer; properties: "visible"; duration: 0 }
                    }
                },
                Transition {
                    to: "open"
                    SequentialAnimation {
                        ParallelAnimation {
                            PropertyAnimation { target: updaterOpen; properties: "scale"; easing.type: Easing.InExpo; duration: 300 }
                            PropertyAnimation { target: updaterText; properties: "opacity"; easing.type: Easing.InExpo; duration: 300 }
                        }
                        ParallelAnimation {
                            PropertyAnimation { target: updateOpenText; properties: "opacity"; easing.type: Easing.InExpo; duration: 300 }
                            PropertyAnimation { target: updateVersionText; properties: "opacity"; easing.type: Easing.InExpo; duration: 300 }
                            PropertyAnimation { target: updateButton; properties: "opacity"; easing.type: Easing.InExpo; duration: 300 }
                        }
                    }
                }
            ]
        }

        Text {
            id: updateOpenText
            color: colorText
            text: qsTr("New software version is available.") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 104
            width: parent.width-160
            font.family: "Open Sans"
            font.weight: Font.Bold
            font.styleName: "Bold"
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: updateVersionText
            color: colorText
            text: _new_version
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: updateOpenText.bottom
            anchors.topMargin: 10
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        CustomButton {
            id: updateButton
            buttonText: qsTr("Update") + translateHandler.emptyString
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 60

            mouseArea.onClicked: {
                console.debug("Update");
                var command = "/usr/bin/remote/updater.sh " + updateURL;
                mainLauncher.launch(command);
                updateAvailable = false;
                updaterOpen.state = "closed";
            }
        }
    }

    Item {
        id: container
        width: parent.width
        height: 40


        //////////////////////////////////////////////////////////////////////////////////////////////////
        // UPDATE ICON
        //////////////////////////////////////////////////////////////////////////////////////////////////
        Rectangle {
            id: updater
            width: 22
            height: width
            radius: width/2
            color: colorRed
            visible: updateAvailable
            anchors.right: batteryIcon.left
            anchors.rightMargin: 10
            anchors.verticalCenter: container.verticalCenter

            Text {
                id: updaterText
                color: colorText
                text: "1"
                verticalAlignment: Text.AlignVCenter
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                font.family: "Open Sans"
                font.weight: Font.Bold
                font.styleName: "Bold"
                font.pixelSize: 20
                lineHeight: 1
            }

            MouseArea {
                width: parent.width + 60
                height: width
                anchors.centerIn: parent

                onClicked: {
                    if (updaterOpen.state == "closed") {
                        updaterContainer.visible = true;
                        updaterOpen.state = "open";
                    }
                }
            }
        }


        //////////////////////////////////////////////////////////////////////////////////////////////////
        // TIME
        //////////////////////////////////////////////////////////////////////////////////////////////////
        Timer { // timer that updates the time and date
            id: timer
            interval: 100
            repeat: true
            running: true

            onTriggered:
            {
                timeText.text =  Qt.formatDateTime(new Date(),"hh:mm")
            }
        }


        Text { // time in the left corner
            id: timeText
            color: colorText
            text: "22:00"
            verticalAlignment: Text.AlignVCenter
            height: parent.height
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: container.verticalCenter
            anchors.top: parent.top
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////
        // BATTERY ICON
        //////////////////////////////////////////////////////////////////////////////////////////////////
        Rectangle {
            id: batteryIcon
            width: 36
            anchors.right: batteryPercentText.left
            anchors.rightMargin: 5
            anchors.verticalCenter: container.verticalCenter

            // battery fill
            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 5
                id: icon_battery
                width: battery_level*20
                height: 6
                color: battery_level >= 0.200 ? colorLine : colorRed
            }
            // battery frame
            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                width: 30
                height: 18
                color: colorBackgroundTransparent
                radius: 4
                border.width: 2
                border.color: colorLine
            }
            // battery notch
            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                width: 8
                height: 10
                color: colorBackgroundTransparent
                radius: 2
                border.width: 2
                border.color: colorLine
            }
        }

        Text {
            id: batteryPercentText
            color: colorText
            text: Math.round(battery_level * 100) + "%"
            verticalAlignment: Text.AlignVCenter
            height: parent.height
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: container.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }
    }
}
