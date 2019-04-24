import QtQuick 2.11
import QtQuick.Controls 2.4

import "qrc:/basic_ui" as BasicUI
import "qrc:/basic_ui/main_navigation" as Navigation
import "qrc:/components/light" as ComponentLight

Item {
    id: main_container
    width: parent.width
    height: parent.height
    clip: true

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN CONTAINER CONTENT
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias mainNavigationSwipeview: mainNavigationSwipeview

    SwipeView {
        id: mainNavigationSwipeview
        width: parent.width-20
        height: parent.height-statusBar.height-mainNavigation.height-miniMediaPlayer.height
        anchors.top: statusBar.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        clip: true

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // DASHBOARD
        Loader {
            active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem

            sourceComponent: Flickable {
                id: dashboardFlickable
                width: parent.width
                height: parent.height
                maximumFlickVelocity: 4000
                flickDeceleration: 1000
                clip: true
                contentHeight: dashboardFlow.height < parent.height - mainNavigation.height - statusBar.height - miniMediaPlayer.height + bottomGradient.height ? dashboardFlow.height + 40 : dashboardFlow.height
                boundsBehavior: Flickable.DragAndOvershootBounds
                flickableDirection: Flickable.VerticalFlick

                ScrollBar.vertical: ScrollBar {
                    opacity: 0.5
                }

                property alias dashboardFlow: dashboardFlow

                Flow {
                    id: dashboardFlow
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10

                    Component.onCompleted: {
                        for (var i=0; i<loaded_entities.length; i++) {
                            for (var j=0; j<loaded_components[loaded_entities[i]].entities.length; j++) {
                                if (loaded_components[loaded_entities[i]].entities[j].favorite) {
                                    // load entity button
                                    var comp = Qt.createComponent("qrc:/components/"+ loaded_entities[i] +"/Button.qml");
                                    var obj = comp.createObject(dashboardFlow, {entityID: j});
                                }
                            }
                        }
                    }
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // ROOMS
        Loader {
            active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem

            sourceComponent: Flickable {
                width: parent.width
                height: parent.height
                maximumFlickVelocity: 4000
                flickDeceleration: 1000
                clip: true
                contentHeight: roomsFlow.height
                boundsBehavior: Flickable.DragAndOvershootBounds
                flickableDirection: Flickable.VerticalFlick

                ScrollBar.vertical: ScrollBar {
                    opacity: 0.5
                }

                Flow {
                    id: roomsFlow
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10

                    Component.onCompleted: {
                        for (var k=0; k<config.rooms.length; k++) {
                            // load room title
                            if (k>0) {
                                var spacerObj = Qt.createQmlObject('import QtQuick 2.0; Rectangle {color: colorBackgroundTransparent; width: parent.width; height: 40;}', roomsFlow, '')
                            }
                            var roomObj = Qt.createQmlObject('import QtQuick 2.0; Text {color: colorText; font.family: "Open Sans"; font.weight: Font.Normal; font.pixelSize: 60; text: "'+ config.rooms[k].room +'";}', roomsFlow, "");

                            // load room entities
                            // go through all entities, if it matches the room, create a component
                            for (var i=0; i<loaded_entities.length; i++) {
                                for (var j=0; j<loaded_components[loaded_entities[i]].entities.length; j++) {
                                    if (loaded_components[loaded_entities[i]].entities[j].room == config.rooms[k].room) {
                                        // load entity button
                                        var comp = Qt.createComponent("qrc:/components/"+ loaded_entities[i] +"/Button.qml");
                                        var obj = comp.createObject(roomsFlow, {entityID: j});
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // SUPPORTED ENTITIES
        Repeater {
            model: loaded_entities

            Loader {
                active: SwipeView.isCurrentItem || SwipeView.isNextItem || (SwipeView.isPreviousItem && (mainNavigationSwipeview.currentIndex !=  2 + supported_entities.length))

                sourceComponent: Flickable {
                    width: parent.width
                    height: parent.height
                    maximumFlickVelocity: 4000
                    flickDeceleration: 1000
                    clip: true
                    contentHeight: repeaterFlow.height
                    boundsBehavior: Flickable.DragAndOvershootBounds
                    flickableDirection: Flickable.VerticalFlick

                    ScrollBar.vertical: ScrollBar {
                        opacity: 0.5
                    }

                    Flow {
                        id: repeaterFlow
                        width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 10

                        Component.onCompleted: {
                            // load room entities
                            // go through all entities, if it matches the room, create a component
                            for (var i=0; i<loaded_entities.length; i++) {
                                for (var j=0; j<loaded_components[loaded_entities[i]].entities.length; j++) {
                                    if (loaded_components[loaded_entities[i]].entities[j].type == loaded_entities[index]) {
                                        // load entity button
                                        var comp = Qt.createComponent("qrc:/components/"+ loaded_entities[i] +"/Button.qml");
                                        var obj = comp.createObject(repeaterFlow, {entityID: j});
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // SETTINGS
        Loader {
            active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem

            sourceComponent: Flickable {
                width: parent.width
                height: parent.height
                maximumFlickVelocity: 4000
                flickDeceleration: 1000
                clip: true
                contentHeight: settingsPage.height
                boundsBehavior: Flickable.DragAndOvershootBounds
                flickableDirection: Flickable.VerticalFlick

                ScrollBar.vertical: ScrollBar {
                    opacity: 0.5
                }

                BasicUI.Settings {
                    id: settingsPage
                }
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BOTTOM GRADIENT FADE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: bottomGradient
        width: parent.width
        height: mainNavigation.state == "closed" ? 80 : 160
        anchors.bottom: mainNavigation.top

        opacity: mainNavigationSwipeview.currentItem.children[0] && mainNavigationSwipeview.currentItem.children[0].atYEnd ? 0 : 1

        Behavior on opacity {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutExpo
            }
        }

        gradient: Gradient {
            GradientStop { position: 0.0; color: colorBackgroundTransparent }
            GradientStop { position: 0.9; color: colorBackground }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN NAVIGATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Navigation.MainNavigation {
        id: mainNavigation
        anchors.bottom: miniMediaPlayer.top
        anchors.horizontalCenter: parent.horizontalCenter
        state: mainNavigationSwipeview.currentItem.children[0] && mainNavigationSwipeview.currentItem.children[0].atYBeginning ? "open" : "closed"
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MINI MEDIA PLAYER
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Rectangle {
        id: miniMediaPlayer
        width: parent.width
        height: 0
        anchors.bottom: parent.bottom
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATUS BAR
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BasicUI.StatusBar {
        id: statusBar
    }
}
