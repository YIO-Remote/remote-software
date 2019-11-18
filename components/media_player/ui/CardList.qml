import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: mediaList
    width: parent.width
    height: parent.height
    color: colorDark

    property var    browseResult:       obj.browseResult                        // structure of items, playCommands, type, title, level
                                                                                // items : array of structure (item_key, title, sub_title, image_url
    property var    navCommands:        ["TOP", "BACK", "GOTO"]
    property bool   showNavCommands:    false;                                  // could be configurable
    property int    selectedItem:       0
    property string selectedItemKey:    ""
    property int    selectedCommand:    0;
    property string selectedCommandKey: ""
    property string selectedSearch:     ""
    property bool   showCommands:       false
    property bool   showSearch:         false

    onBrowseResultChanged: {
        var i;
        // update items
        listItems.clear();
        for (i = 0; i < browseResult.items.length; i++) {
            var item = browseResult.items[i];
            if (item.image_url === "")
                item.image_url = "qrc:/images/mini-music-player/no_image.png";
            listItems.append(item);
        }
        selectItem (0);

        // update play commands
        selectedCommand = 0;
        commandItems.clear();
        if (showNavCommands) {
            if (browseResult.level > 0) {
                commandItems.append({command : "TOP" });
                commandItems.append({command : "BACK"});
            }
            selectedCommand = commandItems.count;
            if (browseResult.type !== "Album" && browseResult.type !== "Playlist") {
                commandItems.append({command : "GOTO"});
            }
        }
        for (i = 0; i < browseResult.playCommands.length; i++) {
            commandItems.append({command : browseResult.playCommands[i]});
        }
        selectedCommandKey = commandItems.count > selectedCommand ? commandItems.get(selectedCommand).command : "";
    }

    function performCommand (command) {
        if (navCommands.indexOf (command) >= 0) {
            if (command === "GOTO") {
                if (selectedSearch != "") {
                    var searchText = searchSection.children[1].text;
                    if (showSearch && searchText !== "") {
                        obj.search(searchText, selectedItemKey);
                    }
                    else {
                        showSearch = true;
                        return;
                    }
                }
                else
                    obj.browse(selectedItemKey);
            }
            else
                obj.browse(command)
        }
        else
            obj.playMedia(command, selectedItemKey);
        showSearch = false;
    }
    function selectItem (index) {
        if (index < 0)
            index = 0;
        if (index >= browseResult.items.length)
            index = browseResult.items.length - 1;

        if (index >= 0) {
            selectedItem = index;
            var item = browseResult.items[selectedItem];
            selectedItemKey = item.item_key;
            if (item.input_prompt)
                selectedSearch = item.input_prompt;
            else {
                showSearch = false;
                selectedSearch = "";
            }
            listView.currentIndex = selectedItem;
        }
        else {
            selectedItem = 0;
            selectedItemKey = "";
            selectedSearch = "";
        }

    }

    /////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS (with long press detection)
    /////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: buttonHandler
        enabled: true

        onButtonPress: {
            if (standbyControl.mode === "on" || standbyControl.mode === "dim") {
                switch (button) {
                    case "dpad up":
                        if (showCommands) {
                            if (selectedCommand > 0) {
                                selectedCommand--;
                                selectedCommandKey = commandItems.get(selectedCommand).command;
                            }
                            return;
                        }
                        selectItem(selectedItem - 1);
                        break;
                    case "dpad down":
                        if (showCommands) {
                            if (selectedCommand < commandItems.count - 1) {
                                selectedCommand++;
                                selectedCommandKey = commandItems.get(selectedCommand).command;
                            }
                            return;
                        }
                        selectItem(selectedItem + 1);
                        break;
                    case "dpad middle":
                        if (showCommands) {
                            performCommand(selectedCommandKey);
                        }
                        else {
                            if (commandItems.count > 0)
                                showCommands = true;
                            return;
                        }
                        break;
                    case "dpad right":
                        performCommand("GOTO");
                        showCommands = false;
                        return;
                    case "dpad left":
                        if (!showCommands)
                            obj.browse("BACK");
                        break;
                    case "top left":
                        obj.browse("TOP");
                        break;
                    case "top right":
                        selectItem(selectedItem - 8);
                        break;
                    case "bottom right":
                        selectItem(selectedItem + 8);
                        break;
                    case "volume up":
                        obj.volumeUp();
                        break;
                    case "volume down":
                        obj.volumeDown();
                        break;
                }
                showCommands = false;
                showSearch = false;
            }
        }
        onButtonRelease: {
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // Catch backround click
    /////////////////////////////////////////////////////////////////////////////////////
    MouseArea {
        anchors.fill: parent

        onClicked: {
            haptic.playEffect("click");
            //obj.browse(model.item_key);
            showCommands = false;
            showSearch = false;
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////
    // Header type title
    /////////////////////////////////////////////////////////////////////////////////////
    Item {
        id: textItem;
        width: parent.width
        height: 40
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 20

        Text {
            id: type
            color: colorText
            text: browseResult.type
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            width: 120
            anchors.left: parent.left
            anchors.leftMargin: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

        Text {
            id: title
            color: colorText
            text: browseResult.title
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            width: 250
            anchors.right: parent.right
            anchors.rightMargin: 40
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 27
            lineHeight: 1
        }

    }

    /////////////////////////////////////////////////////////////////////////////////////
    // Items List
    /////////////////////////////////////////////////////////////////////////////////////
    ListModel {
        id: listItems
        /*
        ListElement {
            key: "key"
            title: "title"
            sub_title: "subtitle"
            image_url: "image_url"
        }
        */
    }
    Component {
        id: listDelegate
        Item {
            width: parent.width
            height: 64
            Rectangle {
                id: rect
                width: parent.width
                height: parent.height
                color: model.item_key === selectedItemKey ? colorHighlight1 : colorMedium
                visible: true

                Text {
                    anchors { left: parent.left; top:parent.top; topMargin: 0; leftMargin: 10 }
                    id: title
                    color: colorText
                    text: model.title
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 24
                }
                Text {
                    anchors { left: parent.left; top: title.bottom; leftMargin: 10 }
                    color: colorText
                    text: model.sub_title
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 24
                }
                Image {
                    asynchronous: true
                    width: 64
                    height: 64
                    fillMode: Image.PreserveAspectFit
                    anchors { right: parent.right; rightMargin: 10 }
                    visible: !model.image_url.startsWith ('qrc');
                    source: model.image_url;
                }
            }
            MouseArea {
                anchors.fill: parent

                onClicked: {
                    haptic.playEffect("click");
                    //obj.browse(model.item_key);
                    selectItem(model.index);
                    if (showCommands)
                        showCommands = false;
                    else if (commandItems.count > 0)
                        showCommands = true;
                }
            }
        }
    }
    ListView {
        id: listView
        width: parent.width
        height: parent.height - textItem.bottom - 30
        spacing: 10
        orientation: ListView.Vertical
        flickableDirection: Flickable.VerticalFlick
        anchors {
            left: parent.left; top: textItem.bottom;
            right: parent.right; bottom: parent.bottom;
            margins: 10
        }
        clip: true
        model: listItems
        delegate: listDelegate
    }
    /////////////////////////////////////////////////////////////////////////////////////
    // Commands List
    /////////////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: commandSection
        width: parent.width - 250
        height: commandItems.count * 70 + 20;
        radius: cornerRadius
        color: colorBackground
        visible : showCommands
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        clip: true

        ListModel {
            id: commandItems
            /*
            ListElement {
                command: "key"
            }
            */
        }
        Component {
            id: commandDelegate
            Item {
                width: parent.width
                height: 50
                Rectangle {
                    id: rect
                    width: parent.width
                    height: parent.height
                    radius: cornerRadius;
                    color:  model.command === selectedCommandKey ? colorHighlight1 : colorMedium;
                    visible: true

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        id: title
                        color: colorText
                        text: model.command
                        font.family: "Open Sans"
                        font.weight: Font.Normal
                        font.pixelSize: 24
                    }
                }
                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        haptic.playEffect("click");
                        performCommand(model.command);
                        showCommands = false;
                    }
                }
            }
        }
        ListView {
            id: commandView
            visible: showCommands
            width: parent.width
            height: parent.height
            spacing: 20
            anchors.fill: parent
            anchors.margins: 20
            orientation: ListView.Vertical
            model: commandItems
            delegate: commandDelegate
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // Search
    /////////////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: searchSection
        width: parent.width - 100
        height: 200;
        radius: cornerRadius
        color: colorBackground
        visible : showSearch
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            id: textSearch
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 10
            color: colorText
            text: "Enter your search !"
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 24
        }
        TextField {
            id: textfieldSearch
            width: parent.width - 40
            anchors.left: parent.left
            anchors.top: textSearch.bottom
            anchors.margins: 20
            font.family: "Open Sans"
            font.weight: Font.Normal
            font.pixelSize: 28
            color: "#000000"
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhPreferLowercase | Qt.ImhNoPredictiveText
            background: Rectangle {
                implicitWidth: parent.width
                implicitHeight: 50
                color: colorHighlight1
            }
        }
        Rectangle {
            id: rectSearch
            anchors.left: parent.left
            anchors.top: textfieldSearch.bottom
            anchors.margins: 20
            width: parent.width - 40
            height: 50
            radius: cornerRadius;
            color:  colorHighlight1;

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                color: colorText
                text: "Start Search"
                font.family: "Open Sans"
                font.weight: Font.Normal
                font.pixelSize: 24
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    var x;
                    x = parent.anchors;
                    x = textSearch.anchors.top;
                    x = textfieldSearch.anchors.top;
                    x = rectSearch.anchors.top;
                    haptic.playEffect("click");
                    performCommand("GOTO");
                }
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////
    // Close Button
    /////////////////////////////////////////////////////////////////////////////////////
    Image {
        id: closeButton
        asynchronous: true
        width: 30
        height: 30
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/components/close_button.png"
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 20

        ColorOverlay {
            visible: !darkMode
            anchors.fill: parent
            source: parent
            color: colorText
            antialiasing: true
        }

        MouseArea {
            width: parent.width + 60
            height: parent.height + 60
            anchors.centerIn: parent

            onClicked: {
                haptic.playEffect("click");
                mediaPlayer.listOpen = false;
            }
        }
    }
}
