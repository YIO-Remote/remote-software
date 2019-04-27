import QtQuick 2.11
import QtQuick.Controls 2.4

import "qrc:/basic_ui" as BasicUI
import "qrc:/components/light" as ComponentLight

Item {
    id: main_container
    width: parent.width
    height: parent.height
    clip: true

    property int listWidth: parent.width-20
    property int listHeight: parent.height-statusBar.height-mainNavigation.height-miniMediaPlayer.height

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN CONTAINER CONTENT
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias mainNavigationSwipeview: mainNavigationSwipeview

    ListView {
        id: mainNavigationSwipeview

        width: parent.width-20
        height: parent.height-statusBar.height-mainNavigation.height-miniMediaPlayer.height
        anchors.top: statusBar.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        maximumFlickVelocity: 2000
        flickDeceleration: 400
        clip: true
        boundsBehavior: Flickable.DragAndOvershootBounds
        flickableDirection: Flickable.HorizontalFlick
        orientation: ListView.Horizontal
        interactive: true
        focus: true
        highlightMoveDuration: 200
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange

        currentIndex: 0

        model: mainNavigation.menuConfig

        delegate: swipeViewPage

        onCurrentIndexChanged: {
            if (!mainNavigation.mainNavigationListView.currentItem.held) {
                mainNavigation.mainNavigationListView.currentIndex = currentIndex
                //            mainNavigation.mainNavigationListView.positionViewAtIndex(currentIndex, ListView.Center)
            }
        }

    }

    Component {
        id: swipeViewPage

        Item {
            id: contentWrapper
            width: listWidth
            height: listHeight

            property bool selected: ListView.isCurrentItem
//            property string nameValue: name
//            property string displayNameValue: display_name

            property alias pageLoader: pageLoader

            // figure out which page type to load
            function determinePageToLoad(name) {
                if (name === "dashboard") {
                    pageLoader.source = "qrc:/basic_ui/pages/Dashboard.qml";
                } else if (name === "area") {
                   pageLoader.setSource("qrc:/basic_ui/pages/Area.qml", { "area": display_name });
                } else if (name === "settings") {
                    pageLoader.source = "qrc:/basic_ui/pages/Settings.qml";
                } else {
                    pageLoader.setSource("qrc:/basic_ui/pages/Device.qml", { "type": name });
                }
            }

            Component.onCompleted: {
                determinePageToLoad(name);
            }

            Loader {
                id: pageLoader
                //                                active: selected
                asynchronous: true
                width: listWidth
                height: listHeight
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

        opacity: mainNavigationSwipeview.currentItem && mainNavigationSwipeview.currentItem.pageLoader.status == Loader.Ready && mainNavigationSwipeview.currentItem.pageLoader.item.atYEnd ? 0 : 1

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
        state: /*mainNavigationSwipeview.currentItem && mainNavigationSwipeview.currentItem.pageLoader.status == Loader.Ready &&*/ mainNavigationSwipeview.currentItem.pageLoader.item.atYBeginning ? "open" : "closed"

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
