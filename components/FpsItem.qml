import QtQuick 2.11
import QtQuick.Window 2.11

Rectangle {
    id: root
    property int frameCounter: 0
    property int frameCounterAvg: 0
    property int counter: 0
    property int fps: 0
    property int fpsAvg: 0

    color: "black"
    width:  childrenRect.width;
    height: childrenRect.height;

    Image {
        id: spinnerImage
        anchors.verticalCenter: parent.verticalCenter
        x: 4;
        width: 36;
        height: width
        source: "qrc:/images/spinner.png"
        NumberAnimation on rotation {
            from:0
            to: 360
            duration: 800
            loops: Animation.Infinite
        }
        onRotationChanged: frameCounter++;
    }

    Text {
        anchors.left: spinnerImage.right
        anchors.leftMargin: 8;
        anchors.verticalCenter: spinnerImage.verticalCenter
        color: "#c0c0c0"
        font.pixelSize: 18;
        text: "Ø " + root.fpsAvg + " | " + root.fps + " fps"
    }

    Timer {
        interval: 2000
        repeat: true
        running: true
        onTriggered: {
            frameCounterAvg += frameCounter;
            root.fps = frameCounter/2;
            counter++;
            frameCounter = 0;
            if (counter >= 3) {
                root.fpsAvg = frameCounterAvg/(2*counter)
                frameCounterAvg = 0;
                counter = 0;
            }
        }
    }
}
