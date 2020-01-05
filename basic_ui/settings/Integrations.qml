/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

import QtQuick 2.11
import QtQuick.Controls 2.5

import Haptic 1.0

import Style 1.0
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
                color: Style.colorText
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 27 }
            }

            Rectangle {
                width: parent.width
                height: 2
                color: Style.colorBackground

                anchors.bottom: parent.bottom
            }

            Rectangle {
                id: circleButton
                width: 36
                height: 36
                color: Style.colorLine
                radius: width/2
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 20

                Rectangle {
                    width: 4
                    height: 4
                    color: Style.colorBackground
                    radius: width/2
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    width: 4
                    height: 4
                    color: Style.colorBackground
                    radius: width/2
                    anchors.centerIn: parent
                }

                Rectangle {
                    width: 4
                    height: 4
                    color: Style.colorBackground
                    radius: width/2
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        Haptic.playEffect(Haptic.Click);
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
                    color: Style.colorLine
                    radius: Style.cornerRadius
                }

                Item {
                    width: parent.width
                    height: 62
                    anchors.top: parent.top

                    Text {
                        text: qsTr("Connect") + translateHandler.emptyString
                        color: Style.colorBackground
                        opacity: obj.state == 0 ? 0.5 : 1
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 27 }
                    }

                    MouseArea {
                        anchors.fill: parent
                        enabled: obj.state == 0 ? false : true

                        onClicked: {
                            Haptic.playEffect(Haptic.Click);
                            obj.connect();
                            popup.close();
                        }
                    }
                }


                Rectangle {
                    width: parent.width
                    height: 2
                    color: Style.colorBackground
                    anchors.verticalCenter: parent.verticalCenter
                }

                Item {
                    width: parent.width
                    height: 62
                    anchors.bottom: parent.bottom
                    opacity: obj.state == 2 ? 0.5 : 1

                    Text {
                        text: qsTr("Disconnect") + translateHandler.emptyString
                        color: Style.colorBackground
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 27 }
                    }

                    MouseArea {
                        anchors.fill: parent
                        enabled: obj.state == 2 ? false : true

                        onClicked: {
                            Haptic.playEffect(Haptic.Click);
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
        color: Style.colorText
        text: qsTr("Integrations") + translateHandler.emptyString
        anchors.left: parent.left
        font.family: "Open Sans Regular"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: section
        width: parent.width
        height: childrenRect.height + 40
        radius: Style.cornerRadius
        color: Style.colorDark

        anchors.top: header.bottom
        anchors.topMargin: 20

        Component.onCompleted: {
//            var comp;
//            var obj;
//            for (var key in integration) {
//                if (integration.hasOwnProperty(key)) {
//                    integrationComponent.createObject(column, { title: integration[key].friendly_name, obj: integration[key].obj });
//                }
//            }
            for (var i=0; i<integrations.list.length; i++) {
                integrationComponent.createObject(column, { title: integrations.getFriendlyName(integrations.list[i]), obj: integrations.list[i] });
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
