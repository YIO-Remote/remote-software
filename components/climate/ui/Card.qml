import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0

import Entity.Climate 1.0

import "qrc:/basic_ui" as BasicUI

Item {
    id: climateCard

    property double targetTemperature: obj.targetTemperature

    Component.onCompleted: {
        var tempBase = temperatureDial.from;
        for (var i=temperatureDial.from; i<(temperatureDial.to*2)-3; i++) {
            var item = {};
            item["temp"] = tempBase;
            dialListModel.append(item);
            tempBase = tempBase + 0.5;
        }
        dialListView.currentIndex = findNumber(targetTemperature);
    }

    function findNumber(temp) {
        for (var i=0; i<dialListModel.count; i++) {
            if (dialListModel.get(i).temp == temp) {
                return i;
            }
        }
    }

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
            PropertyChanges {target: modeButton; opacity: 0}
            PropertyChanges {target: dialListView; opacity: 1}
        },
        State {
            name: "notmoving"
            PropertyChanges {target: temperatureText; opacity: 1}
            PropertyChanges {target: titleText; opacity: 1}
            PropertyChanges {target: areaText; opacity: 1}
            PropertyChanges {target: turnOnButton; opacity: 1}
            PropertyChanges {target: modeButton; opacity: 1}
            PropertyChanges {target: dialListView; opacity: 0}
        }
    ]

    transitions: [
        Transition {
            to: "moving"
            PropertyAnimation { target: temperatureText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: titleText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: areaText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: turnOnButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: modeButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: dialListView; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }

        },
        Transition {
            to: "notmoving"
            PropertyAnimation { target: temperatureText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: titleText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: areaText; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: turnOnButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: modeButton; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            PropertyAnimation { target: dialListView; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
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
            id: dialBg
            x: temperatureDial.width / 2 - width / 2
            y: temperatureDial.height / 2 - height / 2
            width: Math.max(64, Math.min(temperatureDial.width, temperatureDial.height))
            height: width
            color: {
                if ((obj.state === Climate.HEAT && targetTemperature > obj.temperature) || (targetTemperature > obj.temperature)) {
                    return colorOrange
                } else if ((obj.state === Climate.COOL && targetTemperature < obj.temperature) || targetTemperature < obj.temperature) {
                    return colorBlue
                } else {
                    return colorHighlight2
                }
            }
            radius: width / 2

            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: Item {
                    width: dialBg.width
                    height: dialBg.height
                    Rectangle {
                        anchors.fill: parent
                        radius: dialBg.radius
                    }
                }
            }

            ListModel {
                id: dialListModel
            }

            ListView {
                id: dialListView
                width: parent.width
                height: parent.height
                spacing: 10
                interactive: false
                highlightMoveDuration: 200
                highlightRangeMode: ListView.StrictlyEnforceRange
                preferredHighlightBegin: height / 2 - 80
                preferredHighlightEnd: height / 2 + 80

                model: dialListModel

                delegate: Component {
                    Item {
                        height: 160
                        anchors.horizontalCenter: parent.horizontalCenter
                        opacity: ListView.isCurrentItem ? 1 : 0.5
                        scale: ListView.isCurrentItem ? 1 : 0.5

                        Behavior on opacity {
                            NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
                        }

                        Behavior on scale {
                            NumberAnimation { duration: 100; easing.type: Easing.OutExpo }
                        }

                        Text {
                            id: temperatureTextDial
                            color: colorText
                            text: {
                                var s = temp.toString().split(".")
                                return s[0]
                            }
                            horizontalAlignment: Text.AlignLeft
                            anchors.centerIn: parent
                            font {family: "Open Sans Light"; pixelSize: 140 }
                        }

                        Text {
                            color: colorText
                            text: {
                                var s = temp.toString().split(".")
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
                }
            }
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
            var i = findNumber(targetTemperature);
            if (i)
                dialListView.currentIndex = i;
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
        text: {
            var s = targetTemperature.toString().split(".")
            return s[0]
        }
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

    BasicUI.CustomButton {
        id: modeButton
        anchors { left:turnOnButton.right; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
        color: colorText
        buttonTextColor: colorBackground
        buttonText: qsTr("Mode") + translateHandler.emptyString
        visible: obj.isSupported(Climate.F_HVAC_MODES)

        mouseArea.onClicked: {
            haptic.playEffect("click");
            var list = [];

            if (obj.isSupported(Climate.OFF))
                list.push(qsTr("Off")+translateHandler.emptyString);
            if (obj.isSupported(Climate.F_HEAT))
                list.push(qsTr("Heat")+translateHandler.emptyString);
            if (obj.isSupported(Climate.F_COOL))
                list.push(qsTr("Cool")+translateHandler.emptyString);

            contextMenuLoader.setSource("qrc:/components/climate/ui/ContextMenu.qml", { "width": climateCard.width, "climateObj": obj, "list": list })
        }
    }

    Loader {
        id: contextMenuLoader
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        visible: modeButton.visible

        onStatusChanged: {
            if (contextMenuLoader.status == Loader.Ready)
                contextMenuLoader.item.state = "open"
        }
    }

}
