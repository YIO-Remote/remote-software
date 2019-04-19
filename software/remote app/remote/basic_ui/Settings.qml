import QtQuick 2.11
import QtQuick.Controls 2.4

import Launcher 1.0

Item {
    width: parent.width
    height: displayHeader.height + displaySection.height + standbyHeader.height + standbySection.height + batteryHeader.height + batterySection.height + networkHeader.height + networkSection.height + systemHeader.height + systemSection.height + 280

    Launcher {
        id: settingsLauncher
    }

    Timer {
        running: mainNavigationSwipeview.currentIndex ==  2 + supported_entities.length ? true : false
        repeat: true
        interval: 2000

        onTriggered: {
            uptimeValue.text = settingsLauncher.launch("/usr/bin/remote/uptime.sh");
            temperatureValue.text = parseInt(settingsLauncher.launch("cat /sys/class/thermal/thermal_zone0/temp"))/1000 + "ºC";
            wifiSignalValue.text = settingsLauncher.launch("/usr/bin/remote/wifi_rssi.sh");
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // DISPLAY
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Text {
        id: displayHeader
        color: colorText
        text: translations[languange].settings.display
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: displaySection
        width: parent.width
        height: 375
        radius: cornerRadius
        color: colorMedium

        anchors.top: displayHeader.bottom
        anchors.topMargin: 20

        Text {
            id: autobrightnessText
            color: colorText
            text: translations[languange].settings.autobrightness
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
                if (standbyControl.display_autobrightness) {
                    standbyControl.display_autobrightness = false;
                } else {
                    standbyControl.display_autobrightness = true;
                }
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
            anchors.top: displaySection.top
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
                standbyControl.display_brightness = brightnessSlider.value
                if (setbyUser) {
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
            text: translations[languange].settings.darkmode
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
                if (darkMode) {
                    darkMode = false;
                } else {
                    darkMode = true;
                }
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
            text: translations[languange].settings.cleaningmode
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
            text: translations[languange].settings.cleaningtext
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // POWER SAVING
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text {
        id: standbyHeader
        color: colorText
        text: translations[languange].settings.powersaving
        anchors.top: displaySection.bottom
        anchors.topMargin: 40
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: standbySection
        width: parent.width
        height: 405
        radius: cornerRadius
        color: colorMedium

        anchors.top: standbyHeader.bottom
        anchors.topMargin: 20

        Text {
            id: wifioffText
            color: colorText
            text: standbyControl.wifiOffTime == 0 ? translations[languange].settings.wifiofftext3 : translations[languange].settings.wifiofftext1 + "\n" + standbyControl.wifiOffTime/60 + translations[languange].settings.wifiofftext2
            wrapMode: Text.WordWrap
            width: parent.width-40
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Slider {
            id: wifioffSlider
            from: 0
            value: standbyControl.wifiOffTime/60
            to: 60
            stepSize: 1
            live: true

            width: parent.width-40
            anchors.top: standbySection.top
            anchors.topMargin: 114
            anchors.horizontalCenter: parent.horizontalCenter

            background: Rectangle {
                x: parent.leftPadding
                y: wifioffSlider.topPadding + wifioffSlider.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 4
                width: wifioffSlider.availableWidth
                height: implicitHeight
                radius: 4
                color: colorBackground

                Rectangle {
                    width: wifioffSlider.visualPosition * parent.width
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
                x: wifioffSlider.leftPadding + wifioffSlider.visualPosition * (wifioffSlider.availableWidth - width)
                y: wifioffSlider.topPadding + wifioffSlider.availableHeight / 2 - height / 2
                implicitWidth: cornerRadius*2
                implicitHeight: cornerRadius*2
                radius: cornerRadius
                color: colorLine
            }

            onValueChanged: {
                standbyControl.wifiOffTime = wifioffSlider.value*60
            }
        }

        Text {
            color: colorText
            opacity: 0.5
            text: translations[languange].settings.never
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: wifioffSlider.bottom
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Text {
            color: colorText
            opacity: 0.5
            text: "60 " + translations[languange].settings.minutes
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.top: wifioffSlider.bottom
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Rectangle {
            id: line3
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: wifioffSlider.bottom
            anchors.topMargin: 40
        }

        Text {
            id: shutdownText
            color: colorText
            text: standbyControl.shutdownTime == 0 ? translations[languange].settings.shutdowntext3 : translations[languange].settings.shutdowntext1 + "\n" + standbyControl.shutdownTime/60/60 + translations[languange].settings.shutdowntext2
            wrapMode: Text.WordWrap
            width: parent.width-40
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line3.bottom
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Slider {
            id: shutdownSlider
            from: 0
            value: standbyControl.shutdownTime/60/60
            to: 8
            stepSize: 0.5
            live: true

            width: parent.width-40
            anchors.top: line3.top
            anchors.topMargin: 114
            anchors.horizontalCenter: parent.horizontalCenter

            background: Rectangle {
                x: parent.leftPadding
                y: shutdownSlider.topPadding + shutdownSlider.availableHeight / 2 - height / 2
                implicitWidth: 200
                implicitHeight: 4
                width: shutdownSlider.availableWidth
                height: implicitHeight
                radius: 4
                color: colorBackground

                Rectangle {
                    width: shutdownSlider.visualPosition * parent.width
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
                x: shutdownSlider.leftPadding + shutdownSlider.visualPosition * (shutdownSlider.availableWidth - width)
                y: shutdownSlider.topPadding + shutdownSlider.availableHeight / 2 - height / 2
                implicitWidth: cornerRadius*2
                implicitHeight: cornerRadius*2
                radius: cornerRadius
                color: colorLine
            }

            onValueChanged: {
                standbyControl.shutdownTime = shutdownSlider.value*60*60
            }
        }

        Text {
            color: colorText
            opacity: 0.5
            text: translations[languange].settings.never
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: shutdownSlider.bottom
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }

        Text {
            color: colorText
            opacity: 0.5
            text: "8 " + translations[languange].settings.hours
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.top: shutdownSlider.bottom
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BATTERY
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text {
        id: batteryHeader
        color: colorText
        text: translations[languange].settings.battery
        anchors.top: standbySection.bottom
        anchors.topMargin: 40
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: batterySection
        width: parent.width
        height: 235
        radius: cornerRadius
        color: colorMedium

        anchors.top: batteryHeader.bottom
        anchors.topMargin: 20

        Text {
            id: batterychargeText
            color: colorText
            text: translations[languange].settings.batterycharge
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            color: colorText
            text: battery_level + "%"
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: batterychargeText.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line4
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: batterychargeText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: batteryhealthText
            color: colorText
            text: translations[languange].settings.batteryhealth
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line4.bottom
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            color: colorText
            text: battery_health + "%"
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: batteryhealthText.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line5
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: batteryhealthText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: batteryvoltageText
            color: colorText
            text: translations[languange].settings.batteryvoltage
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line5.bottom
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            color: colorText
            text: battery_voltage + "V"
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: batteryvoltageText.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NETWORK
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Text {
        id: networkHeader
        color: colorText
        text: translations[languange].settings.network
        anchors.top: batterySection.bottom
        anchors.topMargin: 40
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: networkSection
        width: parent.width
        height: 155
        radius: cornerRadius
        color: colorMedium

        anchors.top: networkHeader.bottom
        anchors.topMargin: 20

        Text {
            id: wifiSignalText
            color: colorText
            text: translations[languange].settings.wifisignal
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: wifiSignalValue
            color: colorText
            text: "-59"
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: wifiSignalText.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line6
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: wifiSignalText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: ipaddressText
            color: colorText
            text: translations[languange].settings.ipaddress
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line6.bottom
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            color: colorText
            text: "192.168.1.1"
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: ipaddressText.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SYSTEM
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Text {
        id: systemHeader
        color: colorText
        text: translations[languange].settings.system
        anchors.top: networkSection.bottom
        anchors.topMargin: 40
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: systemSection
        width: parent.width
        height: 268
        radius: cornerRadius
        color: colorMedium

        anchors.top: systemHeader.bottom
        anchors.topMargin: 20

        Text {
            id: uptimeText
            color: colorText
            text: translations[languange].settings.uptime
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: uptimeValue
            color: colorText
            text: "0h"
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: uptimeText.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line7
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: uptimeText.bottom
            anchors.topMargin: 20
        }

        Text {
            id: temperatureText
            color: colorText
            text: translations[languange].settings.cputemperature
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: line7.bottom
            anchors.topMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: temperatureValue
            color: colorText
            text: "36ºC"
            horizontalAlignment: Text.AlignRight
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: temperatureText.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Rectangle {
            id: line8
            width: parent.width
            height: 2
            color: colorBackground
            anchors.top: temperatureText.bottom
            anchors.topMargin: 20
        }

        CustomButton {
            id: buttonReboot
            buttonText: translations[languange].settings.reboot
            anchors.top: line8.bottom
            anchors.topMargin: 30
            anchors.left: parent.left
            anchors.leftMargin: (parent.width - (buttonReboot.width + buttonShutdown.width + 60))/2

            mouseArea.onClicked: {
                settingsLauncher.launch("fbv -d 1 /bye.png")
                console.debug("now reboot")
                settingsLauncher.launch("reboot");
            }
        }

        CustomButton {
            id: buttonShutdown
            buttonText: translations[languange].settings.shutdown
            anchors.top: line8.bottom
            anchors.topMargin: 30
            anchors.left: buttonReboot.right
            anchors.leftMargin: 60

            mouseArea.onClicked: {
                settingsLauncher.launch("fbv -d 1 /bye.png")
                console.debug("now shutdown")
                settingsLauncher.launch("halt");
            }
        }
    }
}
