import QtQuick 2.11
import QtGraphicalEffects 1.0

import Entity.Remote 1.0

Item {
    ListModel {
        id: channelsModel
    }

    GridView {
        id: channelGridView
        width: parent.width-50
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        cellWidth: width/3; cellHeight: cellWidth
        clip: true

        model: channelsModel

        delegate: Item {
            id: main
            width: channelGridView.cellWidth
            height: width

            Rectangle {
                id: imageContainer
                width: 80
                height: width
                anchors.centerIn: parent
                radius: cornerRadius
                color: colorMedium

                Drag.active: mouseArea.drag.active
                Drag.source: mouseArea
                Drag.hotSpot.x: 40
                Drag.hotSpot.y: 40

                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: Item {
                        width: imageContainer.width
                        height: imageContainer.height
                        Rectangle {
                            anchors.fill: parent
                            radius: cornerRadius
                        }
                    }
                }

                Text {
                    color: colorText
                    text: number
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.centerIn: parent
                    font.family: "Open Sans Bold"
                    font.weight: Font.Normal
                    font.pixelSize: 40
                    lineHeight: 1
                }

                Image {
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectCrop
                    asynchronous: true
                    source: "file:/" + image
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        haptic.playEffect("click");
                        obj.channel(number);
                    }
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

    Component.onCompleted: {
        for (let i=0; i<obj.channels.length; i++) {
            var o = obj.channels[i];
            o["id"] = i;
            channelsModel.append(o);
        }
    }
}
