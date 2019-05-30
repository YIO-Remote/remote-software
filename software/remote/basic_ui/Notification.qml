import QtQuick 2.0
import QtGraphicalEffects 1.0

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

    signal removeNotification()
    signal dismissNotification()


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // MOUSEAREA
    //////////////////////////////////////////////////////////////////////////////////////////////////

    property bool dragged: false

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        drag.target: notification
        drag.axis: Drag.XAxis
        drag.minimumX: 10
        drag.maximumX: 400

        onClicked: {
            notificationRemoverTimer.stop();
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
                removeNotification();
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // PROPERTIES FOR THE NOTIFICATION
    //////////////////////////////////////////////////////////////////////////////////////////////////

    property alias text: notificationText.text
    property string type
    property var action
    property var timestamp
    property int idN


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    //////////////////////////////////////////////////////////////////////////////////////////////////

    state: "visible"

    states: [
        State {
            name: "visible"
            PropertyChanges { target: notification; opacity: 1; y: 20}
        },
        State {
            name: "hidden"
            PropertyChanges { target: notification; opacity: 0; y: -200}
        },
        State {
            name: "permanent"
        }
    ]

    transitions: [
        Transition {
            to: "visible"
            PropertyAnimation { target: notification; properties: "opacity, y"; easing.type: Easing.InExpo; duration: 300 }
        },
        Transition {
            to: "hidden"
            PropertyAnimation { target: notification; properties: "opacity, y"; easing.type: Easing.InExpo; duration: 300 }
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
        height: parent.height-20
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

    Image {
        id: icon
        asynchronous: true
        width: 60
        height: 60
        fillMode: Image.PreserveAspectFit
        source: type == "normal" ? "qrc:/images/notification/icon-notification-normal.png" : "qrc:/images/notification/icon-notification-error.png"
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.verticalCenter: parent.verticalCenter

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
            dismissNotification();
        }
    }
}
