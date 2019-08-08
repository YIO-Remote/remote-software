import QtQuick 2.11
import QtQuick.Controls 2.5

Rectangle {

    property alias mouseArea: mouseArea
    property alias buttonText: buttonText.text
    property alias buttonTextColor: buttonText.color

    id: button
    width: buttonText.implicitWidth+60
    height: 60
    color: colorBackgroundTransparent
    radius: 40
    border.color: colorLine

    states: State {
        name: "pressed"
        when: mouseArea.pressed == true
        PropertyChanges {
            target: button
            color: colorLine
            border.color: colorBackgroundTransparent
        }
    }

    transitions: [
      Transition {
          from: ""; to: "pressed"; reversible: true
          PropertyAnimation { target: button
                              properties: "color"; duration: 300 }
      }]

    Text {
        id: buttonText
        color: colorText
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font { family: "Open Sans Regular"; pixelSize: 27 }
        lineHeight: 0.8
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
    }
}
