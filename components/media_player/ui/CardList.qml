import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: mediaList
    width: parent.width
    height: parent.height
    color: colorDark

    property var items: obj.browseItems;
    property var buttons: obj.browseCmds;

    BasicUI.CustomButton {
        id: buttonLeft
        anchors { left: parent.left; top: parent.top; }
        color: colorText
        buttonText: buttons[0];
        buttonTextColor: colorBackground
        visible: buttons[0].length > 0;
        mouseArea.onClicked: {
            haptic.playEffect("click");
            obj.browse(buttons[0]);
        }
    }
    onItemsChanged: {
        listItems.clear();
        for (var i = 0; i < items.length; i++) {
            if (items[i].image_url === "")
                items[i].image_url = "qrc:/images/mini-music-player/no_image.png";
            listItems.append(items[i]);
        }
    }

    BasicUI.CustomButton {
        id: buttonMiddle
        anchors { left: buttonLeft.right; leftMargin: 10; top: parent.top;  }
        color: colorText
        buttonText: buttons[1];
        buttonTextColor: colorBackground
        visible: buttons[1].length > 0;
        mouseArea.onClicked: {
            haptic.playEffect("click");
            obj.browse(buttons[1]);
        }
    }
    BasicUI.CustomButton {
        id: buttonRight
        anchors { left: buttonMiddle.right; leftMargin: 10; top: parent.top; }
        color: colorText
        buttonText: buttons[2];
        buttonTextColor: colorBackground
        visible: buttons[2].length > 0;

        mouseArea.onClicked: {
            haptic.playEffect("click");
            obj.browse(buttons[2]);
        }
    }


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
                color: colorMedium
                visible: true

                Text {
                    anchors { topMargin: 20; leftMargin: 20 }
                    id: title
                    color: colorText
                    text: model.title
                    font.pixelSize: 24
                }
                Text {
                    anchors { top : title.bottom; leftMargin: 20 }
                    color: colorText
                    text: model.sub_title
                    font.pixelSize: 24
                }
                Image {
                    asynchronous: true
                    width: 64
                    height: 64
                    fillMode: Image.PreserveAspectFit
                    anchors { right: rect.right; rightMargin: 20 }
                    visible: !model.image_url.startsWith ('qrc');
                    source: model.image_url
                }
            }
            MouseArea {
                anchors.fill: parent

                onClicked: {
                    haptic.playEffect("click");
                    obj.browse(model.item_key);
                }
            }
        }
    }
    ListView {
        id: listView
        width: parent.width
        spacing: 20
        orientation: ListView.Vertical
        flickableDirection: Flickable.VerticalFlick
        anchors {
            left: parent.left; top: buttonLeft.bottom;
            right: parent.right; bottom: parent.bottom;
            margins: 20
        }
        model: listItems
        delegate: listDelegate
    }
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
