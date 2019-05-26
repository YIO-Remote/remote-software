import QtQuick 2.11
import QtWebSockets 1.0

import "qrc:/scripts/helper.js" as JSHelper

Item {
    id: homeassistant
    property bool webSocketConnection: false
    property int webSocketId: 4

    property int integrationId

    property bool connectionOpen: false

    onConnectionOpenChanged: {
        if (connectionOpen) {
            connectionState = "connecting"
            websocketReconnect.tries = 0;
            socket.active = true;
        } else {
            socket.active = false;
            websocketReconnect.running = false;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SIGNALS
    // Signals to push the data to the components
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property var fetch_json
    property var event_json

    signal sendFetchJson(var json)

    onFetch_jsonChanged: {
        homeassistant.sendFetchJson(fetch_json)
    }

    signal sendEventJson(var json)

    onEvent_jsonChanged: {
        homeassistant.sendEventJson(event_json)
    }


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
            if (!firstRun) {
                connectionState = "connected";
            }
            webSocketConnection = true;
            console.debug("Subscribed to state changes");
            console.debug("Command successful");
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
                             webSocketConnection = false
                             console.debug("Websocket connection error: " + webSocketConnection)
                             websocketReconnect.start()
                         } else if (socket.status == WebSocket.Open) {
                             // open
                         } else if (socket.status == WebSocket.Closed) {
                             socket.active = false
                             webSocketConnection = false
                             console.debug("Websocket connection closed: " + webSocketConnection);
                             websocketReconnect.start()
                         }
        active: true
    }

    Timer {
        id: websocketReconnect
        running: false
        repeat: false
        interval: 2000

        property int tries: 0

        onTriggered: {
            if (tries == 4) {
                websocketReconnect.running = false;
                connectionState = "failed"
                console.debug("Failed to connect")
                tries = 0
            } else {
                webSocketId = 4
                if (connectionState != "connecting") {
                    connectionState = "connecting"
                }
                socket.active = true
                console.debug("Reconnecting...")
                tries += 1
            }
        }
    }

}
