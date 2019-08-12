import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

import "qrc:/scripts/helper.js" as JSHelper
import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: mediaButton

    property var obj

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO INTEGRATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: integration[obj.integration].obj

        onConnected: {
            mediaButton.opacity = 1
            mediaButton.enabled = true
        }

        onDisconnected: {
            mediaButton.opacity = 0.3
            mediaButton.enabled = false
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

    property var originParent: mediaButton.parent

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // LAYER MASK TO MASK EVERYTHING THAT IS INSIDE THE BUTTON
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    layer.enabled: true
    layer.effect: OpacityMask {
        maskSource: Item {
            width: mediaButton.width
            height: mediaButton.height
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
            PropertyChanges {target: mediaButton; width: parent.width-20; height: 125}
            PropertyChanges {target: button; _opacity: 1}
            ParentChange { target: mediaButton; parent: originParent }
        },
        State {
            name: "open"
            PropertyChanges {target: mediaButton; width: 440; height: 720}
            PropertyChanges {target: button; _opacity: 0}
            ParentChange { target: mediaButton; parent: contentWrapper; x: 20; y: 80 }
        }
    ]

    transitions: [
        Transition {
            to: "closed"
            ParallelAnimation {
                PropertyAnimation { target: mediaButton; properties: "width, height"; easing.type: Easing.OutExpo; duration: 300 }
                PropertyAnimation { target: button; properties: "opacity"; easing.type: Easing.OutExpo; duration: 300 }
            }
        },
        Transition {
            to: "open"
            ParallelAnimation {
                PropertyAnimation { target: mediaButton; properties: "width, height"; easing.type: Easing.OutExpo; duration: 300 }
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
        enabled: mediaButton.state == "open" ? false : true

        onPressAndHold: {
            haptic.playEffect("press");

            addToFavButton.state = "open"
        }

        onClicked: {
            haptic.playEffect("click");

            originParent = mediaButton.parent

            loader_main.state = "hidden"
            mediaButton.state = "open"
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
            source: "qrc:/components/blind/images/icon-blind.png"
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
        mouseArea.enabled: mediaButton.state == "open" ? false: true
        mouseArea.onClicked: {
            if (obj.state) {
                obj.turnOff();
            } else {
                obj.turnOn();
            }
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

    Loader {
        id: cardLoader
        width: mediaButton.width
        height: mediaButton.height
        asynchronous: true
        active: mediaButton.state == "open"
        source: "qrc:/components/blind/ui/Card.qml"
        opacity: cardLoader.status == Loader.Ready ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutExpo
            }
        }

    }

}
