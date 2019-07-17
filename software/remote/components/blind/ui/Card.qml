import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: blindAdjust
    width: parent.width
    height: parent.height
    color: colorDark

    // blind graphics draggable element

    property var percent

    Component.onCompleted: {
        percent = obj.position
    }

    Item {
        width: parent.width
        height: parent.height
        anchors.centerIn: parent

        Column {
            anchors.top: parent.top

            spacing: 10

            Repeater {
                id: repeater
                model: dragger.pressed ? Math.round(percent*36/100) : Math.round(obj.position*36/100)

                delegate: Rectangle {
                    width: blindAdjust.width
                    height: 10
                    color: colorHighlight2
                }
            }
        }

        Rectangle {
            id: dragger_target
            width: parent.width
            height: parent.height
            color: colorBackgroundTransparent
            y: dragger.pressed ? Math.round(height*percent/100) : Math.round(obj.position/100)
        }

        MouseArea {
            id: dragger
            anchors.fill: parent
            drag.target: dragger_target
            drag.axis: Drag.YAxis
            drag.minimumY: 0
            drag.maximumY: dragger_target.height

            onPositionChanged: {
                percent = Math.round(mouse.y/parent.height*100)
                if (percent < 0) percent = 0
                if (percent > 100) percent = 100
            }

            onReleased: {
                obj.setPosition(percent);
            }
        }
    }

    Image {
        id: icon
        asynchronous: true
        width: 85
        height: 85
        fillMode: Image.PreserveAspectFit
        source: "qrc:/components/blind/images/icon-blind.png"
        anchors {top: parent.top; topMargin: 30; left: parent.left; leftMargin: 30}

        ColorOverlay {
            visible: !darkMode
            anchors.fill: parent
            source: parent
            color: colorText
            antialiasing: true
        }
    }

    Text {
        id: percentage
        color: colorText
        text: percent
        horizontalAlignment: Text.AlignLeft
        anchors { top: parent.top; topMargin: 100; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Light"; pixelSize: 180 }
    }

    Text {
        color: colorText
        text: "%"
        anchors { left: percentage.right; bottom: percentage.bottom; bottomMargin: 30 }
        font {family: "Open Sans Light"; pixelSize: 100 }
    }

    Text {
        id: title
        color: colorText
        text: obj.friendly_name
        wrapMode: Text.WordWrap
        width: parent.width-60
        anchors { top: percentage.bottom; topMargin: -40; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans SemiBold"; pixelSize: 60 }
        lineHeight: 0.9
    }

    Text {
        id: areaText
        color: colorText
        opacity: 0.5
        text: obj.area
        elide: Text.ElideRight
        wrapMode: Text.NoWrap
        width: parent.width-60
        anchors { top: title.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
        font {family: "Open Sans Regular"; pixelSize: 24 }
    }

    function getFeature(feature) {
        if (obj.supported_features.indexOf(feature) > -1) {
            return true;
        } else {
            return false;
        }
    }

    BasicUI.CustomButton {
        anchors { left: parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
        color: colorText
        buttonText: "   "
        visible: getFeature("CLOSE");

        mouseArea.onClicked: {
            haptic.playEffect("click");
            obj.close()
        }
    }

    BasicUI.CustomButton {
        anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom; bottomMargin: 70 }
        color: colorText
        buttonText: "   "
        visible: getFeature("STOP");
        mouseArea.onClicked: {
            haptic.playEffect("click");
            obj.stop()
        }
    }

    BasicUI.CustomButton {
        anchors { right: parent.right; rightMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
        color: colorText
        buttonText: "   "
        visible: getFeature("OPEN");

        mouseArea.onClicked: {
            haptic.playEffect("click");
            obj.open()
        }
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
                blindButton.state = "closed"
                loader_main.state = "visible"
                var tmp = mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY;
                mainNavigationSwipeview.currentItem.mainNavigationLoader.active = false;
                mainNavigationSwipeview.currentItem.mainNavigationLoader.active = true;
                mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY = tmp;
            }
        }
    }
}
