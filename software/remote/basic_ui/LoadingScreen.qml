import QtQuick 2.12

Rectangle {
    id: loadingScreenComp
    anchors.fill: parent
    color: "#00000000"

    state: "start"

    onStateChanged: {
        if (state == "loaded") {
            endAnim.start();
        }
    }

    SequentialAnimation {
        id: startAnim
        running: true

        PauseAnimation {duration: 1000}
        ParallelAnimation {
            PropertyAnimation { target: yio_Y; properties: "x"; to: -150; easing.type: Easing.OutExpo; duration: 600 }
            PropertyAnimation { target: yio_Y; properties: "opacity"; to: 0; easing.type: Easing.OutExpo; duration: 600 }
            SequentialAnimation {
                PauseAnimation {duration: 100}
                ParallelAnimation {
                    PropertyAnimation { target: yio_I; properties: "x"; to: -50; easing.type: Easing.OutExpo; duration: 600 }
                    PropertyAnimation { target: yio_I; properties: "opacity"; to: 0; easing.type: Easing.OutExpo; duration: 600 }
                }
            }
            SequentialAnimation {
                PauseAnimation {duration: 100}
                ParallelAnimation {
                    PropertyAnimation { target: yio_O; properties: "x";to: 192; easing.type: Easing.OutExpo; duration: 600 }
                    PropertyAnimation { target: loadingIconAnim; properties: "running";to: true; duration: 0 }
                }
            }
        }
        PauseAnimation {duration: 500}
        PropertyAction { target: loadingScreenComp; property: "startAnimFinished"; value: true }
    }

    SequentialAnimation {
        id: endAnim
        running: false

        PauseAnimation {duration: 1000}
        PropertyAnimation { target: loadingIconAnim; properties: "running";to: false; duration: 0 }
        PropertyAnimation { target: yio_O; properties: "opacity"; to: 0; easing.type: Easing.OutExpo; duration: 400 }
//        PauseAnimation {duration: 500}
        ParallelAnimation {
            PropertyAnimation { target: left; properties: "width"; to: 0; easing.type: Easing.Linear; duration: 800 }
            PropertyAnimation { target: right; properties: "width"; to: 0; easing.type: Easing.Linear; duration: 800 }
        }
        PropertyAction { target: loadingScreenComp; property: "endAnimFinished"; value: true }
    }

    property bool startAnimFinished: false;

    onStartAnimFinishedChanged: {
        if (startAnimFinished) {
            loader_main.active = true;
        }
    }

    property bool endAnimFinished: false;

    onEndAnimFinishedChanged: {
        if (endAnimFinished) {
            loadingScreen.source = "";
            loadingScreen.active = false;
        }
    }

    Rectangle {
        id: left
        width: 240
        height: 800
        color: "#000000"

        anchors {
            left: parent.left
        }
    }

    Rectangle {
        id: right
        width: 240
        height: 800
        color: "#000000"

        anchors {
            right: parent.right
        }
    }


    Image {
        asynchronous: true
        id: yio_Y
        width: 101
        height: 90
        x: 112
        y: 354
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/loading/Y.png"
    }

    Image {
        asynchronous: true
        id: yio_I
        width: 5
        height: 90
        x: 237
        y: 354
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/loading/I.png"
    }

    Image {
        asynchronous: true
        id: yio_O
        width: 96
        height: 96
        x: 272
        y: 351
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/loading/O.png"

        RotationAnimator on rotation {
            id: loadingIconAnim
            running: false
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 2000
        }
    }

    MouseArea {
        anchors.fill: parent
    }
}


