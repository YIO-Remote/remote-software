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


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MENU CONFIGURATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property alias menuConfig: menuConfig

    ListModel {
        id: menuConfig
    }

    //: menu items that are in the bottom menu
    property var menuTranslations: [qsTr("Favorites") + translateHandler.emptyString, qsTr("Settings") + translateHandler.emptyString,
                qsTr("Lights") + translateHandler.emptyString, qsTr("Blinds") + translateHandler.emptyString
    ]


    function loadmenuConfig() {
        // clear the menuConfig
        menuConfig.clear();

        // if the default config is in the menu
        if (config.read.settings.menu.order.length < 3) {
            for (var i=0; i<config.read.settings.menu.order.length; i++) {
                config.read.settings.menu.order[i].display_name = qsTr(config.read.settings.menu.order[i].display_name) + translateHandler.emptyString;
                menuConfig.append(config.read.settings.menu.order[i]);
            }
            addDeviceTypes();
            addAreas();
            menuConfig.move(1, menuConfig.count-1, 1);
        } else {
            for (var i=0; i<config.read.settings.menu.order.length; i++) {
                config.read.settings.menu.order[i].display_name = qsTr(config.read.settings.menu.order[i].display_name) + translateHandler.emptyString;
                menuConfig.append(config.read.settings.menu.order[i]);
            }
        }
    }

    function savemenuConfig() {
        // clear the list
        config.read.settings.menu.order = [];
        // get the data from the listmodel
        for (var i=0; i<menuConfig.count; i++) {
            config.read.settings.menu.order.push({"name": menuConfig.get(i).name, "display_name": menuConfig.get(i).display_name, "show": menuConfig.get(i).show});
        }

        // write to json file
        jsonconfig.write(config);
    }

    function addDeviceTypes() {
        for (var i=0; i<entities.loaded_entities.length; i++) {
            menuConfig.append({"name": entities.loaded_entities[i], "display_name": entities.getSupportedEntityTranslation(entities.loaded_entities[i]),"show": true});
        }
    }

    function addAreas() {
        for (var i=0; i<config.read.areas.length; i++) {
            menuConfig.append({"name": "area", "display_name": config.read.areas[i].area,"show": true});
        }
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

            width: content.width
            height: content.height

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
                        text: qsTr(display_name) + translateHandler.emptyString
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
    }

    DelegateModel {
        id: visualModel

        model: menuConfig
        delegate: dragDelegate
    }

    Rectangle { //left gradient fade
        width: parent.height
        height: parent.height
        anchors.left: parent.left
        anchors.verticalCenter: mainNavigation.verticalCenter
        rotation: 90

        opacity: mainNavigationListView.atXBeginning ? 0 : 1

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

        opacity: mainNavigationListView.atXEnd ? 0 : 1

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
