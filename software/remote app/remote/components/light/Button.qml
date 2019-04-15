import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

Rectangle {
    id: lightButton

    property var entity_id
    property var friendly_name
    property var lstate
    property int brightness
    property int componentID

    property bool favorite: false

    width: parent.width
    height: 125
    color: colorMedium
    radius: cornerRadius

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    state: "closed"

    states: [
        State {
            name: "closed"
            PropertyChanges {target: lightButton; height: 125}
            PropertyChanges {target: brightnessSlider; visible: false; opacity: 0}
        },
        State {
            name: "on"
            when: lstate == "on"
            PropertyChanges {target: lightButton; height: 160}
            PropertyChanges {target: brightnessSlider; visible: true; opacity: 1}
        },
        State {
            name: "open"
            PropertyChanges {target: lightButton; height: 720}
            PropertyChanges {target: brightnessSlider; visible: false; opacity: 0}
        }
    ]

    transitions: [
        Transition {
            to: "closed"
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: lightButton; properties: "height"; easing.type: Easing.OutExpo; duration: 300 }
                    PropertyAnimation { target: brightnessSlider; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                }
                PropertyAnimation { target: brightnessSlider; properties: "visible"; easing.type: Easing.OutExpo; duration: 300 }
            }
        },
        Transition {
            to: "on"
            SequentialAnimation {
                PropertyAnimation { target: brightnessSlider; properties: "visible"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: lightButton; properties: "height"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: brightnessSlider; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            }
        },
        Transition {
            to: "open"
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: lightButton; properties: "height"; easing.type: Easing.OutExpo; duration: 300 }
                    PropertyAnimation { target: brightnessSlider; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                }
                PropertyAnimation { target: brightnessSlider; properties: "visible"; easing.type: Easing.OutExpo; duration: 300 }
            }
        }
    ]


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MOUSEAREAS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MouseArea {
        anchors.fill: parent
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: lightButton.state == "open" ? false: true

        onPressAndHold: {
            //            addToFavButton.state = "open"
        }

        onClicked: {
            //            FN.loadScreen("ButtonLight", button_main)
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CLOSED STATE ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Item {
        width: parent.width
        height: 125
        anchors.top: parent.top

        Text {
            id: title
            color: colorText
            text: friendly_name
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            width: parent.width-232
            anchors.left: parent.left
            anchors.leftMargin: 126
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Image {
            id: icon
            asynchronous: true
            width: 80
            height: 80
            fillMode: Image.PreserveAspectFit
            source: "qrc:/components/light/images/icon-light.png"
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter

            ColorOverlay {
                visible: !darkMode
                anchors.fill: parent
                source: parent
                color: colorText
                antialiasing: true
            }
        }

    }

    Switch {
        id: button
        anchors.top: parent.top
        anchors.topMargin: 20
        implicitHeight: 36
        implicitWidth: 66
        anchors.right: parent.right
        anchors.rightMargin: 20
        checked: lstate == "off" ? false : true

        indicator: Rectangle {
            x: button.visualPosition * (button.width - width)
            y: (button.height - height) / 2
            width: 36
            height: 36
            radius: cornerRadius
            color: button.checked ? colorSwitchOn : colorSwitch

            Behavior on x {
                enabled: !button.pressed
                SmoothedAnimation { velocity: 150 }
            }
        }

        background: Rectangle {
            radius: cornerRadius+2
            color: button.checked ? colorHighlight : colorSwitchBackground
        }

        onClicked: {
            loaded_components[componentID].lightComponentIntegration.toggle(entity_id);
        }

    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ON STATE ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property bool sliderMovedByUser: false

    Slider {
        id: brightnessSlider
        from: 1
        value: brightness
        to: 100
        stepSize: 1
        live: false

        width: parent.width-40
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        background: Rectangle {
            x: brightnessSlider.leftPadding
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
            if (sliderMovedByUser) {
                loaded_components[componentID].lightComponentIntegration.setBrightness(entity_id, brightnessSlider.value);
                sliderMovedByUser = false;
            }
        }

        onMoved: {
            sliderMovedByUser = true;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // OPEN STATE ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
