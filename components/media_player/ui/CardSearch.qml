import QtQuick 2.12
import QtQuick.Controls 2.12

import "qrc:/basic_ui" as BasicUI

Flickable {
    id: itemFlickable
    width: parent.width
    height: parent.height
    maximumFlickVelocity: 6000
    flickDeceleration: 1000
    contentHeight: 100 + searchContainer.height + recentSearches.height + searchResults.height
    boundsBehavior: Flickable.DragAndOvershootBounds
    flickableDirection: Flickable.VerticalFlick

    Behavior on contentY {
        PropertyAnimation {
            duration: 300
            easing.type: Easing.OutExpo
        }
    }

    ScrollBar.vertical: ScrollBar {
        opacity: 0.5
    }

    Connections {
        target: card

        onLooseFocus: {
            searchTextField.focus = false;
        }
    }

    function onSearch() {
        obj.search(searchTextField.text);
        searchResultsTitle.text = searchTextField.text;
        recentSearches.state = "hidden";
        recentSearchesModel.insert(0, {"searchString":searchTextField.text});
        searchTextField.focus = false;
        itemFlickable.contentY = 230;
        searchTextField.text = "";
    }

    Rectangle {
        id: searchContainer
        width: parent.width - 60
        height: 80
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.horizontalCenter: parent.horizontalCenter
        color: colorText
        radius: cornerRadius

        Text {
            id: searchIcon
            color: colorBackground
            text: "\uE90C"
            renderType: Text.NativeRendering
            width: 70
            height: 70
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    haptic.playEffect("click");
                    onSearch();
                }
            }
        }

        TextField {
            id: searchTextField
            cursorVisible: false
            width: parent.width-90
            height: parent.height
            anchors.left: searchIcon.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            placeholderText: qsTr("Search") + translateHandler.emptyString
            color: colorBackground
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27

            background: Rectangle {
                color: colorBackgroundTransparent
                border.width: 0
            }

            onAccepted: {
                onSearch();
            }

            onFocusChanged: {
                if (focus) {
                    inputPanel.active = true
                    recentSearches.state = "visible";
                    itemFlickable.contentY = 0;
                } else
                    inputPanel.active = false
            }
        }
    }

    ListModel {
        id: recentSearchesModel

        onCountChanged: {
            if (count == 0)
                recentSearches.state = "hidden";
        }
    }

    Item {
        id: recentSearches
        width: parent.width - 60
        height: childrenRect.height
        anchors.top: searchContainer.bottom
        anchors.topMargin: 40
        anchors.horizontalCenter: parent.horizontalCenter
        state: "visible"

        states:[
            State {
                name: "hidden"
                PropertyChanges { target: recentSearches; opacity: 0; height: 0; visible: false }
            },
            State {
                name: "visible"
                PropertyChanges { target: recentSearches; opacity: 1; height: childrenRect.height; visible: true }
            }
        ]

        transitions: [
            Transition {
                to: "hidden"
                SequentialAnimation {
                    PropertyAnimation { target: recentSearches; properties: "opacity, height"; easing.type: Easing.OutExpo; duration: 300 }
                    PropertyAnimation { target: recentSearches; properties: "visible"; duration: 1 }
                }
            },
            Transition {
                to: "visible"
                SequentialAnimation {
                    PropertyAnimation { target: recentSearches; properties: "visible"; duration: 1 }
                    PropertyAnimation { target: recentSearches; properties: "opacity, height"; easing.type: Easing.OutExpo; duration: 300 }
                }
            }
        ]

        Text {
            id: recentSearchesTitle
            color: colorText
            text: qsTr("Recent searches") + translateHandler.emptyString
            anchors.left: parent.left
            anchors.top: parent.top
            font.family: "Open Sans "
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            color: colorText
            opacity: 0.5
            text: qsTr("Clear") + translateHandler.emptyString
            anchors.right: parent.right
            anchors.top: parent.top
            font.family: "Open Sans"
            font.pixelSize: 27
            lineHeight: 1

            visible: recentSearchesModel.count > 0 ? true : false
            enabled: visible

            MouseArea {
                width: parent.width+20
                height: parent.height+20

                onClicked: {
                    haptic.playEffect("click");
                    recentSearchesModel.clear();
                }
            }
        }

        ListView {
            model: recentSearchesModel
            anchors.top: recentSearchesTitle.bottom
            anchors.topMargin: 40
            height: childrenRect.height
            interactive: false

            delegate: Item {
                width: recentSearches.width
                height: 60

                Text {
                    color: colorText
                    text: searchString
                    font.family: "Open Sans"
                    font.pixelSize: 25
                    lineHeight: 1
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        haptic.playEffect("click");
                        obj.search(searchString);
                        searchResultsTitle.text = searchString;
                        recentSearches.state = "hidden";
                        itemFlickable.contentY = 230;
                    }
                }

            }

            add: Transition {
                PropertyAnimation { properties: "opacity"; from: 0; to: 1; duration: 400; easing.type: Easing.OutExpo }
            }

            displaced: Transition {
                PropertyAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBounce }
            }
        }
    }

    Item {
        id: searchResults
        width: parent.width-60
        height: 200
        anchors.top: recentSearches.bottom
        anchors.topMargin: 40
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            id: searchResultsTitle
            color: colorText
            anchors.left: parent.left
            anchors.top: parent.top
            font.family: "Open Sans "
            font.pixelSize: 30
            lineHeight: 1
        }

        Flow {
            id: tags
            width: parent.width

        }

    }
}
