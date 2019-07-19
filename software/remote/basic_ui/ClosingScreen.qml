import QtQuick 2.12
import Launcher 1.0

Rectangle {
    id: closingScreen
    anchors.fill: parent
    color: "#00000000"

    Launcher {
        id: launcher
    }

    SequentialAnimation {
        id: startAnim
        running: true

        PauseAnimation {duration: 1000}
        ParallelAnimation {
            ParallelAnimation {
                PropertyAnimation { target: left; properties: "width"; to: 240; easing.type: Easing.Linear; duration: 1000 }
                PropertyAnimation { target: right; properties: "width"; to: 240; easing.type: Easing.Linear; duration: 1000 }
            }
        }
        PropertyAction { target: closingScreen; property: "startAnimFinished"; value: true }
    }

    property bool startAnimFinished: false

    onStartAnimFinishedChanged: {
        if (startAnimFinished) {
            console.debug("NOW SHUTDOWN");
            launcher.launch("fbv -d 1 /bye.png")
            buttonHandler.interruptHandler.shutdown();
            launcher.launch("halt");
        }
    }

    Rectangle {
        id: left
        width: 0
        height: 800
        color: "#000000"

        anchors {
            left: parent.left
        }
    }

    Rectangle {
        id: right
        width: 0
        height: 800
        color: "#000000"

        anchors {
            right: parent.right
        }
    }

    MouseArea {
        anchors.fill: parent
    }
}


