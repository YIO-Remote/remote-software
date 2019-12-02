import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import "qrc:/scripts/helper.js" as JSHelper
import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: weatherButton

    property var obj

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO INTEGRATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: obj.integrationObj //integrations.getByType(obj.integration) //integration[obj.integration].obj

        onConnected: {
            weatherButton.opacity = 1
            weatherButton.enabled = true
        }

        onDisconnected: {
            weatherButton.opacity = 0.3
            weatherButton.enabled = false
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

            if (standbyControl.mode === "on" || standbyControl.mode === "dim") {
                switch (button) {
                case "top right":
                    weatherButton.state = "closed"
                    loader_main.state = "visible"
                    break;
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BASIC SETTINGS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    width: 460
    height: 125
    anchors.horizontalCenter: parent.horizontalCenter
    color: colorDark
    radius: cornerRadius

    property var originParent: weatherButton.parent

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // LAYER MASK TO MASK EVERYTHING THAT IS INSIDE THE BUTTON
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    layer.enabled: true
    layer.effect: OpacityMask {
        maskSource: Item {
            width: weatherButton.width
            height: weatherButton.height
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
            PropertyChanges {target: weatherButton; width: 460; height: 125; scale: 1}
            ParentChange { target: weatherButton; parent: originParent; scale: 1}
            PropertyChanges {target: loader_main; state: "visible" }
        },
        State {
            name: "open"
            PropertyChanges {target: weatherButton; width: 440; height: 720}
            ParentChange { target: weatherButton; parent: contentWrapper; x: 20; y: 80; scale: 1}
            PropertyChanges {target: loader_main; state: "hidden" }
        }
    ]

    transitions: [
        Transition {
            to: "closed"
            ParallelAnimation {
                PropertyAnimation { target: weatherButton; properties: "width, height"; easing.type: Easing.OutExpo; duration: 300 }
                ParentAnimation {
                    NumberAnimation { properties: "x, y, scale"; easing.type: Easing.OutExpo; duration: 300 }
                }
            }
        },
        Transition {
            to: "open"
            ParallelAnimation {
                PropertyAnimation { target: weatherButton; properties: "width, height"; easing.type: Easing.OutBack; easing.overshoot: 0.8; duration: 300 }
                ParentAnimation {
                    NumberAnimation { properties: "x, y, scale"; easing.type: Easing.OutBack; easing.overshoot: 0.8; duration: 300 }
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
        enabled: weatherButton.state == "open" ? false : true

        onPressAndHold: {
            haptic.playEffect("press");

            addToFavButton.state = "open"
        }

        onClicked: {
            haptic.playEffect("click");

            originParent = weatherButton.parent

            loader_main.state = "hidden"
            weatherButton.state = "open"
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

        Text {
            id: icon
            color: colorText
            text: "\uE901"
            renderType: Text.NativeRendering
            width: 85
            height: 85
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 100 }
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            id: temp
            color: colorText
            text: obj.current.temp
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            wrapMode: Text.WordWrap
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
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
                    obj.favorite = !obj.favorite;
                    addToFavButton.state = "closed";
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
        width: weatherButton.width
        height: weatherButton.height
        asynchronous: true
        active: weatherButton.state == "open"
        source: "qrc:/components/weather/ui/Card.qml"
//        opacity: cardLoader.status == Loader.Ready ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutExpo
            }
        }

    }

}
