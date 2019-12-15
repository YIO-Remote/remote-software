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
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: notification
    width: parent.width-20
    height: 104 // 244
    x: 10
    radius: cornerRadius
    color: type ? colorRed : colorLight

    Behavior on opacity {
        NumberAnimation {
            duration: 300
            easing.type: Easing.OutExpo
        }
    }

    Behavior on x {
        PropertyAnimation {
            duration: 300
            easing.type: Easing.OutExpo
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // MOUSEAREA
    //////////////////////////////////////////////////////////////////////////////////////////////////

    property bool dragged: false
    property bool closing: false

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        drag.target: notification.state != "open" ? notification : null
        drag.axis: Drag.XAxis
        drag.minimumX: 10
        drag.maximumX: 400

        onClicked: {
            notificationRemoverTimer.stop();
            notification.state = "open";
            if (notificationsDrawer.position > 0) {
                notificationsDrawer.height += 176;
                if (notificationsDrawer.height >= 100 + 5 * 104 ) {
                    notificationsDrawer.height = 100 + 5 * 104;
                }
            } else {
                for (var i=idN+1; i<notifications.length; i++) {
                    notificationObj[i].y += 176;
                }
            }
        }
    }

    onXChanged: {
        if (x > 240 && !dragged) {
            dragged = true;
            notificationRemoverTimer.stop();
            notification.opacity = 0;
            if (state == "permanent") {
                notifications.remove(idN);
            } else {
                notification.destroy(400);
                notifications.remove(idN);
            }
        } else if (x > 40 && x <= 240 && !closing) {
            notification.x = 100;
            notificationRemoverTimer.stop();
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // PROPERTIES FOR THE NOTIFICATION
    //////////////////////////////////////////////////////////////////////////////////////////////////

    property alias text: notificationText.text
    property bool type
    property var action
    property string actionlabel
    property var timestamp
    property int idN
    property string _state

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    //////////////////////////////////////////////////////////////////////////////////////////////////

    state: "hidden"

    Component.onCompleted: {
        notification.state = _state
    }

    states: [
        State {
            name: "visible"
            PropertyChanges { target: notification; opacity: 1; y: 20}
            PropertyChanges { target: timestampText; opacity: 0 }
            PropertyChanges { target: buttons; opacity: 0; enabled: false }
        },
        State {
            name: "hidden"
            PropertyChanges { target: notification; opacity: 0; y: -200 }
            PropertyChanges { target: timestampText; opacity: 0 }
            PropertyChanges { target: buttons; opacity: 0; enabled: false }
        },
        State {
            name: "permanent"
            PropertyChanges { target: timestampText; opacity: 0 }
            PropertyChanges { target: buttons; opacity: 0; enabled: false }
        },
        State {
            name: "open"
            PropertyChanges { target: notification; height: 280; y:20 }
            PropertyChanges { target: notificationText; height: 120 }
            PropertyChanges { target: timestampText; opacity: 0.5 }
            PropertyChanges { target: buttons; opacity: 1; enabled: true }
        }
    ]

    transitions: [
        Transition {
            to: "visible"
            PropertyAnimation { target: notification; properties: "opacity, y"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: timestampText; properties: "opacity"; easing.type: Easing.InExpo; duration: 300 }
            PropertyAnimation { target: buttons; properties: "opacity"; easing.type: Easing.InExpo; duration: 300 }
        },
        Transition {
            to: "hidden"
            PropertyAnimation { target: notification; properties: "opacity, y"; easing.type: Easing.InExpo; duration: 300 }
            PropertyAnimation { target: timestampText; properties: "opacity"; easing.type: Easing.InExpo; duration: 300 }
            PropertyAnimation { target: buttons; properties: "opacity"; easing.type: Easing.InExpo; duration: 300 }
        },
        Transition {
            to: "open"
            PropertyAnimation { target: notification; properties: "height"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: timestampText; properties: "opacity"; easing.type: Easing.InExpo; duration: 300 }
            PropertyAnimation { target: buttons; properties: "opacity"; easing.type: Easing.InExpo; duration: 300 }
        }
    ]

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // QML ELEMENTS
    //////////////////////////////////////////////////////////////////////////////////////////////////

    MouseArea {
        width: 100
        height: width
        anchors.right: parent.left
        anchors.verticalCenter: parent.verticalCenter

        enabled: notification.x == 100 ? true : false

        onClicked: {
            closing = true;
            notification.x = 400;
            notifications.remove(idN);
        }

        Image {
            id: closeIcon
            asynchronous: true
            width: 60
            height: 60
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/notification/icon-notification-dismiss.png"
            anchors.centerIn: parent

            opacity: notification.x == 100 ? 1 : 0

            ColorOverlay {
                visible: !darkMode
                anchors.fill: parent
                source: parent
                color: colorText
                antialiasing: true
            }

            Behavior on opacity {
                NumberAnimation {
                    duration: 300
                    easing.type: Easing.OutExpo
                }
            }
        }
    }


    Text {
        id: notificationText
        width: parent.width-170
        height: 84
        anchors.left: parent.left
        anchors.leftMargin: 108
        anchors.top: parent.top
        anchors.topMargin: 10

        color: colorText
        elide: Text.ElideRight
        wrapMode: Text.WordWrap
        verticalAlignment: Text.AlignVCenter
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Text {
        id: timestampText
        text: timestamp.toDateString() + "  " +  timestamp.toTimeString()
        width: parent.width-170
        anchors.left: parent.left
        anchors.leftMargin: 108
        anchors.top: notificationText.bottom
        anchors.topMargin: 10

        color: colorText
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 20
        lineHeight: 1
    }

    Item {
        id: buttons
        width: parent.width
        height: 60
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        anchors.horizontalCenter: parent.horizontalCenter

        RowLayout {
            spacing: 40
            anchors.horizontalCenter: parent.horizontalCenter
            Layout.alignment: Qt.AlignHCenter

            BasicUI.CustomButton {
                buttonText: qsTr("Dismiss") + translateHandler.emptyString
                mouseArea.onClicked: {
                    notification.opacity = 0;
                    if (notificationsDrawer.position > 0) {
                        notifications.remove(idN);
                    } else {
                        notification.destroy(400);
                        notifications.remove(idN);
                    }
                }
            }

            BasicUI.CustomButton {
                buttonText: qsTr(actionlabel) + translateHandler.emptyString
                visible: action != "" ? true : false
                mouseArea.onClicked:  {
                    notification.opacity = 0;
                    if (notificationsDrawer.position > 0) {
                        notifications.remove(idN);
                    } else {
                        notification.destroy(400);
                        notifications.remove(idN);
                    }
                    if (notification.actionlabel == "Reconnect") {
//                        integrations.getByType(notification.action).connect();
                    }
                }
            }
        }

    }

    Image {
        id: icon
        asynchronous: true
        width: 60
        height: 60
        fillMode: Image.PreserveAspectFit
        source: type ? "qrc:/images/notification/icon-notification-error.png" : "qrc:/images/notification/icon-notification-normal.png"
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.top: parent.top
        anchors.topMargin: 20

        ColorOverlay {
            visible: !darkMode
            anchors.fill: parent
            source: parent
            color: colorText
            antialiasing: true
        }
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // TIMER FOR AUTOMATIC DISMISS
    //////////////////////////////////////////////////////////////////////////////////////////////////

    Timer {
        id: notificationRemoverTimer
        repeat: false
        running: notification.state == "visible" ? true : false
        interval: 5000

        onTriggered: {
            notification.state = "hidden";
            notification.destroy(400);
        }
    }
}
