import QtQuick 2.9
import QtQuick.Controls 2.2
import QtWebSockets 1.0
import QtGraphicalEffects 1.0
import QtQuick.VirtualKeyboard 2.2
import QtQuick.VirtualKeyboard.Settings 2.2

import Launcher 1.0

ApplicationWindow {
    id: applicationWindow

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // COLORS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property string colorBackground: "#000000"
    property string colorBackgroundTransparent: "#00000000"

    property string colorText: "#ffffff"
    property string colorLine: "#ffffff"
    property string colorHighlight: "#918682"

    property string colorLight: "#2E373D"
    property string colorMedium: "#121519"
    property string colorDark: "#16191E"
    property string colorDarkest: "#0E0F12"

    property string colorGreen: "#19D37B"
    property string colorRed: "#EA003C"

    property string colorSwitch: "#1E242C"
    property string colorSwitchOn : "#ffffff"
    property string colorSwitchBackground: "#000000"

    property string colorButton: "#121519"
    property string colorButtonPressed: "#16191E"
    property string colorButtonFav: "#1A1D23"

    property string colorRoundButton: "#1A1D23"

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Main application window properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    visible: true
    width: 480
    height: 800
    color: colorBackground

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property bool wifiSelected: false
    property var wifiSelectedButton
    property bool wifiPassword: false
    property var wifiNetworks: []
    property var wifiNetworksRSSI: []
    property var wifiNetworkSelected
    property var wifiNetworkSelectedRSSI

    property var homeAssistantIP
    property var homeAssistantToken
    property var homeAssistantAreas: []
    property var homeAssistantDevices: []
    property var homeAssistantEntities: []

    property var selectedEntities: []
    property int selectedEntitiesCounter: 0

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SUPPORTED ENTITIES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property var supportedEntities: ["media_player","light","cover","climate","remote","alarm_control_panel"]

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    function sortJsonArrayByProperty(objArray, prop, direction){
        if (arguments.length<2) throw new Error("sortJsonArrayByProp requires 2 arguments");
        var direct = arguments.length>2 ? arguments[2] : 1; //Default to ascending

        if (objArray && objArray.constructor===Array){
            var propPath = (prop.constructor===Array) ? prop : prop.split(".");
            objArray.sort(function(a,b){
                for (var p in propPath){
                    if (a[propPath[p]] && b[propPath[p]]){
                        a = a[propPath[p]];
                        b = b[propPath[p]];
                    }
                }
                // convert numeric strings to integers
                a = a.match(/^\d+$/) ? +a : a;
                b = b.match(/^\d+$/) ? +b : b;
                return ( (a < b) ? -1*direct : ((a > b) ? 1*direct : 0) );
            });
        }
    }


    function buttonClicked(buttonId) {
        if (!wifiSelected) {
            wifiSelected = true
            wifiButtons[buttonId].selected = true
            wifiSelectedButton = buttonId
            wifiNetworkSelected = wifiButtons[buttonId].ssid
            wifiNetworkSelectedRSSI = wifiButtons[buttonId].rssi
        } else {
            if (wifiSelectedButton != buttonId) {
                wifiButtons[wifiSelectedButton].selected = false
                wifiButtons[buttonId].selected = true
                wifiSelectedButton = buttonId
                wifiNetworkSelected = wifiButtons[buttonId].ssid
                wifiNetworkSelectedRSSI = wifiButtons[buttonId].rssi
            } else {
                wifiButtons[wifiSelectedButton].selected = false
                wifiSelected = false
                wifiNetworkSelected = ""
                wifiNetworkSelectedRSSI = ""
            }
        }
    }

    property var wifiButtons: []

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // WEBSOCKET
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    function findAreaName(area_id) {
        for (var i=0; i<homeAssistantAreas.length; i++) {
            if (homeAssistantAreas[i].id == area_id) {
                return homeAssistantAreas[i].name;
            }
        }
    }

    function findArea(device_id) {
        for (var i=0; i<homeAssistantDevices.length; i++) {
            if (homeAssistantDevices[i].id == device_id) {
                return findAreaName(homeAssistantDevices[i].area_id);
            }
        }
    }

    function webSocketProcess(message) {

        //        console.log(message);
        var json = JSON.parse(message);

        if (json.error != null) {
            console.log(json.error);
        }

        if (json.type == "auth_required") {
            json = {};
            json["type"] = "auth";
            json["access_token"] = homeAssistantToken;
            socket.sendTextMessage(JSON.stringify(json));
        }

        if (json.type == "auth_ok") {
            console.log("Connection successful")
            // GET AREAS
            socket.sendTextMessage('{"id": 2, "type": "config/area_registry/list"}\n');
        }

        if (json.success == true && json.id == 2) {
            // AREA RESPONSE
            for (var i=0; i<json.result.length; i++) {
                // process area json result
                homeAssistantAreas[i] = {};
                homeAssistantAreas[i].id = json.result[i].area_id;
                homeAssistantAreas[i].name = json.result[i].name;
            }
            socket.sendTextMessage('{"id": 3, "type": "get_states"}\n');
        }

        if (json.success == true && json.id == 3) {
            // STATES RESPONSE
            for (var i=0; i<json.result.length; i++) {
                var type = json.result[i].entity_id.split('.');

                homeAssistantEntities[i] = {};
                homeAssistantEntities[i].entity_id = json.result[i].entity_id;
                homeAssistantEntities[i].friendly_name = json.result[i].attributes.friendly_name;
                homeAssistantEntities[i].type = type[0];
            }
            socket.sendTextMessage('{"id": 4, "type": "config/device_registry/list"}\n');
        }

        if (json.success == true && json.id == 4) {
            // DEVICES RESPONSE
            for (var i=0; i<json.result.length; i++) {
                homeAssistantDevices[i] = {};
                homeAssistantDevices[i].id = json.result[i].id;
                homeAssistantDevices[i].area_id = json.result[i].area_id;
            }
            socket.sendTextMessage('{"id": 5, "type": "config/entity_registry/list"}\n');
        }

        if (json.success == true && json.id == 5) {
            // ENTITIES RESPONSE
            for (var i=0; i<json.result.length; i++) {
                for (var k=0; k<homeAssistantEntities.length; k++) {
                    if (homeAssistantEntities[k].entity_id == json.result[i].entity_id) {
                        homeAssistantEntities[k].device_id = json.result[i].device_id;
                        homeAssistantEntities[k].room = findArea(json.result[i].device_id);
                    }
                }
            }

            // list all supported entities
//            for (i=0; i<homeAssistantEntities.length; i++) {
//                for (var k=0; k<supportedEntities.length; k++) {
//                    if (supportedEntities[k] == homeAssistantEntities[i].type) {
//                        console.log(homeAssistantEntities[i].friendly_name + "    Entity id: " + homeAssistantEntities[i].entity_id + "    Room: " + homeAssistantEntities[i].room);
//                    }
//                }
//            }

            // SUCCES MOVE ON TO THE NEXT PAGE
            connectionLoaderText.text = "Connected"
            connectionLoaderSmallText.text = "";
            connectionLoader.state = "connected";
            mainSwipeview.currentIndex += 1
        }
    }

    WebSocket {
        id: socket
        url: "ws://" + homeAssistantIP + "/api/websocket"
        onTextMessageReceived: {
            //                        console.log("\nReceived message: " + message)
            webSocketProcess(message);
            //            parseWebsocketMessage.sendMessage( {message: message, devices: devices} );
        }
        onStatusChanged: if (socket.status == WebSocket.Error) {
                             //                             console.log("Error: " + socket.errorString)
                             //                             socket.active = false
                             //                             connectionLoader.state = "disconnected"
                             //                             console.log("Websocket connection error: " + webSocketConnection)
                             connectionLoaderText.text = "Failed to connect to Home Assistant"
                             connectionLoaderSmallText.text = "Make sure you have entered \n the credidentials corretly"
                             connectionLoader.state = "failed";
                         } else if (socket.status == WebSocket.Open) {
                             //                             statusBar.iconWifi.state = "wifi"
                             //                             socket.sendTextMessage('{"id": 1, "type": "get_states"}\n')
                             //                             socket.sendTextMessage('{"id": 2, "type": "subscribe_events", "event_type": "state_changed"}\n')
                             //                             FN.connNotification("Connecting")
                         } else if (socket.status == WebSocket.Closed) {
                             connectionLoaderText.text = "Failed to connect to Home Assistant"
                             connectionLoaderSmallText.text = "Make sure you have entered \n the credidentials corretly"
                             connectionLoader.state = "failed";
                             //                             socket.active = false
                             //                             console.log("Websocket connection closed: " + webSocketConnection);
                         }
        active: false
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN CONTAINER CONTENT
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Launcher { id: myLauncher }


    SwipeView {
        id: mainSwipeview
        width: parent.width
        height: parent.height
        clip: true
        interactive: false

        // Welcome page
        Item {
            Text {
                id: text_hello
                color: colorText
                text: "Hello"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 342
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.family: "Neuzeit Grotesk"
                font.weight: Font.Light
                font.styleName: "Light"
                font.pixelSize: 78
            }

            Text {
                color: colorText
                opacity: 0.4
                text: "Let’s set up a WiFi connection and Home Assistant."
                horizontalAlignment: Text.AlignHCenter
                anchors.top: text_hello.bottom
                anchors.topMargin: 0
                anchors.horizontalCenter: text_hello.horizontalCenter
                width: parent.width-80
                wrapMode: Text.WordWrap
                font.family: "Neuzeit Grotesk"
                font.weight: Font.Normal
                font.pixelSize: 28
                lineHeight: 1
            }

            CustomButton {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 60
                buttonText: "Next"

                mouseArea.onClicked: {
                    // Start wifi network scan
                    var tmp = myLauncher.launch("/usr/bin/wifi_network_list.sh");
                    tmp = tmp.split('\n');
                    for (var i=0; i<tmp.length-1; i++) {
                        var wifitmp = tmp[i].split(',')
                        wifiNetworks[i] = wifitmp[1];
                        wifiNetworksRSSI[i] = wifitmp[0]
                    }

                    mainSwipeview.currentIndex += 1
                }
            }

        }

        // Wifi step 1: choose a network

        Loader {
            id: loaderWifiStep1
            active: mainSwipeview.currentIndex == 1 ? true : false
            asynchronous: true
            source: "qrc:/WifiStep1.qml"

            onStatusChanged: if ( loaderWifiStep1.status == Loader.Ready) {
                                 loaderWifiStep1.item.addNetworks();
                             }

        }

        // Wifi step 2: enter password or enter ssid and password

        Loader {
            id: loaderWifiStep2
            active: mainSwipeview.currentIndex == 2 ? true : false
            asynchronous: true
            y: inputPanel.state == "visible" ? (loaderWifiStep2.source == "qrc:/WifiStep2.qml" ? -210 : -270) : 0
        }

        // Home assistant step 1: enter credidentials

        Loader {
            id: loaderHomeAssistantStep1
            active: mainSwipeview.currentIndex == 3 ? true : false
            asynchronous: true
            source: "qrc:/HomeAssistant1.qml"
            y: inputPanel.state == "visible" ? -310 : 0
        }

        // Home assistant step 2: choose entities

        Loader {
            id: loaderHomeAssistantStep2
            active: mainSwipeview.currentIndex == 4 ? true : false
            asynchronous: true
            source: "qrc:/HomeAssistant2.qml"
        }
    }

    InputPanel {
        id: inputPanel
        width: parent.width
        y: applicationWindow.height

        states: State {
            name: "visible"
            when: inputPanel.active
            PropertyChanges {
                target: inputPanel
                y: applicationWindow.height - inputPanel.height
            }
        }
        transitions: Transition {
            id: inputPanelTransition
            from: ""
            to: "visible"
            reversible: true
            ParallelAnimation {
                NumberAnimation {
                    properties: "y"
                    duration: 300
                    easing.type: Easing.InOutExpo
                }
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECTION NOTIFIER
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Rectangle {
        id: connectionLoader
        anchors.fill: parent
        color: colorBackground

        state: "connected"

        property int iconShowDelay: 0

        states: [
            State {
                name: "connecting"
                PropertyChanges {target: connectionLoader; opacity: 1; visible: true}
                PropertyChanges {target: loadingIcon; opacity: 1}
                PropertyChanges {target: loadingIconOK; opacity: 0}
                PropertyChanges {target: loadingIconX; opacity: 0}
                PropertyChanges {target: loadingIconAnim; running: true}
                //                PropertyChanges {target: buttonTryAgain; opacity: 0}
                //                PropertyChanges {target: connectionLoaderText; text: "Connecting ..."}
            },
            State {
                name: "connected"
                PropertyChanges {target: connectionLoader; opacity: 0; visible: false}
                PropertyChanges {target: loadingIcon; opacity: 0}
                PropertyChanges {target: loadingIconOK; opacity: 1}
                PropertyChanges {target: loadingIconX; opacity: 0}
                //                PropertyChanges {target: buttonTryAgain; opacity: 0}
                PropertyChanges {target: loadingIconAnim; running: false}
                //                PropertyChanges {target: connectionLoaderText; text: "Connected"}
            },
            State {
                name: "saved"
                PropertyChanges {target: loadingIcon; opacity: 0}
                PropertyChanges {target: loadingIconOK; opacity: 1}
                PropertyChanges {target: loadingIconX; opacity: 0}
                //                PropertyChanges {target: buttonTryAgain; opacity: 0}
                PropertyChanges {target: loadingIconAnim; running: false}
                //                PropertyChanges {target: connectionLoaderText; text: "Connected"}
            },
            State {
                name: "failed"
                PropertyChanges {target: loadingIcon; opacity: 0}
                PropertyChanges {target: loadingIconOK; opacity: 0}
                PropertyChanges {target: loadingIconX; opacity: 1}
                PropertyChanges {target: loadingIconAnim; running: false}
                //                PropertyChanges {target: buttonTryAgain; opacity: 1}
                //                PropertyChanges {target: connectionLoaderText; text: "Connection failed"}
            }
        ]

        transitions: [
            Transition {
                to: "connecting"
                SequentialAnimation {
                    PauseAnimation { duration: connectionLoader.iconShowDelay }
                    PropertyAnimation { target: connectionLoader; properties: "visible"; duration: 0 }
                    PropertyAnimation { target: loadingIcon; properties: "opacity"; duration: 0 }
                    PropertyAnimation { target: loadingIconAnim; properties: "running"; duration: 0 }
                    PropertyAnimation { target: loadingIconOK; properties: "opacity"; duration: 0 }
                    PropertyAnimation { target: loadingIconX; properties: "opacity"; duration: 0 }
                    //                    PropertyAnimation { target: buttonTryAgain; properties: "opacity"; duration: 0 }
                    //                    PropertyAnimation { target: connectionLoaderText; properties: "text"; duration: 0 }
                    PropertyAnimation { target: connectionLoader; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
                }},
            Transition {
                //                from: "connecting"
                to: "connected"
                SequentialAnimation {
                    ParallelAnimation {
                        PropertyAnimation { target: loadingIcon; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                        PropertyAnimation { target: loadingIconAnim; properties: "running"; duration: 0 }
                        PropertyAnimation { target: loadingIconOK; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
                        //                        PropertyAnimation { target: connectionLoaderText; properties: "text"; duration: 0 }
                    }
                    PauseAnimation { duration: 800 }
                    PropertyAnimation { target: connectionLoader; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
                    PropertyAnimation { target: connectionLoader; properties: "visible"; duration: 0 }
                }},
            Transition {
                //                from: "connecting"
                to: "saved"
                SequentialAnimation {
                    ParallelAnimation {
                        PropertyAnimation { target: loadingIcon; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                        PropertyAnimation { target: loadingIconAnim; properties: "running"; duration: 0 }
                        PropertyAnimation { target: loadingIconOK; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
                    }
                }},
            Transition {
                from: "connecting"
                to: "failed"
                SequentialAnimation {
                    ParallelAnimation {
                        PropertyAnimation { target: loadingIcon; properties: "opacity"; easing.type: Easing.InExpo; duration: 400 }
                        PropertyAnimation { target: loadingIconAnim; properties: "running"; duration: 0 }
                        PropertyAnimation { target: loadingIconX; properties: "opacity"; easing.type: Easing.OutExpo; duration: 400 }
                        //                        PropertyAnimation { target: connectionLoaderText; properties: "text"; duration: 0 }
                    }
                    //                    PauseAnimation { duration: 200 }
                    //                    PropertyAnimation { target: buttonTryAgain; properties: "opacity"; easing.type: Easing.InExpo; duration: 600 }
                }}
        ]

        Rectangle {
            anchors.fill: parent
            color: colorBackground
            opacity: 0.96
        }

        Image {
            asynchronous: true
            id: loadingIcon
            width: 160
            height: 160
            opacity: 0
            y: 300
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/icon-loading.png"

            RotationAnimator on rotation {
                id: loadingIconAnim
                running: false
                loops: Animation.Infinite
                from: 0
                to: 360
                duration: 2000
            }

            Behavior on y {
                NumberAnimation { duration: 400 }
            }
        }

        Image {
            asynchronous: true
            id: loadingIconOK
            width: 160
            height: 160
            opacity: 0
            y: 300
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/icon-loading-ok.png"

            Behavior on y {
                NumberAnimation { duration: 400 }
            }
        }

        Image {
            asynchronous: true
            id: loadingIconX
            width: 160
            height: 160
            opacity: 0
            y: 300
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/icon-loading-x.png"

            Behavior on y {
                NumberAnimation { duration: 400 }
            }
        }

        Text {
            id: connectionLoaderText
            visible: true
            color: colorText
            width: 300
            text: "Reconnecting ..."
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: loadingIcon.bottom
            anchors.topMargin: 20
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            font.family: "Neuzeit Grotesk"
            font.weight: Font.Light
            font.pixelSize: 28
            lineHeight: 0.8
        }

        Text {
            id: connectionLoaderSmallText
            color: colorText
            opacity: 0.5
            width: 390
            visible: connectionLoaderSmallText.text != "" ? true : false
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 60
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            font.family: "Neuzeit Grotesk"
            font.weight: Font.Light
            font.pixelSize: 22
            lineHeight: 1
        }

        CustomButton {
            id: actionButton
            visible: connectionLoader.state == "saved" ? true : false
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 60
            buttonText: "Restart"

            mouseArea.onClicked: {
                myLauncher.launch("reboot");
            }
        }
        Image {
            id: backButton
            asynchronous: true
            width: 30
            height: 50
            visible: connectionLoader.state == "saved" ? true : false
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
                    connectionLoader.visible = false
                    mainSwipeview.currentIndex -= 1
                }
            }
        }

    }

}
