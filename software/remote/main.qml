import QtQuick 2.11
import QtQuick.Controls 2.4
import QtWebSockets 1.0

import Launcher 1.0
import JsonFile 1.0
import Haptic 1.0
import Battery 1.0

import "qrc:/scripts/softwareupdate.js" as JSUpdate
import "qrc:/basic_ui" as BasicUI

ApplicationWindow {
    id: applicationWindow

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
    //
    // CURRENT SOFTWARE VERSION
    property real _current_version: 0.1 // change this when bumping the software version

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BATTERY
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property real battery_voltage: 5
    property real battery_level: 1
    property real battery_health: 100
    property real battery_time: (new Date()).getTime()
    property bool wasBatteryWarning: false

    property var battery_data: []

    Battery {
        id: battery
        capacity: 2500

        Component.onCompleted: {
            battery.begin();
        }
    }

    Timer {
        running: true
        repeat: true
        interval: standbyControl.mode == "on" ? 4000 : 10000

        onTriggered: {
            battery_voltage = battery.getVoltage() / 1000
            battery_level = battery.getStateOfCharge() / 100
            battery_health = battery.getStateOfHealth()

            if (battery_voltage <= 3.4 && battery.getAveragePower() < 0) {
                // set turn on button to low
                buttonHandler.interruptHandler.shutdown();
                // halt
                mainLauncher.launch("halt");
            }

            if (battery.getAveragePower() > 0 ) {
                chargingScreen.item.state = "visible";
            } else {
                chargingScreen.item.state = "hidden";
            }
        }
    }

    // battery data logger
    Timer {
        running: true
        repeat: true
        interval: 600000

        onTriggered: {
            var tmp = {};
            tmp.timestamp = new Date().getTime();
            tmp.datestamp = new Date().getDate();
            tmp.level = battery_level;
            tmp.power = battery.getAveragePower();
            tmp.voltage = battery_voltage;

            battery_data.push(tmp);

            console.debug(JSON.stringify(battery_data));
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN WINDOW PROPERTIES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    visible: true
    width: 480
    height: 800
    color: colorBackground

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // COLORS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property int cornerRadius: 18 // radius of the buttons, defined here

    property bool darkMode: true

    property string colorBackground: darkMode ? "#000000" : "#ffffff"
    property string colorBackgroundTransparent: darkMode ? "#00000000" :  "#00000000"

    property string colorText: darkMode ? "#ffffff" : "#000000"
    property string colorLine: darkMode ? "#ffffff" : "#000000"
    property string colorHighlight: "#918682"

    property string colorLight: darkMode ? "#2E373D" : "#CBCBCB"
    property string colorMedium: darkMode ? "#121519" : "#D4D4D4"
    property string colorDark: darkMode ? "#16191E" : "#ffffff"
    property string colorDarkest: darkMode ? "#0E0F12" : "#0E0F12"

    property string colorGreen: "#19D37B"
    property string colorRed: "#EA003C"

    property string colorSwitch: darkMode ? "#1E242C" : "#B9B9B9"
    property string colorSwitchOn : darkMode ? "#ffffff" : "#ffffff"
    property string colorSwitchBackground: darkMode ? "#000000" : "#ffffff"

    property string colorButton: darkMode ? "#121519" : "#EAEAEA"
    property string colorButtonPressed :darkMode ? "#16191E" : "#D7D7D7"
    property string colorButtonFav: darkMode ? "#1A1D23" : "#1A1D23"

    //    property string colorRoundButton: "#1A1D23"

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // TRANSLATIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property var translations: translationsJson.read()
    property string language: config.language

    JsonFile {
        id: translationsJson
        name: appPath + "/translations.json"
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SOFTWARE UPDATE
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property bool updateAvailable: false
    property real _new_version
    property string updateURL

    Timer {
        repeat: true
        running: true
        interval: 7200000
        triggeredOnStart: true

        onTriggered: {
            if (config.settings.softwareupdate) {
                JSUpdate.checkForUpdate();
            }
        }
    }

    Launcher { id: mainLauncher }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONFIGURATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property var config: jsonConfig.read();
    property var integration: ({}) // holds the integration qmls
    property var integrationObj: []

    property var device: ({}) // holds the standalone device integration qmls

    JsonFile { // this module loads the config file
        id: jsonConfig
        name: configPath + "/config.json"
    }

    // load the hub integrations
    function loadHubIntegrations() {
        var comp;

        for (var i=0; i<config.integration.length; i++) {
            comp = Qt.createComponent("qrc:/integrations/"+ config.integration[i].type +".qml");
            integration[config.integration[i].type] = comp.createObject(applicationWindow, {integrationId: i});
            if (comp.status != Component.Ready) {
                console.debug("Error: " + comp.errorString() );
            }
            integrationObj[i] = config.integration[i];

        }

        // must be at least one integration for this to be successful
        if (i != 0) {
            return true;
        } else {
            return false;
        }
    }

    // load the entities
    function loadEntities() {
        var comp;
        var obj;

        // load the entities from the config file that are supported
        for (var i=0; i<config.entities.length; i++) {
            for (var k=0; k<supported_entities.length; k++) {
                if (supported_entities[k] === config.entities[i].type) {
                    // load the supported component
                    comp = Qt.createComponent("qrc:/components/" + supported_entities[k] + "/Main.qml");
                    if (comp.status != Component.Ready) {
                        console.debug("Error: " + comp.errorString() );
                    }
                    loaded_components[supported_entities[k]] = comp.createObject(applicationWindow);
                    loaded_components[supported_entities[k]].entities = config.entities[i].data;

                    // store which entity type was loaded. Not app supported entities are loaded.
                    loaded_entities.push(supported_entities[k]);
                    loaded_entities_id.push(k)
                }
            }
        }
    }

    Component.onCompleted: {
        // change dark mode to the configured value
        darkMode = Qt.binding(function () { return config.settings.darkmode});
        standbyControl.display_autobrightness = Qt.binding(function() { return config.settings.autobrightness })

        // load the hub integrations
        if (loadHubIntegrations()) {
            // if success, load the entities
            loadEntities();
        }

        // set the language
        translateHandler.selectLanguage(language);

        // when everything is loaded, load the main UI
        loader_main.setSource("qrc:/MainContainer.qml");
        loader_main.active = true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SUPPORTED COMPONENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property var supported_entities: ["light","blind"]
    property var supported_entities_translation: [qsTr("Lights") + translateHandler.emptyString, qsTr("Blinds") + translateHandler.emptyString]

    property var loaded_entities: []  // holds the loaded entities. Not all supported entities are loaded
    property var loaded_entities_id: [] // holds the loaded entity ids

    property var loaded_components: ({}) // holds the loaded component, for example it has the Main.qml file from lights

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SYSTEM VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property bool firstRun: true // tells if the application is running for the first time

    property string connectionState: "connecting"

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STANDBY CONTROL
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias standbyControl: standbyControl

    StandbyControl {
        id: standbyControl

        Component.onCompleted: {
            standbyControl.wifiOffTime = Qt.binding(function () { return config.settings.wifitime});
            standbyControl.shutdownTime = Qt.binding(function () { return config.settings.shutdowntime});
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BUTTON HANDLER
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ButtonHandler{
        id: buttonHandler
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // HAPTIC FEEDBACK HANDLER
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias haptic: haptic

    Haptic {
        id: haptic
        // play haptic effects with: haptic.playEffect("click");
        // available effects: click, bump
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // QML GUI STUFF
    // The main container holds almost all the GUI elements. The secondary container is used to load the buttons into, with their open state.

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // MAIN CONTAINER
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Loader {
        id: loader_main
        asynchronous: true
        visible: false
        width: 480
        height: 800
        x: 0
        y: 0
        active: false
        //        source: "qrc:/MainContainer.qml"

        transform: Scale {
            id: scale
            origin.x: loader_main.width/2
            origin.y: loader_main.height/2
        }

        states: [
            State { name: "hidden"; PropertyChanges {target: loader_main; y: -60; scale: 0.8; opacity: 0.4; enabled: false}},
            State { name: "visible"; PropertyChanges {target: loader_main; scale: 1; opacity: 1; visible: true; enabled: true}}
        ]
        transitions: [
            Transition {to: "hidden"; PropertyAnimation { target: loader_main; properties: "y, scale, opacity"; easing.type: Easing.OutExpo; duration: 800 }},
            Transition {to: "visible"; SequentialAnimation {
                    PropertyAnimation { target: loader_main; properties: "y, scale, opacity"; easing.type: Easing.InExpo; duration: 300 }
                }}
        ]

        onStatusChanged: if (loader_main.status == Loader.Ready) {
                             firstRun = false;
                             loader_main.visible = true;
                             connectionState = "connected";
                         }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SECONDARY CONTAINER
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //    Loader {
    //        id: loader_second
    //        asynchronous: true
    //        visible: false
    //        width: 480
    //        height: 800
    //        x: 0
    //        y: 0

    //        onStatusChanged: if (loader_second.status == Loader.Ready) {
    //                             loader_second.visible = true;
    //                         }
    //    }

    property alias contentWrapper: contentWrapper

    Item {
        id: contentWrapper
        width: 480
        height: 800
        x: 0
        y: 0
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CHARING SCREEN
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Visible when charging

    property alias chargingScreen: chargingScreen
    Loader {
        id: chargingScreen
        width: 480
        height: 800
        x: 0
        y: 0
        asynchronous: true
        source: "qrc:/basic_ui/ChargingScreen.qml"
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // LOW BATTERY POPUP NOTIFICAITON
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Pops up when battery level is under 20%
    onBattery_levelChanged: {
        if (battery_level < 0.2 && !wasBatteryWarning) {
            lowBatteryNotification.item.open();
            wasBatteryWarning = true;
            standbyControl.touchDetected = true;
        }
        if (battery_level > 0.3) {
            wasBatteryWarning = false;
        }
    }

    property alias lowBatteryNotification: lowBatteryNotification

    Loader {
        id: lowBatteryNotification
        width: 480
        height: 800
        x: 0
        y: 0
        asynchronous: true
        source: "qrc:/basic_ui/PopupLowBattery.qml"
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECTION SCREEN
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Visible when connecting, reconnecting to the integration
    BasicUI.ConnectionScreen {}

}
