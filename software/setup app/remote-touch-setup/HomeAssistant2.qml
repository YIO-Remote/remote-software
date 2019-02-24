import QtQuick 2.9
import QtQuick.Controls 2.2

import FileHelpers 1.0

Item {

    Flickable {
        width: parent.width
        height: parent.height
        clip: true
        contentHeight: roomsFlow.height+600
        boundsBehavior: Flickable.DragAndOvershootBounds
        flickableDirection: Flickable.VerticalFlick
        maximumFlickVelocity: 4000
        flickDeceleration: 400

        opacity: Math.max(0.5, 1.0 - Math.abs(verticalOvershoot) / height*4)

        ScrollBar.vertical: ScrollBar {
            opacity: 0.5
            anchors.right: parent.right
            anchors.rightMargin: 10
        }

        Text {
            id: text_homeassistant
            color: colorText
            width: 300
            text: "Home Assistant"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 100
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            font.family: "Neuzeit Grotesk"
            font.weight: Font.Light
            font.styleName: "Light"
            font.pixelSize: 78
        }

        Text {
            color: colorText
            opacity: 0.4
            text: "Please select the entities, you would like to control"
            horizontalAlignment: Text.AlignHCenter
            anchors.top: text_homeassistant.bottom
            anchors.topMargin: 0
            anchors.horizontalCenter: text_homeassistant.horizontalCenter
            width: parent.width-120
            wrapMode: Text.WordWrap
            font.family: "Neuzeit Grotesk"
            font.weight: Font.Normal
            font.pixelSize: 28
            lineHeight: 1
        }

        Flow {
            id: roomsFlow
            width: parent.width-40
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: text_homeassistant.bottom
            anchors.topMargin: 100
            spacing: 2

            Component.onCompleted: {
                var comp;
                var obj;

                var entity_comp;
                var entity_obj;

                sortJsonArrayByProperty(homeAssistantEntities, 'entity_id');

                // add rooms
                for (var i=0; i<supportedEntities.length; i++) {
                    var counter = 0;
                    comp = Qt.createComponent("qrc:/HomeAssistantAreaElement.qml");
                    obj = comp.createObject(roomsFlow, {roomTitle:supportedEntities[i]});

                    counter++;

                    for (var k=0; k<homeAssistantEntities.length; k++) {
                        var entity_counter = 0;
                        if (homeAssistantEntities[k].type == supportedEntities[i]) {

                            entity_comp = Qt.createComponent("qrc:/HomeAssistantEntityElement.qml");
                            entity_obj = entity_comp.createObject(roomsFlow, {friendly_name: homeAssistantEntities[k].friendly_name, entity_id: homeAssistantEntities[k].entity_id, roomText: homeAssistantEntities[k].room, type: supportedEntities[i]});
                            entity_counter++;
                        }

                    }

                    obj.height += 120*counter + entity_counter*60;
                }
            }
        }

        Text {
            color: colorText
            text: selectedEntitiesCounter == 1 || selectedEntitiesCounter == 0 ? selectedEntitiesCounter + " entity selected" : selectedEntitiesCounter + " entities selected"
            horizontalAlignment: Text.AlignHCenter
            anchors.top: roomsFlow.bottom
            anchors.topMargin: 40
            anchors.horizontalCenter: text_homeassistant.horizontalCenter
            width: parent.width
            font.family: "Neuzeit Grotesk"
            font.weight: Font.Normal
            font.pixelSize: 22
            lineHeight: 1
        }

        FileHelper {
            id: filehelper
        }

        CustomButton {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: roomsFlow.bottom
            anchors.topMargin: 120
            buttonText: "Done"

            mouseArea.onClicked: {
                connectionLoaderText.text = "Saving configuration ..."
                connectionLoaderSmallText.text = "";
                connectionLoader.state = "connecting";

                function getEntities(type) {
                    var tmp = [];
                    var counter = 0;
                    for (var i=0; i<selectedEntities.length; i++) {
                        if (selectedEntities[i].type == type) {
                            tmp[counter] = selectedEntities[i];
                            counter++;
                        }
                    }
                    return tmp;
                }

                var homeAssistantConfigJson = {}
                homeAssistantConfigJson.homeassistant = {}
                homeAssistantConfigJson.homeassistant.token = {}

                homeAssistantConfigJson.homeassistant.ip = homeAssistantIP;
                homeAssistantConfigJson.homeassistant.token = homeAssistantToken;

                homeAssistantConfigJson.rooms = homeAssistantAreas;

                for (var i=0; i<supportedEntities.length; i++) {
                    homeAssistantConfigJson[supportedEntities[i]+"s"] = getEntities(supportedEntities[i]);
                }

                var success = filehelper.writeFile(JSON.stringify(homeAssistantConfigJson));

                if (success == true) {
                    connectionLoaderText.text = "Configuration saved"
                    connectionLoaderSmallText.text = "You can also change the configuration manually by editing home-assistant.json on the SD card.";
                    connectionLoaderSmallText.anchors.bottomMargin = 192;
                    connectionLoader.state = "saved";
                } else {
                    connectionLoaderText.text = "Failed to save the configuration"
                    connectionLoaderSmallText.text = "Please reboot the remote"
                    connectionLoaderSmallText.anchors.bottomMargin = 192;
                    connectionLoader.state = "failed";
                }

            }
        }
    }

    Image {
        asynchronous: true
        width: 30
        height: 50
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/icon-back.png"
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 15

        MouseArea {
            width: parent.width + 40
            height: width
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            onClicked: {
                mainSwipeview.currentIndex -= 1
                socket.active = false
                connectionLoader.visible = false
            }
        }
    }

}


