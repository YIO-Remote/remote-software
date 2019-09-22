import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

Item {
    id: volume
    width:  parent.width
    height: 160

    property double volumePosition

    state: "hidden"

    states: [
        State { name: "hidden"; PropertyChanges {target: bg; y: 400; height: 80; scale: 0.2}},
        State { name: "visible"; PropertyChanges {target: bg; y: 40; height: 80; scale: 1}}
    ]
    transitions: [
        Transition {to: "hidden";
            SequentialAnimation {
                PropertyAnimation { target: bg; properties: "y, scale"; easing.type: Easing.InExpo; duration: 300 }
                PropertyAnimation { target: bg; properties: "height"; duration: 1 }
            }
        },
        Transition {to: "visible"; PropertyAnimation { target: bg; properties: "y, scale"; easing.type: Easing.OutExpo; duration: 300 }}
    ]

    onVolumePositionChanged : {
            hideTimer.restart();
    }

    Timer {
        id: shrinkTimer
        running: state == "visible" ? true : false
        interval: 1000
        repeat: false

        onTriggered: {
            bg.height = 20;
            bg.y += 60;
            hideTimer.start();
        }
    }

    Timer {
        id: hideTimer
        running: false
        interval: 2000
        repeat: false

        onTriggered: {
            volume.state = "hidden";
        }
    }

    Rectangle {
        id: bg
        width: 300
        height: 80
        color: colorLight
        radius: height/2
        anchors.horizontalCenter: parent.horizontalCenter

        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource:
                Rectangle {
                    width: bg.width
                    height: bg.height
                    radius: bg.radius
            }
        }

        Rectangle {
            id: volumeBar
            height: parent.height
            width: bg.width * volumePosition
            color: colorLine
            anchors.left: parent.left
        }

        Image {
            anchors.centerIn: parent
            source: "qrc:/images/volume/icon-speaker.png"
            opacity: bg.height == 80 ? 1 : 0

            Behavior on opacity {
                NumberAnimation { duration: 200; easing.type: Easing.OutExpo }
            }
        }


        Behavior on height {
            NumberAnimation { duration: 200; easing.type: Easing.OutExpo }
        }

        Behavior on y {
            NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
        }
    }

//    DropShadow {
//        anchors.fill: bg
//        radius: 5
//        samples: 9
//        color: "#80000000"
//        source: bg
//    }
}
