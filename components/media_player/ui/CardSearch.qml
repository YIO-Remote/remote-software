import QtQuick 2.12
import QtQuick.Controls 2.12

import "qrc:/basic_ui" as BasicUI

Flickable {
    id: itemFlickable
    width: parent.width
    height: parent.height-60
    maximumFlickVelocity: 6000
    flickDeceleration: 1000
    contentHeight: 280 + searchContainer.height + recentSearches.height + searchResults.height
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
        height: childrenRect.height
        anchors.top: recentSearches.bottom
        anchors.topMargin: 40
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            id: searchResultsTitle
            color: colorText
            anchors.left: parent.left
            anchors.top: parent.top
            font.family: "Open Sans Bold"
            font.pixelSize: 30
            lineHeight: 1
        }

        Flow {
            id: tags
            width: parent.width
            anchors.top: searchResultsTitle.bottom
            anchors.topMargin: 30

        }

        ListView {
            id: searchResultsList
            width: parent.width
            height: childrenRect.height
            anchors.top: tags.bottom
            anchors.topMargin: 40
            interactive: false
            spacing: 40

            model: obj.model

            delegate:
                Item {
                width: parent.width
                height: childrenRect.height

                // Title
                Text {
                    id: searchResultsListHeader
                    text: {
                        if (title == "albums")
                            return qsTr("Albums") + translateHandler.emptyString
                        else if (title == "tracks")
                            return qsTr("Tracks") + translateHandler.emptyString
                        else if (title == "artists")
                            return qsTr("Artists") + translateHandler.emptyString
                        else if (title == "playlists")
                            return qsTr("Playlists") + translateHandler.emptyString
                    }
                    color: colorText
                    anchors.left: parent.left
                    anchors.top: parent.top
                    font.family: "Open Sans Bold"
                    font.pixelSize: 27
                    lineHeight: 1
                }

                // Listview
                Loader {
                    id: listViewLoader
                    anchors.top: searchResultsListHeader.bottom
                    anchors.topMargin: 40
                    anchors.horizontalCenter: parent.horizontalCenter
                    sourceComponent: {
                        if (title == "albums")
                            return albumList
                        else if (title == "tracks")
                            return trackList
                        else if (title == "artists")
                            return trackList
                        else if (title == "playlists")
                            return albumList
                    }

                    Component.onCompleted: {
                        item.model = item_data;
                    }
                }
            }
        }
    }

    Component {
        id: albumList

        ListView {
            id: albumListView
            width: itemFlickable.width-60
            height: 260
            orientation: ListView.Horizontal
            spacing: 30
            clip: true
            cacheBuffer: 4000

            delegate: albumThumbnail
        }
    }

    Component {
        id: albumThumbnail

        Item {
            width: childrenRect.width
            height: childrenRect.heigh

            Image {
                id: albumImage
                source: item_image
                width: 160
                height: 160
                fillMode: Image.PreserveAspectCrop
                asynchronous: true
            }

            Text {
                id: albumTitleText
                text: item_title
                elide: Text.ElideRight
                width: albumImage.width
                wrapMode: Text.NoWrap
                color: colorText
                anchors.left: parent.left
                anchors.top: albumImage.bottom
                anchors.topMargin: 20
                font.family: "Open Sans"
                font.pixelSize: 25
                lineHeight: 1
            }

            Text {
                id: albumSubTitleText
                text: item_subtitle
                elide: Text.ElideRight
                width: albumImage.width
                wrapMode: Text.NoWrap
                color: colorText
                opacity: 0.6
                anchors.left: parent.left
                anchors.top: albumTitleText.bottom
                anchors.topMargin: 5
                font.family: "Open Sans"
                font.pixelSize: 20
                lineHeight: 1
            }
        }
    }

    Component {
        id: trackList

        ListView {
            id: trackListView
            width: itemFlickable.width-60
            height: childrenRect.height
            spacing: 20
            interactive: false

            delegate: trackThumbnail
        }
    }

    Component {
        id: trackThumbnail

        Item {
            width: childrenRect.width
            height: childrenRect.height

            Image {
                id: albumImage
                source: item_image
                width: 80
                height: 80
                fillMode: Image.PreserveAspectCrop
                asynchronous: true
            }

            Text {
                id: albumTitleText
                text: item_title
                elide: Text.ElideRight
                width: itemFlickable.width-60-albumImage.width-20
                wrapMode: Text.NoWrap
                color: colorText
                anchors.left: albumImage.right
                anchors.leftMargin: 20
                anchors.top: albumImage.top
                anchors.topMargin: item_subtitle == "" ? 28 : 14
                font.family: "Open Sans"
                font.pixelSize: 25
                lineHeight: 1
            }

            Text {
                id: albumSubTitleText
                text: item_subtitle
                elide: Text.ElideRight
                width: itemFlickable.width-60-albumImage.width-20
                wrapMode: Text.NoWrap
                color: colorText
                opacity: 0.6
                anchors.left: albumTitleText.left
                anchors.top: albumTitleText.bottom
                anchors.topMargin: 5
                font.family: "Open Sans"
                font.pixelSize: 20
                lineHeight: 1
            }
        }
    }
}
