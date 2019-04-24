import QtQuick 2.0

import "qrc:/scripts/helper.js" as JSHelper

Item {
    id: lightComponent_homeassistant


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SIGNALS
    // Connections to receive data from the integration
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Connections {
        id: connectionToHub
        target: integration.homeassistant
        onSendFetchJson: {
            // Process fetched json here. This only has to be one time after connecting to the hub
//            var tmp = entities_light;

            var tmp = loaded_components.light.entities;

            for (var i=0; i<json.result.length; i++) {
                for (var k=0; k<loaded_components.light.entities.length; k++) {
                    if (json.result[i].entity_id == loaded_components.light.entities[k].entity_id) {

                        tmp[k].state = json.result[i].state;

                        if (json.result[i].attributes.brightness == null) {
                            tmp[k].brightness = "0";
                        } else {
                            tmp[k].brightness = JSHelper.convertToPercentage(json.result[i].attributes.brightness); // converting the brightness to percentage
                        }

                        tmp[k].friendly_name = json.result[i].attributes.friendly_name;
                    }
                }
            }

            loaded_components.light.entities = tmp;
        }
        onSendEventJson: {
            // Process event json here. Every time there's an update

            var entity_id = json.event.data.entity_id;
            var state = json.event.data.new_state.state;
            var brightness = json.event.data.new_state.attributes.brightness;

            var tmp = loaded_components.light.entities;

            for (var k=0; k < loaded_components.light.entities.length; k++) {
                if (entity_id == loaded_components.light.entities[k].entity_id) {

                    tmp[k].state = state;

                    if (brightness == null) {
                        tmp[k].brightness = "0";
                    } else {
                        tmp[k].brightness = JSHelper.convertToPercentage(brightness); // converting the brightness to percentage
                    }

                    loaded_components.light.entities = tmp;
                }
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function toggle(entity_id) {
        integration.homeassistant.webSocketSendCommand("light","toggle",entity_id);
    }

    function setBrightness(entity_id, brightness) {
        var data = {"brightness_pct": brightness}
        integration.homeassistant.webSocketSendCommand("light","turn_on",entity_id, data);
    }

    function turnOn(entity_id) {
        integration.homeassistant.webSocketSendCommand("light","turn_on",entity_id);
    }

    function turnOff(entity_id) {
        integration.homeassistant.webSocketSendCommand("light","turn_off",entity_id);
    }
}
