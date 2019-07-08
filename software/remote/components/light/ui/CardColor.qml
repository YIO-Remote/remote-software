import QtQuick 2.11
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Rectangle {
    id: cardColor
    width: parent.width
    height: parent.height
    color: colorMedium

    property int _brightness: obj.attributes.brightness

    signal updateBrightness()

    on_BrightnessChanged: {
        updateBrightness()
    }

    Flipable {
        id: flipable
        width: parent.width
        height: parent.height

        property bool flipped: false

        transform: Rotation {
            id: rotation
            origin.x: flipable.width/2
            origin.y: flipable.height/2
            axis.x: 0; axis.y: 1; axis.z: 0     // set axis.y to 1 to rotate around y-axis
            angle: 0    // the default angle
        }

        states: State {
            name: "back"
            PropertyChanges { target: rotation; angle: 180 }
            when: flipable.flipped
        }

        transitions: Transition {
            NumberAnimation { target: rotation; property: "angle"; easing.type: Easing.OutExpo; duration: 300 }
        }

        front: Item {
            anchors.fill: parent

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
                target: cardColor

                onUpdateBrightness: {
                    percentageBG.height = parent.height*brightness/100;
                    percentageBG2.height = parent.height*brightness/100;
                    percentage.text = brightness;
                }
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
                height: parent.height*brightness/100
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
                text: brightness
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
                id: toggleButton
                anchors { left:parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
                color: colorText
                buttonTextColor: colorBackground
                buttonText: obj.attributes.state == "off" ? "Turn on" : "Turn off"

                mouseArea.onClicked: {
                    haptic.playEffect("click");
                    integration[obj.integration].obj.light.toggle(obj.entity_id);
                }
            }

            BasicUI.CustomButton {
                anchors { left:toggleButton.right; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
                color: obj.attributes.color ? Qt.rgba(obj.attributes.color[0]/255, obj.attributes.color[1]/255, obj.attributes.color[2]/255, 1) : colorText
                buttonTextColor: colorBackground
                buttonText: "Color"

                mouseArea.onClicked: {
                    haptic.playEffect("click");
                    flipable.flipped = !flipable.flipped
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
        } // front item end

        back: Item {
            anchors.fill: parent

            Canvas {
                id: canvas
                width: parent.width
                height: parent.height

                function getColor(x, y) {
                    var ctx = getContext("2d");
                    canvas.requestPaint();

                    var imageData = ctx.getImageData(x, y, 1, 1).data;

                    picker.x = x-picker.width/2;
                    picker.y = y-picker.height/2;
                    picker.color = Qt.rgba(imageData[0]/255, imageData[1]/255, imageData[2]/255, 1);
                }

                onPaint: {
                    var ctx = getContext("2d");

                    var gradient = ctx.createLinearGradient(0, 0, parent.width, 0);

                    // Create color gradient
                    gradient.addColorStop(0,    "rgba(255,   0,   0, 1)");
                    gradient.addColorStop(0.15, "rgba(255,   0, 255, 1)");
                    gradient.addColorStop(0.33, "rgba(0,     0, 255, 1)");
                    gradient.addColorStop(0.49, "rgba(0,   255, 255, 1)");
                    gradient.addColorStop(0.67, "rgba(0,   255,   0, 1)");
                    gradient.addColorStop(0.84, "rgba(255, 255,   0, 1)");
                    gradient.addColorStop(1,    "rgba(255,   0,   0, 1)");

                    ctx.fillStyle = gradient;
                    ctx.fillRect(0, 0, parent.width, parent.height);

                    gradient = ctx.createLinearGradient(0, 0, 0, parent.height)

                    gradient.addColorStop(1,   "rgba(255, 255, 255, 1)");
                    gradient.addColorStop(0.5, "rgba(255, 255, 255, 0)");
                    gradient.addColorStop(0.5, "rgba(0,     0,   0, 0)");
                    gradient.addColorStop(0,   "rgba(0,     0,   0, 1)");

                    ctx.fillStyle = gradient;
                    ctx.fillRect(0, 0, parent.width, parent.height);

                }

                MouseArea {
                    anchors.fill: parent

                    onPositionChanged: {
                        canvas.getColor(mouseX, mouseY);

                        icon_b.opacity = 0;
                        color_b.opacity = 0;
                        title_b.opacity = 0;
                        areaText_b.opacity = 0;
                        button_b.opacity = 0;
                        closeButton_b.opacity = 0;
                        closeButton_b.enabled = false;
                    }

                    onReleased: {
                        integration[obj.integration].obj.light.setColor(obj.entity_id, picker.color);

                        icon_b.opacity = 1;
                        color_b.opacity = 1;
                        title_b.opacity = 1;
                        areaText_b.opacity = 1;
                        button_b.opacity = 1;
                        closeButton_b.opacity = 1;
                        closeButton_b.enabled = true;
                    }
                }
            }

            Image {
                id: icon_b
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

                Behavior on opacity {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }

            Text {
                id: color_b
                color: colorText
                text: "R: " + Math.round(picker.color.r*255) + "\nG: " + Math.round(picker.color.g*255) + "\nB: " + Math.round(picker.color.b*255)
                wrapMode: Text.WordWrap
                width: parent.width-60
                anchors { top: icon_b.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
                font {family: "Open Sans Regular"; pixelSize: 50 }
                lineHeight: 0.9

                Behavior on opacity {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }

            Text {
                id: title_b
                color: colorText
                text: obj.friendly_name
                wrapMode: Text.WordWrap
                width: parent.width-60
                anchors { top: color_b.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
                font {family: "Open Sans SemiBold"; pixelSize: 60 }
                lineHeight: 0.9

                Behavior on opacity {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }

            Text {
                id: areaText_b
                color: colorText
                opacity: 0.5
                text: obj.area
                elide: Text.ElideRight
                wrapMode: Text.NoWrap
                width: parent.width-60
                anchors { top: title_b.bottom; topMargin: 20; left: parent.left; leftMargin: 30 }
                font {family: "Open Sans Regular"; pixelSize: 24 }

                Behavior on opacity {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }

            Rectangle {
                id: picker
                width: 60
                height: width
                border.color: colorText
                border.width: 4
                radius: width/2
            }

            BasicUI.CustomButton {
                id: button_b
                anchors { left:parent.left; leftMargin: 30; bottom: parent.bottom; bottomMargin: 70 }
                color: obj.attributes.color ? Qt.rgba(obj.attributes.color[0]/255, obj.attributes.color[1]/255, obj.attributes.color[2]/255, 1) : colorText
                buttonTextColor: colorBackground
                buttonText: "OK"

                mouseArea.onClicked: {
                    haptic.playEffect("click");
                    flipable.flipped = !flipable.flipped
                }

                Behavior on opacity {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }

            Image {
                id: closeButton_b
                asynchronous: true
                width: 30
                height: 30
                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/components/close_button.png"
                anchors.right: parent.right
                anchors.rightMargin: 20
                anchors.top: parent.top
                anchors.topMargin: 20

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

                Behavior on opacity {
                    PropertyAnimation { easing.type: Easing.OutExpo; duration: 300 }
                }
            }

        } // back Item end
    } // flipabble end
}
