import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

import "qrc:/scripts/helper.js" as JSHelper
import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: lightButton

    property var obj

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO INTEGRATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: integration[obj.integration].obj

        onConnected: {
            lightButton.opacity = 1
            lightButton.enabled = true
        }

        onDisconnected: {
            lightButton.opacity = 0.3
            lightButton.enabled = false
        }
    }

    Behavior on opacity {
        PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
    }

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
                    tmp = obj.brightness+10;
                    if (tmp > 100) {
                        tmp = 100;
                    }
                    obj.setBrightness(tmp);
                    break;
                case "dpad down":
                    tmp = obj.brightness-10;
                    if (tmp < 0) {
                        tmp = 0;
                    }
                    obj.setBrightness(tmp);;
                    break;
                case "dpad middle":
                    obj.toggle();
                    break;
                case "top right":
                    lightButton.state = "closed"
                    loader_main.state = "visible"
                    var tmp = mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY;
                    mainNavigationSwipeview.currentItem.mainNavigationLoader.active = false;
                    mainNavigationSwipeview.currentItem.mainNavigationLoader.active = true;
                    mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY = tmp;
                    break;
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BASIC SETTINGS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    width: parent.width-20
    height: 125
    anchors.horizontalCenter: parent.horizontalCenter
    color: colorDark
    radius: cornerRadius

    property var originParent: lightButton.parent

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
            PropertyChanges {target: button; _opacity: 1}
            ParentChange { target: lightButton; parent: originParent }
        },
        State {
            name: "open"
            PropertyChanges {target: lightButton; width: 440; height: 720}
            PropertyChanges {target: button; _opacity: 0}
            ParentChange { target: lightButton; parent: contentWrapper; x: 20; y: 80 }
        }
    ]

    transitions: [
        Transition {
            to: "closed"
            ParallelAnimation {
                PropertyAnimation { target: lightButton; properties: "width, height"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: button; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            }
        },
        Transition {
            to: "open"
            ParallelAnimation {
                PropertyAnimation { target: lightButton; properties: "width, height"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: button; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
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
            text: obj.friendly_name
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

    BasicUI.CustomSwitch {
        id: button

        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 20

        checked: obj.state
        mouseArea.enabled: lightButton.state == "open" ? false: true
        mouseArea.onClicked: {
            obj.toggle();
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
        color: colorHighlight1

        state: "closed"

        states: [
            State {
                name: "closed"
                PropertyChanges {target: addToFavButton; width: 0; height: 0; radius: 200; color: colorHighlight1}
                PropertyChanges {target: addToFavButtonCircle; opacity: 0}
                PropertyChanges {target: addToFavButtonText; opacity: 0}
            },
            State {
                name: "open"
                PropertyChanges {target: addToFavButton; width:500; height: 500; color: colorHighlight1}
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
            color: colorDark
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 40
            opacity: 0

            states: State {
                name: "pressed"
                when: mouseAreaFav.pressed === true
                PropertyChanges {
                    target: addToFavButtonCircle
                    color: colorHighlight1
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
                source: obj.favorite ? "qrc:/images/components/fav-minus.png" : "qrc:/images/components/fav-plus.png"
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
                    obj.favorite = !obj.favorite;

                    for (var i=0; i<mainNavigationSwipeview.count; i++) {
                        if (mainNavigationSwipeview.itemAt(i).mainNavigationLoader.source == "qrc:/basic_ui/pages/dashboard.qml") {
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
            text: obj.favorite ? qsTr("Remove from favorites") + translateHandler.emptyString : qsTr("Add to favorites") + translateHandler.emptyString
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
        } else if (obj.supported_features.indexOf("COLOR") > -1) {
            return "qrc:/components/light/ui/CardColor.qml";
        } else if (obj.supported_features.indexOf("BRIGHTNESS") > -1) {
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
        source: getSource() //lightButton.state != "open" ? "" : (obj.supported_features.indexOf("obj.brightness") > -1 ? "qrc:/components/light/CardDimmable.qml" : "qrc:/components/light/CardSwitch.qml")
        opacity: cardLoader.status == Loader.Ready ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutExpo
            }
        }

    }

}
