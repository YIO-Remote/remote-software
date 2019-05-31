import QtQuick 2.11

Rectangle {
    id: connectionLoader
    anchors.fill: parent
    color: "#00000000"

    state: "connecting"

    property int iconShowDelay: 300

    states: [
        State {
            name: "connecting"
            PropertyChanges {target: connectionLoader; opacity: 1; visible: true}
            PropertyChanges {target: loadingIcon; opacity: 1}
            PropertyChanges {target: loadingIconOK; opacity: 0}
            PropertyChanges {target: loadingIconX; opacity: 0}
            PropertyChanges {target: loadingIconAnim; running: true}
            PropertyChanges {target: buttonTryAgain; opacity: 0}
            //: loading screen text while it loads
            PropertyChanges {target: text_small; text: qsTr("Wait for it ...") + translateHandler.emptyString}
        },
        State {
            name: "connected"
            PropertyChanges {target: connectionLoader; opacity: 0; visible: false}
            PropertyChanges {target: loadingIcon; opacity: 0}
            PropertyChanges {target: loadingIconOK; opacity: 1}
            PropertyChanges {target: loadingIconX; opacity: 0}
            PropertyChanges {target: buttonTryAgain; opacity: 0}
            PropertyChanges {target: loadingIconAnim; running: false}
            //: loading screen text when it is done loading
            PropertyChanges {target: text_small; text: qsTr("Done") + translateHandler.emptyString}
        },
        State {
            name: "failed"
            PropertyChanges {target: loadingIcon; opacity: 0}
            PropertyChanges {target: loadingIconOK; opacity: 0}
            PropertyChanges {target: loadingIconX; opacity: 1}
            PropertyChanges {target: loadingIconAnim; running: false}
            PropertyChanges {target: buttonTryAgain; opacity: 1}
            //: loading screen text when something went wrong.
            PropertyChanges {target: text_small; text: qsTr("Something went wrong. Please restart the remote") + translateHandler.emptyString}
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
                PropertyAnimation { target: text_small; properties: "text"; duration: 0 }
                PropertyAnimation { target: connectionLoader; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
            }},
        Transition {
            to: "connected"
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: loadingIcon; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                    PropertyAnimation { target: loadingIconAnim; properties: "running"; duration: 0 }
                    PropertyAnimation { target: loadingIconOK; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
                    PropertyAnimation { target: text_small; properties: "text"; duration: 0 }
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
                    PropertyAnimation { target: text_small; properties: "text"; duration: 0 }
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
        y: 230
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
        y: 230
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
        y: 230
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/loading/icon-loading-x.png"

        Behavior on y {
            NumberAnimation { duration: 400 }
        }
    }

    CustomButton {
        id: buttonTryAgain
        opacity: 0
        //: loading screen button if it failes to load
        buttonText: qsTr("Restart") + translateHandler.emptyString
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60

        mouseArea.onClicked: {
            settingsLauncher.launch("fbv -d 1 /bye.png")
            console.debug("now reboot")
            settingsLauncher.launch("reboot");
        }
    }

    Text {
        id: text_hello
        color: colorText
        //: greating at the loadin screen
        text: qsTr("Hello") + translateHandler.emptyString
        opacity: 1
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
        id: text_small
        color: colorText
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        width: parent.width-80
        opacity: 0.4
        anchors.top: text_hello.bottom
        anchors.topMargin: 0
        anchors.horizontalCenter: text_hello.horizontalCenter
        verticalAlignment: Text.AlignTop
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


