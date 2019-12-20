import QtQuick 2.11

Rectangle {
    id: main
    width: parent.width
    height: 100+(80*list.length)
    radius: cornerRadius
    color: colorBackground

    property var list: []
    property string id
    property string type

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
                        if (list[index] == "PLAY")
                            return "\uE903"
                        else if (list[index] == "QUEUE")
                            return "\uE907"
                        else if (list[index] == "SONGRADIO")
                            return "\uE90A"
                        else
                            return ""
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
                    text: {
                        if (list[index] == "PLAY")
                            return "Play"
                        else if (list[index] == "QUEUE")
                            return "Add to queue"
                        else if (list[index] == "SONGRADIO")
                            return "Go to song radio"
                        else
                            return "Not supported"
                    }
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
                        if (list[index] == "PLAY")
                            obj.playMedia(id, type);
                        else if (list[index] == "QUEUE")
                            obj.addToQueue(id, type);
                        else if (list[index] == "SONGRADIO")
                            obj.songRadio(id, type);
                        else
                            return "Not supported"

                        main.state = "closed";
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
