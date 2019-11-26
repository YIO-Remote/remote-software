import QtQuick 2.11
import QtGraphicalEffects 1.0
import QtQml.Models 2.3

import "qrc:/scripts/helper.js" as JSHelper

Item {
    id: mainNavigation
    width: parent.width
    height: 70

    Rectangle {
        anchors.fill: parent
        color: colorBackground
    }

    MouseArea {
        anchors.fill: parent
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MENU CONFIGURATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias menuConfig: menuConfig

    ListModel {
        id: menuConfig
    }

    Connections {
        target: config

        onProfileChanged: {
            loadmenuConfig();
        }
    }

    //: menu items that are in the bottom menu
    property var menuTranslations: [qsTr("Favorites") + translateHandler.emptyString, qsTr("Settings") + translateHandler.emptyString,
        qsTr("Lights") + translateHandler.emptyString, qsTr("Blinds") + translateHandler.emptyString, qsTr("Media") + translateHandler.emptyString
    ]

    function loadmenuConfig() {
        // clear the menuConfig
        menuConfig.clear();

        var pageNames = config.getProfilePages();

        for (var j = 0; j < pageNames.length; j++) {
            var c = {};
            c.page = pageNames[j];
            if (c.page === "favorites") {
                c.friendly_name = "Favorites";
            } else if (c.page === "settings") {
                c.friendly_name = "Settings";
            } else {
                c.friendly_name = config.getPage(c.page).name;
            }

            // add to listmodel
            menuConfig.append(c);
        }
    }

    function savemenuConfig() {
        console.debug("SAVE MENUCONFIG");

        var tmp = config.read;

        var newConfig = [];

        for (var i = 0; i < menuConfig.count; i++) {
            var found = false;

            for (var j = 0; j < tmp.ui_config.profiles[config.profile].pages.length && !found; j++) {
                if (tmp.ui_config.profiles[config.profile].pages[j] == menuConfig.get(i).page) {
                    newConfig.push(tmp.ui_config.profiles[config.profile].pages[j]);
                    tmp.ui_config.profiles[config.profile].pages.splice(j,1);
                    found = true;
                }
            }
        }

        // clear the config
        tmp.ui_config.profiles[config.profile].pages  = [];

        // update the config
        tmp.ui_config.profiles[config.profile].pages = newConfig;

        config.write = tmp;
        config.writeConfig();
    }

    Component.onCompleted: {
        loadmenuConfig()
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NAVIGATION ITEM
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Component {
        id: dragDelegate

        MouseArea {
            id: dragArea

            property bool selected: ListView.isCurrentItem

            property bool held: false

            width: content.width + 20
            height: content.height + 20

            drag.target: held ? content : undefined
            drag.axis: Drag.XAxis

            onPressAndHold: {
                held = true;
                haptic.playEffect("click");
            }
            onReleased: {
                if (held) {
                    savemenuConfig()
                }
                haptic.playEffect("click");

                held = false
                mainNavigationListView.currentIndex = index
                mainNavigationSwipeview.currentIndex = index
            }

            property ListView _listView: ListView.view

            property int scrollEdgeSize: 20
            property int _scrollingDirection: 0

            SmoothedAnimation {
                id: leftAnimation
                target: _listView
                property: "contentX"
                to: 0
                running: _scrollingDirection == -1
            }

            SmoothedAnimation {
                id: rightAnimation
                target: _listView
                property: "contentX"
                to: _listView.contentWidth - _listView.width
                running: _scrollingDirection == 1
            }

            states: [
                State {
                    when: dragArea.drag.active
                    name: "dragging"

                    PropertyChanges {
                        target: dragArea
                        _scrollingDirection: {
                            var xCoord = _listView.mapFromItem(dragArea, dragArea.mouseX, 0).x;

                            if (xCoord < scrollEdgeSize) {
                                -1;
                            } else if (xCoord > _listView.width - scrollEdgeSize) {
                                1;
                            } else {
                                0;
                            }
                        }
                    }
                }
            ]

            Rectangle {
                id: content

                Drag.active: dragArea.held
                Drag.source: dragArea
                Drag.hotSpot.x: width / 2
                Drag.hotSpot.y: height / 2

                width: buttonText.implicitWidth+30
                height: 50
                color: colorBackgroundTransparent
                opacity: selected ? 1 : 0.3

                states: State {
                    when: dragArea.held

                    ParentChange { target: content; parent: mainNavigation }
                    AnchorChanges {
                        target: content
                        anchors { horizontalCenter: undefined; verticalCenter: undefined }
                    }
                }

                Behavior on opacity {
                    NumberAnimation { duration: 200 }
                }

                Rectangle {
                    width: parent.width
                    height: parent.height
                    color: dragArea.held ? colorHighlight1 : ( selected ? colorHighlight2 : colorBackgroundTransparent )
                    radius: width / 2

                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }

                    Text {
                        id: buttonText
                        color: colorText
                        text: qsTr(friendly_name) + translateHandler.emptyString
                        horizontalAlignment: Text.AlignHCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: -5
                        font.family: "Open Sans"
                        font.weight: Font.Normal
                        font.pixelSize: 27
                        lineHeight: 0.8
                    }
                }

            }

            DropArea {
                anchors { fill: parent; margins: 10 }

                onEntered: {
                    visualModel.items.move(drag.source.DelegateModel.itemsIndex,dragArea.DelegateModel.itemsIndex)
                    menuConfig.move(drag.source.DelegateModel.itemsIndex, dragArea.DelegateModel.itemsIndex, 1)
                }
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN NAVIGATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias mainNavigationListView: mainNavigationListView

    ListView {
        id: mainNavigationListView

        width: parent.width-20
        height: 50
        anchors.centerIn: parent
        clip: true
        boundsBehavior: Flickable.DragAndOvershootBounds
        flickableDirection: Flickable.HorizontalFlick
        orientation: ListView.Horizontal
        interactive: true
        focus: true
        highlightMoveDuration: 200

        currentIndex: 0

        spacing: 4

        model: visualModel
        delegate: dragDelegate
    }

    DelegateModel {
        id: visualModel

        model: menuConfig
        delegate: dragDelegate
    }
}
