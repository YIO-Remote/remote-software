import QtQuick 2.11

Rectangle {
    id: connectionLoader
    anchors.fill: parent
    color: "#00000000"

    state: connectionState

    property int iconShowDelay: firstRun ? 300 : 0

    states: [
        State {
            name: "connecting"
            PropertyChanges {target: connectionLoader; opacity: 1; visible: true}
            PropertyChanges {target: loadingIcon; opacity: 1}
            PropertyChanges {target: loadingIconOK; opacity: 0}
            PropertyChanges {target: loadingIconX; opacity: 0}
            PropertyChanges {target: loadingIconAnim; running: true}
            PropertyChanges {target: buttonTryAgain; opacity: 0}
            PropertyChanges {target: connectionLoaderText; text: qsTr("Reconnecting...") + translateHandler.emptyString}
        },
        State {
            name: "connected"
            PropertyChanges {target: connectionLoader; opacity: 0; visible: false}
            PropertyChanges {target: loadingIcon; opacity: 0}
            PropertyChanges {target: loadingIconOK; opacity: 1}
            PropertyChanges {target: loadingIconX; opacity: 0}
            PropertyChanges {target: buttonTryAgain; opacity: 0}
            PropertyChanges {target: loadingIconAnim; running: false}
            PropertyChanges {target: connectionLoaderText; text: qsTr("Connected") + translateHandler.emptyString}
        },
        State {
            name: "failed"
            PropertyChanges {target: loadingIcon; opacity: 0}
            PropertyChanges {target: loadingIconOK; opacity: 0}
            PropertyChanges {target: loadingIconX; opacity: 1}
            PropertyChanges {target: loadingIconAnim; running: false}
            PropertyChanges {target: buttonTryAgain; opacity: 1}
            PropertyChanges {target: connectionLoaderText; text: qsTr("Connection failed") + translateHandler.emptyString}
        }
    ]

    transitions: [
        Transition {
            to: "connecting"
            SequentialAnimation {
                PauseAnimation { duration: connectionLoader.iconShowDelay }
                PropertyAnimation { target: connectionLoader; properties: "visible"; duration: 0 }
                PropertyAnimation { target: loadingIcon; properties: "opacity"; duration: 0 }
                PropertyAnimation { target: loadingIconAnim; properties: "running"; duration: 0 }
                PropertyAnimation { target: loadingIconOK; properties: "opacity"; duration: 0 }
                PropertyAnimation { target: loadingIconX; properties: "opacity"; duration: 0 }
                PropertyAnimation { target: buttonTryAgain; properties: "opacity"; duration: 0 }
                PropertyAnimation { target: connectionLoaderText; properties: "text"; duration: 0 }
                PropertyAnimation { target: connectionLoader; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
            }},
        Transition {
            //                from: "connecting"
            to: "connected"
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: loadingIcon; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                    PropertyAnimation { target: loadingIconAnim; properties: "running"; duration: 0 }
                    PropertyAnimation { target: loadingIconOK; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
                    PropertyAnimation { target: connectionLoaderText; properties: "text"; duration: 0 }
                }
                PauseAnimation { duration: 800 }
                PropertyAnimation { target: connectionLoader; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
                PropertyAnimation { target: connectionLoader; properties: "visible"; duration: 0 }
            }},
        Transition {
            from: "connecting"
            to: "failed"
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: loadingIcon; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                    PropertyAnimation { target: loadingIconAnim; properties: "running"; duration: 0 }
                    PropertyAnimation { target: loadingIconX; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
                    PropertyAnimation { target: connectionLoaderText; properties: "text"; duration: 0 }
                }
                PauseAnimation { duration: 200 }
                PropertyAnimation { target: buttonTryAgain; properties: "opacity"; easing.type: Easing.InExpo; duration: 600 }
            }}
    ]

    Rectangle {
        anchors.fill: parent
        color: colorBackground
        opacity: 0.96
    }

    Image {
        asynchronous: true
        id: loadingIcon
        width: 160
        height: 160
        opacity: 0
        y: firstRun ? 230 : 300
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/loading/icon-loading.png"

        RotationAnimator on rotation {
            id: loadingIconAnim
            running: false
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 2000
        }

        Behavior on y {
            NumberAnimation { duration: 400 }
        }
    }

    Image {
        asynchronous: true
        id: loadingIconOK
        width: 160
        height: 160
        opacity: 0
        y: firstRun ? 230 : 300
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/loading/icon-loading-ok.png"

        Behavior on y {
            NumberAnimation { duration: 400 }
        }
    }

    Image {
        asynchronous: true
        id: loadingIconX
        width: 160
        height: 160
        opacity: 0
        y: firstRun ? 230 : 300
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/loading/icon-loading-x.png"

        Behavior on y {
            NumberAnimation { duration: 400 }
        }
    }

    Text {
        id: connectionLoaderText
        visible: firstRun ? false : true
        color: colorText
        text: qsTr("Reconnecting...") + translateHandler.emptyString
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: loadingIcon.bottom
        anchors.topMargin: 20
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Open Sans"
        font.weight: Font.Light
        font.pixelSize: 27
        lineHeight: 0.8
    }

    CustomButton {
        id: buttonTryAgain
        opacity: 0
        buttonText: qsTr("Try again") + translateHandler.emptyString
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60

        mouseArea.onClicked: {
            websocketReconnect.start()
            connectionLoader.state = "connecting"
        }
    }

    Text {
        id: text_hello
        color: colorText
        text: qsTr("Hello") + translateHandler.emptyString
        opacity: firstRun ? 1 : 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: loadingIcon.bottom
        anchors.topMargin: 30
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Open Sans"
        font.weight: Font.Light
        font.styleName: "Light"
        font.pixelSize: 72

        Behavior on opacity {
            NumberAnimation { duration: 400 }
        }
    }

    Text {
        id: text_waitforit
        color: colorText
        text: qsTr("Wait for it...") + translateHandler.emptyString
        opacity: firstRun ? 0.4 : 0
        anchors.top: text_hello.bottom
        anchors.topMargin: 0
        anchors.horizontalCenter: text_hello.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 0.8

        Behavior on opacity {
            NumberAnimation { duration: 400 }
        }
    }

    MouseArea {
        anchors.fill: parent
    }

}


