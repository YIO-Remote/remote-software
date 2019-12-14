import QtQuick 2.12
import QtQuick.Controls 2.12

import "qrc:/basic_ui" as BasicUI

Rectangle {
    width: parent.width
    height: parent.height
    color: colorDark

    function onSearch() {
        obj.search(searchTextField.text);
        searchResultsTitle.text = searchTextField.text;
        recentSearches.state = "hidden";
        obj.recentSearches.insert(0, {"searchString":searchTextField.text});
        searchTextField.focus = false;
        itemFlickable.contentY = 230;
        searchTextField.text = "";
    }

    function load(album, type) {
        swipeView.currentIndex++;
        if (type === "album") {
            obj.getAlbum(album);
            obj.browseModelChanged.connect(onBrowseModelChanged);
        } else if (type == "playlist") {
            obj.getPlaylist(album);
            obj.browseModelChanged.connect(onBrowseModelChanged);
        }
    }

    function onBrowseModelChanged(model) {
        if (model.count != 0 && albumLoader) {
            if (albumLoader.source != "qrc:/components/media_player/ui/AlbumView.qml")
                albumLoader.setSource("qrc:/components/media_player/ui/AlbumView.qml", { "albumModel": model })
            else if (albumLoader.item)
                albumLoader.item.albumModel = model;
        }
        obj.browseModelChanged.disconnect(onBrowseModelChanged);
    }

    property alias albumLoader: albumLoader
    property alias swipeView: swipeView

    SwipeView {
        id: swipeView
        width: parent.width
        height: parent.height
        currentIndex: 0
        interactive: false
        clip: true

        Item {

            Flickable {
                id: itemFlickable
                width: parent.width
                height: parent.height-100
                maximumFlickVelocity: 6000
                flickDeceleration: 1000
                contentHeight: 260 + searchContainer.height + recentSearches.height + searchResults.height
                boundsBehavior: Flickable.DragAndOvershootBounds
                flickableDirection: Flickable.VerticalFlick
                clip: true

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

                Connections {
                    target: obj.recentSearches

                    onCountChanged: {
                        if (obj.recentSearches.count == 0) {
                            recentSearches.state = "hidden";
                        }
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

                        visible: obj.recentSearches.count > 0 ? true : false
                        enabled: visible

                        MouseArea {
                            width: parent.width+20
                            height: parent.height+20

                            onClicked: {
                                haptic.playEffect("click");
                                obj.recentSearches.clear();
                            }
                        }
                    }

                    ListView {
                        model: obj.recentSearches
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
                        visible: searchResultsList.count != 0 ? true : false
                        spacing: 20

                        property var modelHeights: ({})

                        BasicUI.Tag {
                            id: tagAll
                            tag: "All"
                            selected: {
                                var selectedItem = true;
                                for (var i=0; i<tagRepeater.count; i++) {
                                    if (!tagRepeater.itemAt(i).selected) {
                                        selectedItem = false;
                                    }
                                }
                                return selectedItem;
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (tagAll.selected) {
                                        for (var i=0; i<tagRepeater.count; i++) {
                                            tagRepeater.itemAt(i).selected = false;
                                        }
                                    } else {
                                        for (var i=0; i<tagRepeater.count; i++) {
                                            tagRepeater.itemAt(i).selected = true;
                                        }
                                    }
                                }
                            }
                        }

                        Repeater {
                            id: tagRepeater
                            model: obj.searchModel

                            BasicUI.Tag {
                                id: tagItem
                                tag: {
                                    if (title == "albums")
                                        return qsTr("Albums") + translateHandler.emptyString
                                    else if (title == "tracks")
                                        return qsTr("Tracks") + translateHandler.emptyString
                                    else if (title == "artists")
                                        return qsTr("Artists") + translateHandler.emptyString
                                    else if (title == "playlists")
                                        return qsTr("Playlists") + translateHandler.emptyString
                                }
                                selected: true

                                onSelectedChanged: {
                                    if (!selected) {
                                        searchResultsList.currentIndex = index;

                                        tags.modelHeights[index] = searchResultsList.currentItem.height;

                                        searchResultsList.currentItem.visible = false;
                                        searchResultsList.currentItem.height = 0 - searchResultsList.spacing;
                                    } else {
                                        searchResultsList.currentIndex = index;

                                        searchResultsList.currentItem.height = tags.modelHeights[index];
                                        searchResultsList.currentItem.visible = true;
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        if (selected) {
                                            selected = false;
                                        } else {
                                            selected = true;
                                        }
                                    }
                                }
                            }
                        }

                    }

                    ListView {
                        id: searchResultsList
                        width: parent.width
                        height: childrenRect.height
                        anchors.top: tags.bottom
                        anchors.topMargin: 40
                        interactive: false
                        spacing: 40

                        model: obj.searchModel

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
                                font.family: "Open Sans"
                                font.pixelSize: 40
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

                                onStatusChanged: if (listViewLoader.status == Loader.Ready && listViewLoader.item) {
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

                        delegate: albumThumbnail
                    }
                }

                Component {
                    id: albumThumbnail

                    Item {
                        width: childrenRect.width
                        height: childrenRect.height

                        Rectangle {
                            id: albumImage
                            width: 160
                            height: 160
                            color: colorBackground

                            Image {
                                source: item_image
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                            }
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

                        MouseArea {
                            anchors.fill: parent

                            onClicked: {
                                haptic.playEffect("click");
                                load(item_key, item_type);
                            }
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
                        id: trackThumbnailItem
                        width: parent.width
                        height: 80 //childrenRect.height

                        Rectangle {
                            id: albumImage
                            width: 80
                            height: 80

                            Image {
                                source: item_image
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectCrop
                                asynchronous: true
                            }
                        }

                        Text {
                            id: albumTitleText
                            text: item_title
                            elide: Text.ElideRight
                            width: itemFlickable.width-60-albumImage.width-20-60
                            wrapMode: Text.NoWrap
                            color: colorText
                            anchors.left: albumImage.right
                            anchors.leftMargin: 20
                            anchors.top: albumImage.top
                            anchors.topMargin: item_subtitle == "" ? 26 : 12
                            font.family: "Open Sans"
                            font.pixelSize: 25
                            lineHeight: 1
                        }

                        Text {
                            id: albumSubTitleText
                            text: item_subtitle
                            elide: Text.ElideRight
                            visible: item_subtitle == "" ? false : true
                            width: albumTitleText.width
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

                        MouseArea {
                            anchors.fill: parent

                            onClicked: {
                                haptic.playEffect("click");
                                obj.playMedia(item_key, item_type);
                            }
                        }

                        BasicUI.ContextMenuIcon {
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter

                            mouseArea.onClicked: {
                                haptic.playEffect("click");
                                contextMenuLoader.setSource("qrc:/basic_ui/ContextMenu.qml", { "width": itemFlickable.width, "id": item_key, "type": item_type, "list": item_commands })
                            }
                        }
                    }
                }
            }
        }

        Item {

            Loader {
                id: albumLoader
                asynchronous: true
                anchors.fill: parent
            }

            Text {
                id: backButton
                color: colorText
                text: "\uE917"
                renderType: Text.NativeRendering
                width: 70
                height: 70
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font {family: "icons"; pixelSize: 80 }
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 20

                MouseArea {
                    id: backButtonMouseArea
                    width: parent.width + 20
                    height: parent.height + 20
                    anchors.centerIn: parent

                    onClicked: {
                        haptic.playEffect("click");
                        swipeView.currentIndex = 0;
                    }
                }
            }

        }
    }
}
