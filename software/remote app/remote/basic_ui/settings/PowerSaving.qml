import QtQuick 2.11
import QtQuick.Controls 2.4

Item {
    width: parent.width
    height: header.height + section.height + 20

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // POWER SAVING
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text {
        id: header
        color: colorText
        text: translations[language].settings.powersaving
        anchors.top: parent.top
        anchors.left: parent.left
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 27
        lineHeight: 1
    }

    Rectangle {
        id: section
        width: parent.width
        height: 405
        radius: cornerRadius
        color: colorMedium

        anchors.top: header.bottom
        anchors.topMargin: 20

        Text {
            id: wifioffText
            color: colorText
            text: standbyControl.wifiOffTime == 0 ? translations[language].settings.wifiofftext3 : translations[language].settings.wifiofftext1 + "\n" + standbyControl.wifiOffTime/60 + " " + translations[language].settings.wifiofftext2
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
            anchors.top: section.top
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
            text: translations[language].settings.never
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
            text: "60 " + translations[language].settings.minutes
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
            text: standbyControl.shutdownTime == 0 ? translations[language].settings.shutdowntext3 : translations[language].settings.shutdowntext1 + "\n" + standbyControl.shutdownTime/60/60 + " " + translations[language].settings.shutdowntext2
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
            text: translations[language].settings.never
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
            text: "8 " + translations[language].settings.hours
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.top: shutdownSlider.bottom
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 20
            lineHeight: 1
        }
    }
}
