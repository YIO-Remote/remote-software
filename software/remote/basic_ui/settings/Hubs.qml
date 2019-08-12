import QtQuick 2.11
import QtQuick.Controls 2.4

import "qrc:/basic_ui" as BasicUI

Item {
    width: parent.width
    height: header.height + section.height + 20

    Component {
        id: integrationComponent

        Item {
            width: 460
            height: 70
            anchors.horizontalCenter: parent.horizontalCenter

            property var title
            property var obj

            Text {
                width: parent.width - 100
                text: obj.state == 0 ? title : title + qsTr(" - Disconnected") + translateHandler.emptyString
                elide: Text.ElideRight
                color: colorText
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                font { family: "Open Sans"; weight: Font.Normal; pixelSize: 27 }
            }

            Rectangle {
                width: parent.width
                height: 2
                color: colorBackground

                anchors.bottom: parent.bottom
            }

            Rectangle {
                id: circleButton
                width: 36
                height: 36
                color: colorLine
                radius: width/2
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 20

                Rectangle {
                    width: 4
                    height: 4
                    color: colorBackground
                    radius: width/2
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    width: 4
                    height: 4
                    color: colorBackground
                    radius: width/2
                    anchors.centerIn: parent
                }

                Rectangle {
                    width: 4
                    height: 4
                    color: colorBackground
                    radius: width/2
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        haptic.playEffect("click");
                        popup.open();
                    }
                }

            }

            Popup {
                id: popup
                x: circleButton.x + circleButton.width - popup.width
                y: circleButton.y
                width: 300
                height: 160
                modal: true
                focus: true
                clip: true
                closePolicy: Popup.CloseOnPressOutside

                enter: Transition {
                        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; easing.type: Easing.OutExpo; duration: 300 }
                        NumberAnimation { property: "width"; from: 0; to: 300; easing.type: Easing.OutBack; duration: 300 }
                        NumberAnimation { property: "height"; from: 0; to: 160; easing.type: Easing.OutBack; duration: 300 }
                    }

                exit: Transition {
                        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; easing.type: Easing.InExpo; duration: 300 }
                        NumberAnimation { property: "width"; from: 300; to: 0; easing.type: Easing.InExpo; duration: 300 }
                        NumberAnimation { property: "height"; from: 160; to: 0; easing.type: Easing.InExpo; duration: 300 }
                    }

                background: Rectangle {
                    anchors.fill: parent
                    color: colorLine
                    radius: cornerRadius
                }

                Item {
                    width: parent.width
                    height: 62
                    anchors.top: parent.top

                    Text {
                        text: qsTr("Connect") + translateHandler.emptyString
                        color: colorBackground
                        opacity: obj.state == 0 ? 0.5 : 1
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        font { family: "Open Sans"; weight: Font.Normal; pixelSize: 27 }
                    }

                    MouseArea {
                        anchors.fill: parent
                        enabled: obj.state == 0 ? false : true

                        onClicked: {
                            haptic.playEffect("click");
                            obj.connect();
                            popup.close();
                        }
                    }
                }


                Rectangle {
                    width: parent.width
                    height: 2
                    color: colorBackground
                    anchors.verticalCenter: parent.verticalCenter
                }

                Item {
                    width: parent.width
                    height: 62
                    anchors.bottom: parent.bottom
                    opacity: obj.state == 2 ? 0.5 : 1

                    Text {
                        text: qsTr("Disconnect") + translateHandler.emptyString
                        color: colorBackground
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        font { family: "Open Sans"; weight: Font.Normal; pixelSize: 27 }
                    }

                    MouseArea {
                        anchors.fill: parent
                        enabled: obj.state == 2 ? false : true

                        onClicked: {
                            haptic.playEffect("click");
                            obj.disconnect();
                            popup.close();
                        }
                    }
                }
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SYSTEM
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Text {
        id: header
        color: colorText
        text: qsTr("Hubs") + translateHandler.emptyString
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

        Component.onCompleted: {
            var comp;
            var obj;
            for (var key in integration) {
                if (integration.hasOwnProperty(key)) {
                    integrationComponent.createObject(column, { title: integration[key].friendly_name, obj: integration[key].obj });
                }
            }
        }

        Column {
            id: column
            spacing: 20
            height: childrenRect.height

        }


        BasicUI.CustomButton {
            id: buttonAdd
            buttonText: qsTr("Add new") + translateHandler.emptyString
            anchors.top: column.bottom
            anchors.topMargin: 30
            anchors.left: parent.left
            anchors.leftMargin: 20

            mouseArea.onClicked: {
            }
        }
    }
}
