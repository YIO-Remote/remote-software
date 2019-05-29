import QtQuick 2.11
import QtQuick.Controls 2.4

import "qrc:/basic_ui" as BasicUI

Item {
    id: main_container
    width: parent.width
    height: parent.height
    clip: true

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO ALL INTEGRATIONS ONCE THE UI IS LOADED
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Component.onCompleted: {
        for (var i=0; i<integrationObj.length; i++) {
            integration[config.integration[i].type].connectionOpen = true
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN CONTAINER CONTENT
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias mainNavigationSwipeview: mainNavigationSwipeview
    property int itemsLoaded: 0
    property bool startUp: false

    SwipeView {
        id: mainNavigationSwipeview
        width: parent.width-20
        height: parent.height-statusBar.height-mainNavigation.height-miniMediaPlayer.height
        anchors.top: statusBar.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        clip: true

        currentIndex: 0 //mainNavigation.menuConfig.count-1

        Repeater {
            id: mainNavigationRepeater
            model: mainNavigation.menuConfig

            Loader {
                id: mainNavigationLoader
                active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem

                property alias mainNavigationLoader: mainNavigationLoader

                function determinePageToLoad(name) {
                    if (name === "favorites") {
                        mainNavigationLoader.source = "qrc:/basic_ui/pages/dashboard.qml";
                    } else if (name === "area") {
                        mainNavigationLoader.setSource("qrc:/basic_ui/pages/area.qml", { "area": display_name });
                    } else if (name === "settings") {
                        mainNavigationLoader.source = "qrc:/basic_ui/pages/settings.qml";
                    } else {
                        mainNavigationLoader.setSource("qrc:/basic_ui/pages/device.qml", { "type": name });
                    }
                }

                Component.onCompleted: {
                    determinePageToLoad(name);
                }

                onStatusChanged: {
                    if (status == Loader.Ready) {
                        itemsLoaded += 1
                    }
                }
            }
        }

        onCurrentIndexChanged: {
            if (mainNavigationSwipeview.count == mainNavigation.menuConfig.count && !startUp) {
                startUp = true
                mainNavigationSwipeview.currentIndex = 0
            }

            if (itemsLoaded >= 3) {
                if (!mainNavigation.mainNavigationListView.currentItem.held) {
                    mainNavigation.mainNavigationListView.currentIndex = currentIndex
                    //                    mainNavigation.mainNavigationListView.positionViewAtIndex(currentIndex, ListView.Center)
                }
            }
        }
    }

    onItemsLoadedChanged: {
        if (itemsLoaded >= 2) {
            mainNavigation.state = Qt.binding(function() {
                if (mainNavigationSwipeview.currentItem.mainNavigationLoader.item && mainNavigationSwipeview.currentItem.mainNavigationLoader.item.atYBeginning) {
                    return "open"
                } else {
                    return "closed"
                }
            })

            bottomGradient.opacity = Qt.binding(function() {
                if (mainNavigationSwipeview.currentItem.mainNavigationLoader.item && mainNavigationSwipeview.currentItem.mainNavigationLoader.item.atYEnd) {
                    return 0
                } else {
                    return 1
                }
            })
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

        opacity: 1 //mainNavigationSwipeview.currentItem.mainNavigationLoader.item && mainNavigationSwipeview.currentItem.mainNavigationLoader.item.atYEnd ? 0 : 1

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
    BasicUI.MainNavigation {
        id: mainNavigation
        anchors.bottom: miniMediaPlayer.top
        anchors.horizontalCenter: parent.horizontalCenter
        state: "open" // mainNavigationSwipeview.currentItem.mainNavigationLoader.item && mainNavigationSwipeview.currentItem.mainNavigationLoader.item.atYBeginning ? "open" : "closed"

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
