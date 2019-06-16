import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

import "qrc:/scripts/helper.js" as JSHelper

Rectangle {
    id: lightButton

    property int entityID

    property var entity_id:             loaded_components.light.entities[entityID].entity_id
    property var friendly_name:         loaded_components.light.entities[entityID].friendly_name
    property var lstate:                loaded_components.light.entities[entityID].state
    property int brightness:            loaded_components.light.entities[entityID].brightness
    property var _color:                loaded_components.light.entities[entityID].color
    property string integrationType:    loaded_components.light.entities[entityID].integration
    property string area:               loaded_components.light.entities[entityID].area
    property var supported_features:    loaded_components.light.entities[entityID].supported_features

    property bool favorite:             loaded_components.light.entities[entityID].favorite

    Connections {
        target: integration[integrationType].obj

        onConnected: {
            lightButton.opacity = 1
            lightButton.enabled = true
        }

        onDisconnected: {
            lightButton.opacity = 0.3
            lightButton.enabled = false
        }
    }

    width: parent.width-20
    height: 125
    anchors.horizontalCenter: parent.horizontalCenter
    color: colorMedium
    radius: cornerRadius

    onBrightnessChanged: {
        if (brightness > 0 && lightButton.state != "open") {
            lightButton.state = "on"
        } else if (lightButton.state != "open") {
            lightButton.state = "closed"
        }
    }

    property var originParent: lightButton.parent
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: buttonHandler
        enabled: state == "open" ? true : false

        onButtonPress: {
            var tmp;

            if (standbyControl.mode == "on" || standbyControl.mode == "dim") {
                switch (button) {
                case "dpad up":
                    tmp = brightness+10;
                    if (tmp > 100) {
                        tmp = 100;
                    }
                    loaded_components.light.componentIntegration[integrationType].setBrightness(entity_id, tmp);
                    break;
                case "dpad down":
                    tmp = brightness-10;
                    if (tmp < 0) {
                        tmp = 0;
                    }
                    loaded_components.light.componentIntegration[integrationType].setBrightness(entity_id, tmp);
                    break;
                case "dpad middle":
                    loaded_components.light.componentIntegration[integrationType].toggle(entity_id);
                    break;
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // LAYER MASK TO MASK EVERYTHING THAT IS INSIDE THE BUTTON
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    layer.enabled: true
    layer.effect: OpacityMask {
        maskSource: Item {
            width: lightButton.width
            height: lightButton.height
            Rectangle {
                anchors.fill: parent
                radius: cornerRadius
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    state: "closed"

    states: [
        State {
            name: "closed"
            PropertyChanges {target: lightButton; width: parent.width-20; height: 125}
            PropertyChanges {target: brightnessSlider; opacity: 0}
            ParentChange { target: lightButton; parent: originParent }
        },
        State {
            name: "on"
            PropertyChanges {target: lightButton; width: parent.width-20; height: 160}
            PropertyChanges {target: brightnessSlider; opacity: 1}
        },
        State {
            name: "open"
            PropertyChanges {target: lightButton; width: 440; height: 720}
            PropertyChanges {target: brightnessSlider; opacity: 0}
            ParentChange { target: lightButton; parent: contentWrapper; x: 20; y: 80 }
        }
    ]

    transitions: [
        Transition {
            to: "closed"
            ParallelAnimation {
                PropertyAnimation { target: lightButton; properties: "width, height"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: brightnessSlider; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            }
        },
        Transition {
            to: "on"
            SequentialAnimation {
                PauseAnimation { duration: 200 }
                PropertyAnimation { target: lightButton; properties: "width, height"; easing.type: Easing.OutExpo; duration: 300 }
                PauseAnimation { duration: 200 }
                PropertyAnimation { target: brightnessSlider; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            }
        },
        Transition {
            to: "open"
            ParallelAnimation {
                PropertyAnimation { target: lightButton; properties: "width, height"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: brightnessSlider; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
                ParentAnimation {
                    NumberAnimation { properties: "x,y"; easing.type: Easing.OutExpo; duration: 300 }
                }
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
        enabled: lightButton.state == "open" ? false : true

        onPressAndHold: {
            haptic.playEffect("press");

            addToFavButton.state = "open"
        }

        onClicked: {
            haptic.playEffect("click");

            originParent = lightButton.parent

            loader_main.state = "hidden"
            lightButton.state = "open"
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
        checked: lstate === "off" ? false : true
        enabled: lightButton.state == "open" ? false: true

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
            haptic.playEffect("click");
            loaded_components.light.componentIntegration[integrationType].toggle(entity_id);
        }

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ON STATE ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property bool sliderMovedByUser: false

    Slider {
        id: brightnessSlider
        from: 0
        value: brightness
        to: 100
        stepSize: 1
        live: false

        visible: opacity > 0 ? true : false

        width: parent.width
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 12
        anchors.horizontalCenter: parent.horizontalCenter

        background: Rectangle {
            id: sliderBG
            //            x: brightnessSlider.leftPadding
            y: (brightnessSlider.height - height) / 2
            //            width: brightnessSlider.availableWidth
            height: cornerRadius * 2
            //            radius: cornerRadius
            color: colorBackground
            radius: cornerRadius

            layer.enabled: true
            layer.effect: OpacityMask {
                maskSource: Item {
                    width: sliderBG.width
                    height: sliderBG.height
                    Rectangle {
                        anchors.fill: parent
                        radius: cornerRadius
                    }
                }
            }

            Rectangle {
                width: brightnessSlider.visualPosition * parent.width
                height: parent.height
                //                                radius: cornerRadius
                color: colorHighlight //colorBackgroundTransparent

                //                Rectangle {
                //                    width: parent.height
                //                    height: parent.width
                //                    anchors.centerIn: parent
                //                    rotation: -90
                //                    gradient: Gradient {
                //                        GradientStop { position: 0.0; color: colorMedium }
                //                        GradientStop { position: 1.0; color: colorHighlight }
                //                    }
                //                }
            }
        }

        handle: Rectangle {
            x: brightnessSlider.visualPosition * (brightnessSlider.width - width)
            y: (brightnessSlider.height - height) / 2
            width: cornerRadius*2
            height: cornerRadius*2
            radius: cornerRadius
            color: colorLine
        }

        onValueChanged: {
            if (sliderMovedByUser) {
                loaded_components.light.componentIntegration[integrationType].setBrightness(entity_id, brightnessSlider.value);
                sliderMovedByUser = false;
            }
        }

        onMoved: {
            sliderMovedByUser = true;
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ADD TO FAVORITE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: addToFavButton
        width: 0
        height: 0
        radius: 200
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        color: colorHighlight

        state: "closed"

        states: [
            State {
                name: "closed"
                PropertyChanges {target: addToFavButton; width: 0; height: 0; radius: 200; color: colorHighlight}
                PropertyChanges {target: addToFavButtonCircle; opacity: 0}
                PropertyChanges {target: addToFavButtonText; opacity: 0}
            },
            State {
                name: "open"
                PropertyChanges {target: addToFavButton; width:500; height: 500; color: colorHighlight}
                PropertyChanges {target: addToFavButtonCircle; opacity: 1}
                PropertyChanges {target: addToFavButtonText; opacity: 1}
            }
        ]

        transitions: [
            Transition {
                to: "closed"
                SequentialAnimation {
                    PauseAnimation { duration: 300 }
                    ParallelAnimation {
                        PropertyAnimation { target: addToFavButtonCircle; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                        PropertyAnimation { target: addToFavButtonText; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                    }
                    PropertyAnimation { target: addToFavButton; properties: "width, height, radius"; easing.type: Easing.OutExpo; duration: 400 }
                    PropertyAnimation { target: addToFavButton; properties: "color"; duration: 1 }
                }
            },
            Transition {
                to: "open"
                SequentialAnimation {
                    PropertyAnimation { target: addToFavButton; properties: "width, height, radius"; easing.type: Easing.InExpo; duration: 400 }
                    ParallelAnimation {
                        PropertyAnimation { target: addToFavButtonCircle; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                        PropertyAnimation { target: addToFavButtonText; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                    }
                    PropertyAnimation { target: addToFavButton; properties: "color"; easing.type: Easing.InExpo; duration: 400 }
                }
            }
        ]

        MouseArea {
            anchors.fill: parent

            onClicked: {
                haptic.playEffect("click");
                addToFavButton.state = "closed"
            }
        }

        Rectangle {
            id: addToFavButtonCircle
            width: 80
            height: width
            radius: width/2
            color: colorSwitch
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 40
            opacity: 0

            states: State {
                name: "pressed"
                when: mouseAreaFav.pressed === true
                PropertyChanges {
                    target: addToFavButtonCircle
                    color: colorHighlight
                }
            }

            transitions: [
                Transition {
                    from: ""; to: "pressed"; reversible: true
                    PropertyAnimation { target: addToFavButtonCircle
                        properties: "color"; duration: 300 }
                }]

            Image {
                asynchronous: true
                width: 80
                height: 80
                fillMode: Image.PreserveAspectFit
                source: favorite ? "qrc:/images/components/fav-minus.png" : "qrc:/images/components/fav-plus.png"
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter

                ColorOverlay {
                    visible: !darkMode
                    anchors.fill: parent
                    source: parent
                    color: colorText
                }
            }

            MouseArea {
                id: mouseAreaFav
                anchors.fill: parent
                enabled: addToFavButtonCircle.opacity == 1 ? true : false

                onClicked: {
                    addToFavButton.state = "closed";
                    var tmp = loaded_components.light.entities;
                    tmp[entityID].favorite = !tmp[entityID].favorite;
                    loaded_components.light.entities = tmp;
                    for (var i=0; i<mainNavigationSwipeview.count; i++) {
                        if (mainNavigationSwipeview.itemAt(i).mainNavigationLoader.source === "qrc:/basic_ui/pages/dashboard.qml") {
                            mainNavigationSwipeview.itemAt(i).mainNavigationLoader.active = false;
                            mainNavigationSwipeview.itemAt(i).mainNavigationLoader.active = true;
                        }
                    }
                    JSHelper.saveConfig();
                }
            }
        }

        Text {
            id: addToFavButtonText
            color: colorText
            text: favorite ? qsTr("Remove from favorites") + translateHandler.emptyString : qsTr("Add to favorites") + translateHandler.emptyString
            wrapMode: Text.WordWrap
            anchors.verticalCenter: addToFavButtonCircle.verticalCenter
            anchors.left: addToFavButtonCircle.right
            anchors.leftMargin: 26
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
            opacity: 0
        }
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // OPEN STATE ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function getSource() {
        if (lightButton.state != "open") {
            return "";
        } else if (supported_features.indexOf("COLOR") > -1) {
            return "qrc:/components/light/ui/CardColor.qml";
        } else if (supported_features.indexOf("BRIGHTNESS") > -1) {
            return "qrc:/components/light/ui/CardDimmable.qml";
        } else {
            return "qrc:/components/light/ui/CardSwitch.qml";
        }
    }

    Loader {
        id: cardLoader
        width: lightButton.width
        height: lightButton.height
        asynchronous: true
        active: lightButton.state == "open"
        source: getSource() //lightButton.state != "open" ? "" : (supported_features.indexOf("BRIGHTNESS") > -1 ? "qrc:/components/light/CardDimmable.qml" : "qrc:/components/light/CardSwitch.qml")
        opacity: cardLoader.status == Loader.Ready ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutExpo
            }
        }

    }

}
