import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Item {
    id: main_container
    width: parent.width
    height: parent.height
    clip: true
    enabled: loader_main.state === "visible" ? true : false
    layer.enabled: true

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO ALL INTEGRATIONS ONCE THE UI IS LOADED
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Component.onCompleted: {
        for (var key in integration) {
            if (integration.hasOwnProperty(key)) {
                integration[key].obj.connect();
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: buttonHandler
        enabled: loader_main.state === "visible" && standbyControl.mode === "on" ? true : false

        onButtonPress: {
            switch (button) {
            case "dpad right":
                if (loader_main.item.mainNavigationSwipeview.currentIndex < loader_main.item.mainNavigationSwipeview.count-1) {
                    loader_main.item.mainNavigationSwipeview.currentIndex += 1;
                    //                    mainNavigation.mainNavigationListView.currentIndex += 1;
                } else {
                    haptic.playEffect("buzz");
                }
                break;
            case "dpad left":
                if (loader_main.item.mainNavigationSwipeview.currentIndex > 0) {
                    loader_main.item.mainNavigationSwipeview.currentIndex -= 1;
                    //                    mainNavigation.mainNavigationListView.currentIndex -= 1;
                } else {
                    haptic.playEffect("buzz");
                }
                break;
            case "dpad up":
                mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY -= 200;
                if (mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY <= 0) {
                    mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY = 0;
                    haptic.playEffect("buzz");
                }
                break;
            case "dpad down":
                mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY += 200;
                if (mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY >= (mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentHeight - mainNavigationSwipeview.currentItem.mainNavigationLoader.item.height)) {
                    mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY = mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentHeight - mainNavigationSwipeview.currentItem.mainNavigationLoader.item.height;
                    haptic.playEffect("buzz");
                }
                break;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // AUTO ROOM SELECTION BASED ON BLUETOOTH TAGS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: bluetoothArea
        enabled: config.settings.bluetootharea

        onCurrentAreaChanged: {
            if (config.settings.menu.order[mainNavigationSwipeview.currentIndex].display_name != bluetoothArea.currentArea) {
                for (var i=0; config.settings.menu.order.length; i++) {
                    if (config.settings.menu.order[i] && config.settings.menu.order[i].display_name == bluetoothArea.currentArea) {
                        mainNavigationSwipeview.currentIndex = i;
                    }
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN CONTAINER CONTENT
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias mainNavigationSwipeview: mainNavigationSwipeview
    property int itemsLoaded: 0
    property bool startUp: false

    property int prevIndex: 0

    SwipeView {
        id: mainNavigationSwipeview
        width: parent.width
        height: parent.height-statusBar.height-miniMediaPlayer.height
        anchors.top: statusBar.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        currentIndex: 0 //mainNavigation.menuConfig.count-1

        Repeater {
            id: mainNavigationRepeater
            model: mainNavigation.menuConfig

            Loader {
                id: mainNavigationLoader
                asynchronous: true
                //active: SwipeView.isCurrentItem //|| SwipeView.isNextItem || SwipeView.isPreviousItem

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
            // change navigation index after startup
            if (mainNavigationSwipeview.count == mainNavigation.menuConfig.count && !startUp) {
                startUp = true
                mainNavigationSwipeview.currentIndex = 0
            }

            if (startUp) {
                mainNavigation.mainNavigationListView.currentIndex = currentIndex;
            }

            if (itemsLoaded >= 3) {
                if (!mainNavigation.mainNavigationListView.currentItem && !mainNavigation.mainNavigationListView.currentItem.held) {
                    mainNavigation.mainNavigationListView.currentIndex = currentIndex
                    //                    mainNavigation.mainNavigationListView.positionViewAtIndex(currentIndex, ListView.Center)
                }
            }

            // change the statusbar title
            if (currentIndex != prevIndex && mainNavigationSwipeview.currentItem.mainNavigationLoader.item && mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY < 130) {
                statusBar.title = "";
            } else if (mainNavigationSwipeview.currentItem.mainNavigationLoader.item) {
                statusBar.title = mainNavigationSwipeview.currentItem.mainNavigationLoader.item.title;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BOTTOM GRADIENT FADE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    Image {
//        id: bottomGradient
//        width: 480
//        height: 80
//        anchors.bottom: miniMediaPlayer.top
//        asynchronous: true
//        fillMode: Image.Stretch
//        source: "qrc:/images/navigation/bottom_gradient.png"

//        Behavior on opacity {
//            NumberAnimation {
//                duration: 300
//                easing.type: Easing.OutExpo
//            }
//        }
//    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // PAGE INDICATOR
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    PageIndicator {
//        id: indicator

//        count: mainNavigationSwipeview.count
//        currentIndex: mainNavigationSwipeview.currentIndex

//        anchors.bottom: miniMediaPlayer.top
//        anchors.bottomMargin: 10
//        anchors.horizontalCenter: parent.horizontalCenter
//        opacity: mainNavigation.y == 800 ? 1 : 0

//        Behavior on opacity {
//            NumberAnimation { duration: 300; easing.type: Easing.InOutExpo }
//        }

//        delegate: Rectangle {
//            width: 8
//            height: 8
//            radius: height/2
//            color: colorText
//            opacity: index == mainNavigationSwipeview.currentIndex ? 1 : 0.3
//        }
//    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MINI MEDIA PLAYER
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias miniMediaPlayer: miniMediaPlayer
    Item {
        id: miniMediaPlayer
        width: parent.width
        height: 0
        anchors.bottom: mainNavigation.top

        property alias miniMediaPlayerLoader: miniMediaPlayerLoader

        Loader {
            id: miniMediaPlayerLoader
            active: false
            anchors.fill: parent
        }

        function onPlay(name) {
            if (!miniMediaPlayerLoader.active) {
                miniMediaPlayer.height = 80;
                miniMediaPlayerLoader.setSource("qrc:/basic_ui/MiniMediaPlayer.qml")
                miniMediaPlayerLoader.active = true;
            } else if (miniMediaPlayerLoader.active && miniMediaPlayerLoader.status == Loader.Ready) {
                miniMediaPlayerLoader.item.add(name);
            }
        }

        function onStopped(name) {
            if (miniMediaPlayerLoader.active && miniMediaPlayerLoader.status == Loader.Ready) {
                miniMediaPlayerLoader.item.remove(name);
            }
        }

        Component.onCompleted: {
            var e = entities.getByType("media_player");
            for (var i=0; i<e.length; i++) {
                e[i].playing.connect(onPlay);
                e[i].stopped.connect(onStopped);
            }
        }

        Behavior on height {
            NumberAnimation { duration: 400; easing.type: Easing.InOutExpo }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN NAVIGATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias mainNavigation: mainNavigation

    BasicUI.MainNavigation {
        id: mainNavigation
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STATUS BAR
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BasicUI.StatusBar {
        id: statusBar
    }

}
