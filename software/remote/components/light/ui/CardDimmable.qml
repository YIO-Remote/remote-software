import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: cardDimmable
    width: parent.width
    height: parent.height
    color: colorMedium

    property int brightness: obj.attributes.brightness

    signal updateBrightness()

    MouseArea {
        id: dragger
        anchors.fill: parent
        drag.target: draggerTarget
        drag.axis: Drag.YAxis
        drag.minimumY: 0
        drag.maximumY: parent.height-10

        property int percent

        onPositionChanged: {
            haptic.playEffect("bump");
            dragger.percent = Math.round((parent.height - 10 - mouse.y)/(parent.height-10)*100);
            if (dragger.percent < 0) dragger.percent = 0;
            if (dragger.percent > 100) dragger.percent = 100;
            if (dragger.percent > brightness) {
                percentageBG2.height = parent.height*dragger.percent/100
            } else {
                percentageBG.height = parent.height*dragger.percent/100
            }
            percentage.text = dragger.percent;
        }

        onReleased: {
            integration[obj.integration].obj.light.setBrightness(obj.entity_id, dragger.percent);
        }
    }

    Connections {
        target: cardDimmable

        onUpdateBrightness: {
            percentageBG.height = parent.height*obj.attributes.brightness/100;
            percentageBG2.height = parent.height*obj.attributes.brightness/100;
            percentage.text = obj.attributes.brightness;
        }
    }

    onBrightnessChanged: {
        updateBrightness()
    }

    Rectangle {
        id: draggerTarget
        width: parent.width
        height: 30
        color: "#00000000"
        y: parent.height - percentageBG.height
    }

    Rectangle {
        id: percentageBG2
        color: colorSwitch
        width: parent.width
        height: 0
        radius: cornerRadius
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }

        Behavior on height {
            PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
        }
    }

    Rectangle {
        id: percentageBG
        color: colorHighlight
        width: parent.width
        height: parent.height*obj.attributes.brightness/100
        radius: cornerRadius
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }

        Behavior on height {
            PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
        }
    }

    Image {
        id: icon
        asynchronous: true
        width: 85
        height: 85
        fillMode: Image.PreserveAspectFit
        source: "qrc:/components/light/images/icon-light.png"
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
        text: obj.attributes.brightness
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

    BasicUI.CustomButton {
        anchors { left:parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
        color: colorText
        buttonTextColor: colorBackground
        buttonText: obj.attributes.state == "off" ? "Turn on" : "Turn off"

        mouseArea.onClicked: {
            haptic.playEffect("click");
            integration[obj.integration].obj.light.toggle(obj.entity_id);
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
                lightButton.state = "closed"
                loader_main.state = "visible"
                var tmp = mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY;
                mainNavigationSwipeview.currentItem.mainNavigationLoader.active = false;
                mainNavigationSwipeview.currentItem.mainNavigationLoader.active = true;
                mainNavigationSwipeview.currentItem.mainNavigationLoader.item.contentY = tmp;
            }
        }
    }
}
