import QtQuick 2.9

Rectangle {

    property alias mouseArea: mouseArea
    property alias buttonText: buttonText.text
    property bool buttonState: false

    id: button
    width: buttonText.implicitWidth+60
    height: 60
    color: buttonState ? colorHighlight : colorBackgroundTransparent
    radius: 40
    border.color: buttonState ? colorHighlight : colorLine

    states: State {
        name: "pressed"
        when: mouseArea.pressed === true
        PropertyChanges {
            target: button
            color: buttonState ? colorBackgroundTransparent : colorHighlight
            border.color: buttonState ? colorLine : colorBackgroundTransparent
        }
    }

    transitions: [
      Transition {
          from: ""; to: "pressed"; reversible: true
          PropertyAnimation { target: button
                              properties: "color"; duration: 400 }
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
