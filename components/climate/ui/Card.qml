import QtQuick 2.12
import QtQuick.Controls 2.12

import Entity.Climate 1.0

import "qrc:/basic_ui" as BasicUI

Item {
    id: climateCard

    property double targetTemperature: obj.targetTemperature

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    state: "notmoving"

    states: [
        State {
            name: "moving"
            PropertyChanges {target: temperatureText; opacity: 0}
            PropertyChanges {target: titleText; opacity: 0}
            PropertyChanges {target: areaText; opacity: 0}
            PropertyChanges {target: turnOnButton; opacity: 0}
            PropertyChanges {target: temperatureTextDial; opacity: 1}
        },
        State {
            name: "notmoving"
            PropertyChanges {target: temperatureText; opacity: 1}
            PropertyChanges {target: titleText; opacity: 1}
            PropertyChanges {target: areaText; opacity: 1}
            PropertyChanges {target: turnOnButton; opacity: 1}
            PropertyChanges {target: temperatureTextDial; opacity: 0}
        }
    ]

    transitions: [
        Transition {
            to: "moving"
            PropertyAnimation { target: temperatureText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: titleText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: areaText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: turnOnButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: temperatureTextDial; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }

        },
        Transition {
            to: "notmoving"
            PropertyAnimation { target: temperatureText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: titleText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: areaText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: turnOnButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: temperatureTextDial; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
        }
    ]

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Rectangle {
        anchors.fill: parent
        color: colorDark
    }

    Dial {
        id: temperatureDial
        width: parent.width-100
        height: width
        anchors { top: parent.top; topMargin: 140; horizontalCenter: parent.horizontalCenter }
        from: obj.temperatureMin
        to: obj.temperatureMax
        stepSize: 0.5
        snapMode: Dial.SnapAlways
        value: targetTemperature

        background: Rectangle {
            x: temperatureDial.width / 2 - width / 2
            y: temperatureDial.height / 2 - height / 2
            width: Math.max(64, Math.min(temperatureDial.width, temperatureDial.height))
            height: width
            color: {
                if (obj.state === Climate.HEAT && obj.targetTemperate > obj.temperature) {
                    return colorOrange
                } else if (obj.state === Climate.COOL && obj.targetTemperature < obj.temperature) {
                    return colorBlue
                } else {
                    return colorHighlight2
                }
            }

            radius: width / 2
        }

        handle: Rectangle {
            id: handleItem
            x: temperatureDial.background.x + temperatureDial.background.width / 2 - width / 2
            y: temperatureDial.background.y + temperatureDial.background.height / 2 - height / 2
            width: 60
            height: 60
            color: colorBackground
            opacity: 0.5
            radius: 30
            antialiasing: true
            transform: [
                Translate {
                    y: -Math.min(temperatureDial.background.width, temperatureDial.background.height) * 0.45 + handleItem.height / 2
                },
                Rotation {
                    angle: temperatureDial.angle
                    origin.x: handleItem.width / 2
                    origin.y: handleItem.height / 2
                }
            ]
        }

        onValueChanged: {
            targetTemperature = value;
        }

        onPressedChanged: {
            if (pressed) {
                climateCard.state = "moving"
            }
            else {
                climateCard.state = "notmoving"
                obj.setTargetTemperature(targetTemperature);
            }
        }

        Text {
            id: temperatureTextDial
            color: colorText
            text: targetTemperature.toFixed(0)
            horizontalAlignment: Text.AlignLeft
            anchors.centerIn: parent
            font {family: "Open Sans Light"; pixelSize: 140 }
        }

        Text {
            color: colorText
            opacity: temperatureTextDial.opacity
            text: {
                var s = targetTemperature.toString().split(".")
                if (s[1]) {
                    return s[1]
                } else {
                    return ""
                }
            }
            anchors { top: temperatureTextDial.top; topMargin: 16; left: temperatureTextDial.right; leftMargin: 0 }
            font {family: "Open Sans Regular"; pixelSize: 80 }
        }
    }

    Text {
        id: icon
        color: colorText
        text: "\uE913"
        renderType: Text.NativeRendering
        width: 85
        height: 85
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font {family: "icons"; pixelSize: 100 }
        anchors {top: parent.top; topMargin: 20; left: parent.left; leftMargin: 20}
    }

    Text {
        id: temperatureText
        color: colorText
        text: targetTemperature.toFixed(0)
        horizontalAlignment: Text.AlignLeft
        anchors { top: icon.bottom; topMargin: 0; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Light"; pixelSize: 140 }
    }

    Text {
        color: colorText
        opacity: temperatureText.opacity
        text: {
            var s = targetTemperature.toString().split(".")
            if (s[1]) {
                return s[1] + obj.temperatureUnit
            } else {
                return obj.temperatureUnit
            }
        }
        anchors { top: temperatureText.top; topMargin: 16; left: temperatureText.right; leftMargin: 0 }
        font {family: "Open Sans Regular"; pixelSize: 80 }
    }

    Text {
        id: titleText
        color: colorText
        text: obj.friendly_name
        wrapMode: Text.WordWrap
        width: parent.width-60
        anchors { top: temperatureText.bottom; topMargin: 0; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Regular"; pixelSize: 60 }
        lineHeight: 0.9
    }

    Text {
        id: areaText
        color: colorText
        opacity: 0.5
        text: obj.area
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
        width: parent.width-60
        anchors { top: titleText.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Regular"; pixelSize: 24 }
    }

    BasicUI.CustomButton {
        id: turnOnButton
        anchors { left:parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
        color: colorText
        buttonTextColor: colorBackground
        buttonText: obj.state ? qsTr("Turn off") + translateHandler.emptyString : qsTr("Turn on") + translateHandler.emptyString

        mouseArea.onClicked: {
            haptic.playEffect("click");
            obj.toggle();
        }
    }


}
