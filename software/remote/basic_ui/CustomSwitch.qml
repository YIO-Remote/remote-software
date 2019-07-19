import QtQuick 2.12
import QtQuick.Controls 2.4

Item {
    width: 66
    height: 36

    property alias checked: button.checked
    property alias mouseArea: mouseArea
    property alias _opacity: button.opacity

    Switch {
        id: button
        implicitHeight: 36
        implicitWidth: 66
        enabled: mouseArea.enabled

        indicator: Rectangle {
            x: 4 + (button.visualPosition * (button.width - width - 8))
            y: (button.height - height) / 2
            width: 26
            height: 26
            radius: cornerRadius
            color: button.checked ? colorText : colorLight

            Behavior on x {
                enabled: !button.pressed
                SmoothedAnimation { velocity: 150 }
            }
        }

        background: Rectangle {
            radius: cornerRadius+2
            color: button.checked ? colorHighlight1 : colorDark
            border.width: 2
            border.color: button.checked ? colorHighlight1 : colorLight
        }
    }

    MouseArea {
        id: mouseArea
        width: button.width + 60
        height: button.height + 60

        onClicked: {
            haptic.playEffect("click");
            button.toggle();
        }
    }
}
