import QtQuick 2.11
import QtWebSockets 1.0

import "qrc:/scripts/helper.js" as JSHelper

Item {
    id: integration

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT AND DISCONNECT FUNCTIONS
    // Must be the same function name for every integration
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function connect()
    {
        // write connect function here

        // set the status bar loading icon to visible
        connecting = true;

        // reset the reconnnect trial variable
        websocketReconnect.tries = 0;

        // turn on the websocket connection
        socket.active = true;
    }

    function disconnect()
    {
        // write disconnect function here

        // turn of the reconnect try
        websocketReconnect.running = false;

        // turn off the socket
        socket.active = false;

        // disable the status bar loading icon
        connecting = false;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // COMMON VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property bool connected: false  // reports the state if the connection is succesful or not. Update his variable when handling the connection
    property bool connectionOpen: false // this varialbe controls the connection. If set to [true], the connection should start, if set to [false] the connection to the hub should be terminated

    property int integrationId // id of the integration

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // COMMON VARIABLES END
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECTION CONTROL
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property int webSocketId: 4

    onConnectionOpenChanged: {
        if (connectionOpen) {
            connecting = true;
            websocketReconnect.tries = 0;
            socket.active = true;
        } else {
            websocketReconnect.running = false;
            socket.active = false;
            connecting = false;
        }
    }

    onConnectedChanged: {
        if (connected) {
            // remove notifications that say couldn't connec to Home Assistant
            var tmp = notifications;
            tmp.forEach(function(entry, index, object) {
                if (entry.text === "Failed to connect to Home Assistant.") {
                    tmp.splice(index, 1);
                }
            });
            notifications = tmp;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECTION CONTROL END
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SIGNALS
    // Signals to push the data to the components
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property var fetch_json
    property var event_json

    signal sendFetchJson(var json)

    onFetch_jsonChanged: {
        integration.sendFetchJson(fetch_json)
    }

    signal sendEventJson(var json)

    onEvent_jsonChanged: {
        integration.sendEventJson(event_json)
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SIGNALS END
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function webSocketProcess (message) {

        //                        console.debug(message);
        var json = JSON.parse(message);

        if (json.error != null) {
            console.debug(json.error);
        }

        if (json.type == "auth_required") {
            json = {};
            json["type"] = "auth";
            json["access_token"] = config.integration[integrationId].data.token;
            socket.sendTextMessage(JSON.stringify(json));
        }

        if (json.type == "auth_ok") {
            console.debug("Connection successful")
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // FETCH STATES
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            socket.sendTextMessage('{"id": 2, "type": "get_states"}\n');
        }

        if (json.success == true && json.id == 2) {

            fetch_json = json;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // SUBSCRIBE TO EVENTS IN HOME ASSISTANT
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            socket.sendTextMessage('{"id": 3, "type": "subscribe_events", "event_type": "state_changed"}\n');
        }

        if (json.success == true && json.id == 3) {
            connecting = false;
            connected = true;
            console.debug("Subscribed to state changes");
        }

        if (json.success == true && json.id == webSocketId) {
            console.debug("Command successful");
        }

        if (json.success == false) {
            console.debug("Websocket error: " + json.error.message);
        }

        if (json.type == "event" && json.id == 3) {
            event_json = json;
        }
    }


    function webSocketSendCommand(domain, service, entity_id, data) { // sends a command to home assistant

        webSocketId++;

        var json = {};
        json["id"] = webSocketId;
        json["type"] = "call_service";
        json["domain"] = domain;
        json["service"] = service;
        if (data == null) {
            json["service_data"] = {"entity_id" : entity_id};
        } else {
            data["entity_id"] = entity_id;
            json["service_data"] = data;
        }
        socket.sendTextMessage(JSON.stringify(json));
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // QML ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    WebSocket {
        id: socket
        url: "ws://" + config.integration[integrationId].data.ip + "/api/websocket"
        onTextMessageReceived: {
            //            console.debug("\nReceived message: " + message)
            webSocketProcess(message);
        }
        onStatusChanged: if (socket.status == WebSocket.Error) {
                             console.debug("Error: " + socket.errorString)
                             socket.active = false
                             connected = false
                             console.debug("Websocket connection error: " + connected)
                             websocketReconnect.start()
                         } else if (socket.status == WebSocket.Open) {
                             // open
                         } else if (socket.status == WebSocket.Closed) {
                             socket.active = false
                             connected = false
                             console.debug("Websocket connection closed: " + connected);
                             websocketReconnect.start()
                         }
        active: false
    }

    Timer {
        id: websocketReconnect
        running: false
        repeat: false
        interval: 2000

        property int tries: 0

        onTriggered: {
            if (tries == 1) {
                websocketReconnect.running = false;
                //                connectionState = "failed"
                console.debug("Failed to connect");

                addNotification("error", qsTr("Failed to connect to Home Assistant.") + translateHandler.emptyString, function () { integration.integration.obj.connectionOpen = true; }, "Reconnect");
                connectionOpen = false;

                tries = 0
            } else {
                webSocketId = 4
                if (!connecting) {
                    connecting = true;
                }
                socket.active = true
                console.debug("Reconnecting...")
                tries += 1
            }
        }
    }

}
