import QtQuick 2.9
import QtQuick.Controls 2.2

Rectangle {

    property alias mouseArea: mouseArea
    property alias buttonText: buttonText.text

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
            color: colorHighlight
            border.color: colorBackgroundTransparent
        }
    }

    transitions: [
      Transition {
          from: ""; to: "pressed"; reversible: true
          PropertyAnimation { target: button
                              properties: "color"; duration: 100 }
      }]

    Text {
        id: buttonText
        color: colorText
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 2
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.family: "Neuzeit Grotesk"
        font.weight: Font.Normal
        font.pixelSize: 28
        lineHeight: 0.8
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
    }
}
