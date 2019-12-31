import QtQuick 2.11

Rectangle {
    id: main
    width: parent.width
    height: 100+(80*list.length)
    radius: cornerRadius
    color: colorBackground

    property var list: []
    property var climateObj

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state: "closed"

    Component.onCompleted: {
        state = "open";
    }


    states: [
        State { name: "closed";
            PropertyChanges {target: main; y: main.height; enabled: false }
        },
        State { name: "open";
            PropertyChanges {target: main; y: 0; enabled: true }
        }
    ]
    transitions: [
        Transition {to: "closed";
            SequentialAnimation {
                PropertyAnimation { target: main; properties: "y"; easing.type: Easing.OutExpo; duration: 300 }
            }
        },
        Transition {to: "open";
            SequentialAnimation {
                PropertyAnimation { target: main; properties: "y"; easing.type: Easing.OutExpo; duration: 300 }
            }
        }
    ]

    Flow {
        anchors.fill: parent
        spacing: 0

        Item {
            width: main.width
            height: 20
        }

        Repeater {
            model: list

            Item {
                width: main.width
                height: 80

                Text {
                    id: icon
                    color: colorText
                    text: {
                        if (list[index] === "Off")
                            return "\uE908"
                        else if (list[index] === "Heat")
                            return "\uE91F"
                        else if (list[index] === "Cool")
                            return "\uE91E"
                    }
                    renderType: Text.NativeRendering
                    width: 70
                    height: 70
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font { family: "icons"; pixelSize: 80 }
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    text: qsTr(list[index]) + translateHandler.emptyString
                    color: colorText
                    anchors.left: icon.right
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    font.family: "Open Sans"
                    font.pixelSize: 27
                    lineHeight: 1
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        haptic.playEffect("click");
                        main.state = "closed";
                        if (list[index] === "Off")
                            climateObj.turnOff();
                        if (list[index] === "Heat")
                            climateObj.heat();
                        if (list[index] === "Cool")
                            climateObj.cool();
                    }
                }
            }
        }

        Item {
            width: main.width
            height: 80

            Text {
                text: qsTr("Cancel") + translateHandler.emptyString
                color: colorText
                opacity: 0.5
                anchors.centerIn: parent
                font.family: "Open Sans"
                font.pixelSize: 25
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                lineHeight: 1
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    haptic.playEffect("click");
                    main.state = "closed";
                }
            }
        }
    }
}
