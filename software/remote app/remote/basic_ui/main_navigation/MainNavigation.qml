import QtQuick 2.11

Item {
    id: mainNavigation
    width: parent.width-20

    state: "open"

    states: [
        State {
            name: "closed"
            PropertyChanges {target: mainNavigation; height: 60}
        },
        State {
            name: "open"
            PropertyChanges {target: mainNavigation; height: 130}
        }
    ]

    transitions: [
        Transition {
            to: "closed"
            PropertyAnimation { target: mainNavigation; properties: "height"; easing.type: Easing.OutExpo; duration: 300 }
        },
        Transition {
            to: "open"
            PropertyAnimation { target: mainNavigation; properties: "height"; easing.type: Easing.OutExpo; duration: 300 }
        }
    ]

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN NAVIGATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Flickable {
        id: mainNavigationFlickable
        width: parent.width
        height: parent.height
        anchors.bottom: parent.bottom
        maximumFlickVelocity: 2000
        flickDeceleration: 400
        clip: true
        boundsBehavior: Flickable.DragAndOvershootBounds
        flickableDirection: Flickable.HorizontalFlick
        contentWidth: navigationGrid.width-30
        contentX: setNavigationPosition()

        function setNavigationPosition() {
            return mainNavigationSwipeview.currentIndex*20
        }

        Behavior on contentX {
            SequentialAnimation {
                NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
            }
        }

        Grid {
            id: navigationGrid
            spacing: 0
            rows: 1
            //            width: parent.width
            height: parent.height

            NavigationButton {
                buttonText: qsTr("Dashboard") + translateHandler.emptyString
                icon.source: "qrc:/images/navigation/icon-dashboard.png"
                selected: mainNavigationSwipeview.currentIndex == 0 ? true : false
                mouseArea.onClicked: {
                    mainNavigationSwipeview.currentIndex = 0
                }
            }

            NavigationButton {
                buttonText: qsTr("Rooms") + translateHandler.emptyString
                icon.source: "qrc:/images/navigation/icon-rooms.png"
                selected: mainNavigationSwipeview.currentIndex == 1 ? true : false
                mouseArea.onClicked: {
                    mainNavigationSwipeview.currentIndex = 1
                }
            }

            Repeater {
                model: loaded_entities
                NavigationButton {
                    buttonText: supported_entities_translation[loaded_entities_id[index]]
                    icon.source: "qrc:/images/navigation/icon-"+loaded_entities[index]+".png"
                    selected: mainNavigationSwipeview.currentIndex == index + 2 ? true : false
                    mouseArea.onClicked: {
                        mainNavigationSwipeview.currentIndex = index + 2
                    }
                }
            }

            NavigationButton {
                buttonText: qsTr("Settings") + translateHandler.emptyString
                icon.source: "qrc:/images/navigation/icon-settings.png"
                selected: mainNavigationSwipeview.currentIndex == 2 + supported_entities.length ? true : false
                mouseArea.onClicked: {
                    mainNavigationSwipeview.currentIndex = 2 + supported_entities.length
                }
            }
        }

    }

    Rectangle { //left gradient fade
        width: parent.height
        height: parent.height
        anchors.left: parent.left
        anchors.verticalCenter: mainNavigation.verticalCenter
        rotation: 90

        opacity: mainNavigationFlickable.atXBeginning ? 0 : 1

        Behavior on opacity {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutExpo
            }
        }

        gradient: Gradient {
            GradientStop { position: 0.0; color: colorBackgroundTransparent }
            GradientStop { position: 1.0; color: colorBackground }
        }
    }

    Rectangle { //right gradient fade
        width: parent.height
        height: parent.height
        anchors.right: parent.right
        anchors.verticalCenter: mainNavigation.verticalCenter
        rotation: 90

        opacity: mainNavigationFlickable.atXEnd ? 0 : 1

        Behavior on opacity {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutExpo
            }
        }

        gradient: Gradient {
            GradientStop { position: 1.0; color: colorBackgroundTransparent }
            GradientStop { position: 0.0; color: colorBackground }
        }
    }


}
