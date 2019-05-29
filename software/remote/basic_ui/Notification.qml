import QtQuick 2.0

Rectangle {
    id: notification
    width: parent.width-20
    height: 104 // 244
    anchors.horizontalCenter: parent.horizontalCenter
    radius: 52
    color: type == "normal" ? colorHighlight : colorRed
    clip: true

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // SIGNAL
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // signal is emitted when clicked, so the notification is not removed automatically

    signal removeNotification()


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // MOUSEAREA
    //////////////////////////////////////////////////////////////////////////////////////////////////

    MouseArea {
        anchors.fill: parent

        onClicked: {
            notificationRemoverTimer.stop();
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // PROPERTIES FOR THE NOTIFICATION
    //////////////////////////////////////////////////////////////////////////////////////////////////

    property alias text: notificationText.text
    property string type
    property var action


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    //////////////////////////////////////////////////////////////////////////////////////////////////

    Component.onCompleted: {
        notification.state = "visible"
    }

    states: [
        State {
            name: "visible"
            PropertyChanges { target: notification; opacity: 1; y: 20}
        },
        State {
            name: "hidden"
            PropertyChanges { target: notification; opacity: 0; y: -200}
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

    Text {
        id: notificationText
        width: parent.width-170
        anchors.left: parent.left
        anchors.leftMargin: 108
        anchors.top: parent.top
        anchors.topMargin: 20

        color: colorText
        verticalAlignment: Text.AlignVCenter
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////
    // TIMER FOR AUTOMATIC DISMISS
    //////////////////////////////////////////////////////////////////////////////////////////////////

    Timer {
        id: notificationRemoverTimer
        repeat: false
        running: true
        interval: 8000

        onTriggered: {
            notification.state = "hidden";
            removeNotification();
        }
    }
}
