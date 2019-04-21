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
                contentHeight: dashboardFlow.height
                boundsBehavior: Flickable.DragAndOvershootBounds
                flickableDirection: Flickable.VerticalFlick

                ScrollBar.vertical: ScrollBar {
                    opacity: 0.5
                }

                Flow {
                    id: dashboardFlow
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10

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

                }
            }
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        // SUPPORTED ENTITIES
        Repeater {
            model: supported_entities

            Loader {
                active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem

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

                        Repeater {
                            id: entityRepeater
                            property int pageIndex: index

                            model: applicationWindow["entities_"+loaded_entities[entityRepeater.pageIndex]]
                            ComponentLight.Button {
                                entity_id: applicationWindow["entities_"+loaded_entities[entityRepeater.pageIndex]][index].entity_id
                                friendly_name: applicationWindow["entities_"+loaded_entities[entityRepeater.pageIndex]][index].friendly_name
                                componentID: entityRepeater.pageIndex
                                integrationType: applicationWindow["entities_"+loaded_entities[entityRepeater.pageIndex]][index].integration

                                // light specific properties
                                lstate: applicationWindow["entities_"+loaded_entities[entityRepeater.pageIndex]][index].state
                                brightness: applicationWindow["entities_"+loaded_entities[entityRepeater.pageIndex]][index].brightness
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

        opacity: mainNavigationSwipeview.currentItem.children[0].atYEnd ? 0 : 1

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
        state: mainNavigationSwipeview.currentItem.children[0].atYBeginning ? "open" : "closed"
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
