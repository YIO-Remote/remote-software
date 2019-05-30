import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: notification
    width: parent.width-20
    height: 104 // 244
    x: 10
    radius: 52
    color: type == "normal" ? colorHighlight : colorRed

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
    // SIGNAL
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // signal is emitted when clicked, so the notification is not removed automatically

    signal removeNotification(int idN)
    signal dismissNotification(int idN)


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // MOUSEAREA
    //////////////////////////////////////////////////////////////////////////////////////////////////

    property bool dragged: false

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
        if (x > 40 && x <= 120) {
            notification.x = 100;
            notificationRemoverTimer.stop();
        } else if (x > 240 && !dragged) {
            dragged = true;
            notificationRemoverTimer.stop();
            notification.opacity = 0;
            if (state == "permanent") {
                var tmp = notifications;
                tmp.splice(idN, 1);
                notifications = tmp;
            } else {
                removeNotification(idN);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // PROPERTIES FOR THE NOTIFICATION
    //////////////////////////////////////////////////////////////////////////////////////////////////

    property alias text: notificationText.text
    property string type
    property var action
    property string actionlabel
    property var timestamp
    property int idN

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    //////////////////////////////////////////////////////////////////////////////////////////////////

    state: "hidden"

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
            PropertyAnimation { target: notification; properties: "opacity, y"; easing.type: Easing.InExpo; duration: 300 }
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
            notification.x = 400;

            var tmp = notifications;
            tmp.splice(idN, 1);
            notifications = tmp;
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
                        var tmp = notifications;
                        tmp.splice(idN, 1);
                        notifications = tmp;
                    } else {
                        removeNotification(idN);
                    }
                }
            }

            BasicUI.CustomButton {
                buttonText: qsTr(actionlabel) + translateHandler.emptyString
                visible: action !== "" ? true : false
                mouseArea.onClicked:  {
                    notification.opacity = 0;
                    if (notificationsDrawer.position > 0) {
                        var tmp = notifications;
                        tmp.splice(idN, 1);
                        notifications = tmp;
                    } else {
                        removeNotification(idN);
                    }
                    notification.action();
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
        source: type == "normal" ? "qrc:/images/notification/icon-notification-normal.png" : "qrc:/images/notification/icon-notification-error.png"
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
            dismissNotification(idN);
        }
    }
}
