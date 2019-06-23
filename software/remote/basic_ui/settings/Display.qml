import QtQuick 2.11
import QtQuick.Controls 2.4

import "qrc:/scripts/helper.js" as JSHelper

Item {
    width: parent.width
    height: header.height + section.height + 20

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DISPLAY
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text {
        id: header
        color: colorText
        text: qsTr("Display") + translateHandler.emptyString
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: section
        width: parent.width
        height: childrenRect.height + 40
        radius: cornerRadius
        color: colorMedium

        anchors.top: header.bottom
        anchors.topMargin: 20

        Text {
            id: autobrightnessText
            color: colorText
            text: qsTr("Auto brightness") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Switch {
            id: autobrightnessButton
            implicitHeight: 36
            implicitWidth: 66
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: autobrightnessText.verticalCenter
            checked: standbyControl.display_autobrightness

            indicator: Rectangle {
                x: autobrightnessButton.visualPosition * (autobrightnessButton.width - width)
                y: (autobrightnessButton.height - height) / 2
                width: 36
                height: 36
                radius: cornerRadius
                color: autobrightnessButton.checked ? colorSwitchOn : colorSwitch

                Behavior on x {
                    enabled: !autobrightnessButton.pressed
                    SmoothedAnimation { velocity: 150 }
                }
            }

            background: Rectangle {
                radius: cornerRadius+2
                color: autobrightnessButton.checked ? colorHighlight : colorSwitchBackground
            }

            onClicked: {
                var tmp = config;
                tmp.settings.autobrightness = !tmp.settings.autobrightness
                config = tmp;
                JSHelper.saveConfig();
            }

        }

        Slider {
            id: brightnessSlider
            from: 0
            value: standbyControl.display_brightness
            to: 100
            stepSize: 1
            live: false

            width: parent.width-40
            anchors.top: section.top
            anchors.topMargin: 87
            anchors.horizontalCenter: parent.horizontalCenter

            property bool setbyUser: false

            background: Rectangle {
                x: parent.leftPadding
                y: brightnessSlider.topPadding + brightnessSlider.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 4
                width: brightnessSlider.availableWidth
                height: implicitHeight
                radius: 4
                color: colorBackground

                Rectangle {
                    width: brightnessSlider.visualPosition * parent.width
                    height: 4
                    radius: 2
                    color: colorBackgroundTransparent

                    Rectangle {
                        width: parent.height
                        height: parent.width
                        anchors.centerIn: parent
                        rotation: -90
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: colorMedium }
                            GradientStop { position: 1.0; color: colorHighlight }
                        }
                    }
                }
            }

            handle: Rectangle {
                x: brightnessSlider.leftPadding + brightnessSlider.visualPosition * (brightnessSlider.availableWidth - width)
                y: brightnessSlider.topPadding + brightnessSlider.availableHeight / 2 - height / 2
                implicitWidth: cornerRadius*2
                implicitHeight: cornerRadius*2
                radius: cornerRadius
                color: colorLine
            }

            onValueChanged: {
                if (setbyUser) {
                    standbyControl.setBrightness(brightnessSlider.value);
                    standbyControl.display_brightness_set = brightnessSlider.value
                    setbyUser = false;
                }
            }

            onMoved: {
                setbyUser = true;
            }
        }

        Rectangle {
            id: line1
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: brightnessSlider.bottom
            anchors.topMargin: 20
        }

        Text {
            id: darkmodeText
            color: colorText
            text: qsTr("Dark mode") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line1.bottom
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Switch {
            id: darkmodeButton
            implicitHeight: 36
            implicitWidth: 66
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: darkmodeText.verticalCenter
            checked: darkMode

            indicator: Rectangle {
                x: darkmodeButton.visualPosition * (darkmodeButton.width - width)
                y: (darkmodeButton.height - height) / 2
                width: 36
                height: 36
                radius: cornerRadius
                color: darkmodeButton.checked ? colorSwitchOn : colorSwitch

                Behavior on x {
                    enabled: !darkmodeButton.pressed
                    SmoothedAnimation { velocity: 150 }
                }
            }

            background: Rectangle {
                radius: cornerRadius+2
                color: darkmodeButton.checked ? colorHighlight : colorSwitchBackground
            }

            onClicked: {
//                darkMode = !darkMode
                var tmp = config;
                tmp.settings.darkmode = !tmp.settings.darkmode
                config = tmp;
                JSHelper.saveConfig();
            }
        }

        Rectangle {
            id: line2
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: darkmodeText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: cleaningText
            color: colorText
            text: qsTr("Cleaning mode") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line2.bottom
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: cleaningsmallText
            color: colorText
            opacity: 0.5
            text: qsTr("Touch the display for 10 seconds to turn cleaning mode off.") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            width: parent.width - 40 - cleaningmodeButton.width
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: cleaningText.bottom
            anchors.topMargin: 10
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Switch {
            id: cleaningmodeButton
            implicitHeight: 36
            implicitWidth: 66
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: cleaningText.verticalCenter
            checked: false

            indicator: Rectangle {
                x: cleaningmodeButton.visualPosition * (cleaningmodeButton.width - width)
                y: (cleaningmodeButton.height - height) / 2
                width: 36
                height: 36
                radius: cornerRadius
                color: cleaningmodeButton.checked ? colorSwitchOn : colorSwitch

                Behavior on x {
                    enabled: !cleaningmodeButton.pressed
                    SmoothedAnimation { velocity: 150 }
                }
            }

            background: Rectangle {
                radius: cornerRadius+2
                color: cleaningmodeButton.checked ? colorHighlight : colorSwitchBackground
            }

            onClicked: {
                // cleaning mode
                this.checked = true
            }
        }
    }
}
