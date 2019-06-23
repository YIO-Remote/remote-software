import QtQuick 2.0

import "qrc:/scripts/helper.js" as JSHelper

Item {
    id: componentIntegration

    //  Bitfield of features supported by the light entity FROM HOME ASSISTANT
    property int _SUPPORT_BRIGHTNESS: 1
    property int _SUPPORT_COLOR_TEMP: 2
    property int _SUPPORT_EFFECT: 4
    property int _SUPPORT_FLASH: 8
    property int _SUPPORT_COLOR: 16
    property int _SUPPORT_TRANSITION: 32
    property int _SUPPORT_WHITE_VALUE: 128

    property var e


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SIGNALS
    // Connections to send and receive data from the integration
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Connections {
        id: connectionToHub
        target: parent
        onSendFetchJson: {
            // Process fetched json here. This only has to be one time after connecting to the hub
            e = entities.getByType("light");
            var attr = {};
            var sf = [];

            for (var i=0; i<json.result.length; i++) {
                for (var k=0; k<e.length; k++) {
                    if (json.result[i].entity_id === e[k].entity_id) {

                        sf = [];

                        attr.state = json.result[i].state;

                        if (json.result[i].attributes.brightness === null || !json.result[i].attributes.brightness) {
                            attr.brightness = "0";
                        } else {
                            attr.brightness = JSHelper.convertToPercentage(json.result[i].attributes.brightness); // converting the brightness to percentage
                        }

                        if (json.result[i].attributes.rgb_color) {
                            attr.color = json.result[i].attributes.rgb_color;
                        }

                        // Brightness
                        if (json.result[i].attributes.supported_features & _SUPPORT_BRIGHTNESS) {
                            sf.push("BRIGHTNESS");
                        }

                        // Color
                        if (json.result[i].attributes.supported_features & _SUPPORT_COLOR) {
                            sf.push("COLOR");
                        }

                        // Color temperature
                        if (json.result[i].attributes.supported_features & _SUPPORT_COLOR_TEMP) {
                            sf.push("COLOR_TEMP");
                        }

                        e[k].attributes = attr;
                        e[k].supported_features = sf;
                    }
                }
            }
        }
        onSendEventJson: {
            // Process event json here. Every time there's an update

            var entity_id = json.event.data.entity_id;
            var state = json.event.data.new_state.state;
            var brightness = json.event.data.new_state.attributes.brightness;

            var attr = {};

            for (var k=0; k < e.length; k++) {
                if (entity_id === e[k].entity_id) {

                    attr.state = state;

                    if (brightness === null || !brightness) {
                        attr.brightness = "0";
                    } else {
                        attr.brightness = JSHelper.convertToPercentage(brightness); // converting the brightness to percentage
                    }

                    if (json.event.data.new_state.attributes.rgb_color) {
                        attr.color = json.event.data.new_state.attributes.rgb_color;
                    }

                    e[k].attributes = attr;
                }
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function toggle(entity_id) {
        parent.webSocketSendCommand("light","toggle",entity_id);
    }

    function setBrightness(entity_id, brightness) {
        var data = {"brightness_pct": brightness}
        parent.webSocketSendCommand("light","turn_on",entity_id, data);
    }

    function setColor(entity_id, color) {
        var data = {"rgb_color": [color.r, color.g, color.b]}
        parent.webSocketSendCommand("light","turn_on",entity_id, data);
    }

    function turnOn(entity_id) {
       parent.webSocketSendCommand("light","turn_on",entity_id);
    }

    function turnOff(entity_id) {
        parent.webSocketSendCommand("light","turn_off",entity_id);
    }
}
