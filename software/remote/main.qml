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

    signal batteryDataUpdated()

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

            if (battery.getAveragePower() >= 0 ) {
                chargingScreen.item.state = "visible";
            } else {
                chargingScreen.item.state = "hidden";
            }

            // debug
//            console.debug("Battery voltage: " + battery_voltage);
//            console.debug("Battery full charge capacity: " + battery.getFullChargeCapacity());
//            console.debug("Battery full available capacity: " + battery.getFullAvailableCapacity());
//            console.debug("Battery design capacity: " + battery.getDesignCapacity());
        }
    }

    // battery data logger
    Timer {
        running: true
        repeat: true
        interval: 600000

        onTriggered: {
            var tmpA = battery_data;

            var tmp = {};
            tmp.timestamp = new Date();
            tmp.level = battery_level;
            tmp.power = battery.getAveragePower();
            tmp.voltage = battery_voltage;

            tmpA.push(tmp);
            battery_data = tmpA;

            applicationWindow.batteryDataUpdated();
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
    property string colorMedium: darkMode ? "#262626" : "#D4D4D4"
    property string colorDark: darkMode ? "#16191E" : "#ffffff"
    property string colorDarkest: darkMode ? "#0E0F12" : "#0E0F12"

    property string colorGreen: "#19D37B"
    property string colorRed: "#EA003C"

    property string colorSwitch: darkMode ? "#373737" : "#B9B9B9"
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

    onUpdateAvailableChanged: {
        if (updateAvailable) {
            //: Notification text when new software update is available
            //~ "Update" is a label for the button
            addNotification("normal", qsTr("New software version is available!") + translateHandler.emptyString, function() { var command = "/usr/bin/remote/updater.sh " + obj.assets[0].browser_download_url; mainLauncher.launch(command); }, qsTr("Update") + translateHandler.emptyString);
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONFIGURATION
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property var config: jsonConfig.read();
    property var integration: ({}) // holds the integrations

    property var device: ({}) // holds the standalone device integration qmls

    JsonFile { // this module loads the config file
        id: jsonConfig
        name: configPath + "/config.json"
    }

    // load the hub integrations
    function loadHubIntegrations() {
        var comp;

        for (var i=0; i<config.integration.length; i++) {
            integration[config.integration[i].type] = config.integration[i];

            comp = Qt.createComponent("qrc:/integrations/"+ config.integration[i].type +".qml");
            if (comp.status !== Component.Ready) {
                console.debug("Error: " + comp.errorString() );
            }
            integration[config.integration[i].type].obj = comp.createObject(applicationWindow, {integrationId: i});
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
        var tmp = {};

        // load the entities from the config file that are supported
        for (var i=0; i<config.entities.length; i++) {
            for (var k=0; k<supported_entities.length; k++) {
                if (supported_entities[k] === config.entities[i].type) {

                    // load the supported component
                    comp = Qt.createComponent("qrc:/components/" + supported_entities[k] + "/Main.qml");
                    if (comp.status !== Component.Ready) {
                        console.debug("Error: " + comp.errorString() );
                    }
                    loaded_components[supported_entities[k]] = comp.createObject(applicationWindow);
                    loaded_components[supported_entities[k]].entities = config.entities[i].data;

                    // store which entity type was loaded. Not all supported entities are loaded.
                    tmp = {};
                    tmp.obj = supported_entities[k];
                    tmp.id = k;
                    loaded_entities.push(tmp);
                }
            }
        }
    }

    Component.onCompleted: {
        // change dark mode to the configured value
        darkMode = Qt.binding(function () { return config.settings.darkmode});
        standbyControl.display_autobrightness = Qt.binding(function() { return config.settings.autobrightness })
        standbyControl.proximity.proximitySetting = Qt.binding(function() { return config.settings.proximity })

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
    //: names of the entities. Shows up in menu on the bottom. Always plural
    property var supported_entities_translation: [qsTr("Lights") + translateHandler.emptyString, qsTr("Blinds") + translateHandler.emptyString]

    property var loaded_entities: []  // holds the loaded entities. Not all supported entities are loaded

    property var loaded_components: ({}) // holds the loaded component, for example it has the Main.qml file from lights

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SYSTEM VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property bool firstRun: true // tells if the application is running for the first time

    property bool connecting: false // turns on or off the little loader in the status bar

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
        width: 480
        height: 800
        x: 0
        y: 0
        active: false
        state: "visible"

        transform: Scale {
            id: scale
            origin.x: loader_main.width/2
            origin.y: loader_main.height/2
        }

        states: [
            State { name: "hidden"; PropertyChanges {target: loader_main; y: -60; scale: 0.8; opacity: 0.4}},
            State { name: "visible"; PropertyChanges {target: loader_main; scale: 1; opacity: 1}}
        ]
        transitions: [
            Transition {to: "hidden"; PropertyAnimation { target: loader_main; properties: "y, scale, opacity"; easing.type: Easing.OutExpo; duration: 800 }},
            Transition {to: "visible"; PropertyAnimation { target: loader_main; properties: "y, scale, opacity"; easing.type: Easing.OutExpo; duration: 500 }}
        ]

        onStatusChanged: if (loader_main.status == Loader.Ready) {
                             loadingScreen.state = "connected";
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
        if (battery_level < 0.1 && !wasBatteryWarning) {
            lowBatteryNotification.item.open();
            wasBatteryWarning = true;
            standbyControl.touchDetected = true;
        }
        if (battery_level > 0.2) {
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
    // LOADING SCREEN
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BasicUI.LoadingScreen {
        id: loadingScreen
        state: "connecting"
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NOTIFICATIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // { "type": "normal",
    //   "text": "This is the notification text to display",
    //   "action": "function () { return 1; }",
    //   "timestamp" : this is added automatically,
    //   "id": position in the array, assigned automatically
    // }
    //
    // type can be "normal" or "error"

    function addNotification(type, text, action, actionlabel) {
        var json = {};
        json.type = type;
        json.text = text;
        json.action = action;
        json.actionlabel = actionlabel;
        json.timestamp = new Date();

        var tmp = notifications;
        tmp.push(json);
        notifications = tmp;

        // show notification
        var comp = Qt.createComponent("qrc:/basic_ui/Notification.qml");
        notificationObj[notifications.length-1] = comp.createObject(notificationsRow, {type: json.type, text: json.text, actionlabel: json.actionlabel, action: json.action, timestamp: json.timestamp, idN: notifications.length-1, _state: "visible"});
        notificationObj[notifications.length-1].removeNotification.connect(removeNotification);
        notificationObj[notifications.length-1].dismissNotification.connect(dismissNotification);
    }

    function removeNotification(idN) {
        notificationObj[idN].destroy(400);
        var tmp = notifications;
        tmp.splice(notificationObj[idN].idN, 1);
        notifications = tmp;
    }

    function dismissNotification(idN) {
        notificationObj[idN].destroy(400);
    }


    property var notifications: [] // json array that holds all the active notifications
    property var notificationObj: []

    //     close the notification drawer when there is no notification left to show
    onNotificationsChanged: {
        if (notifications.length == 0) {
            notificationsDrawer.close();
        }
    }

    Column {
        id: notificationsRow
        anchors.fill: parent
        spacing: 10
        topPadding: 20
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // NOTIFICATION DRAWER
    //////////////////////////////////////////////////////////////////////////////////////////////////

    Drawer {
        id: notificationsDrawer
        width: parent.width
        height: notifications.length > 5 ? 100 + 5 * 104 : 100 + (notifications.length + 1) * 104
        edge: Qt.TopEdge
        dragMargin: 40
        interactive: loader_main.state == "visible" ? true : false
        dim: false
        opacity: position

        background: Rectangle {
            x: parent.width - 1
            width: parent.width
            height: parent.height
            color: colorBackgroundTransparent
        }

        Rectangle {
            width: parent.width
            height: parent.height - 40
            y: 40
            color: colorBackground
            opacity: notificationsDrawer.position
        }

        onOpened: {
            loader_main.item.mainNavigation.opacity = 0.3
            loader_main.item.mainNavigationSwipeview.opacity = 0.3
        }

        onClosed: {
            loader_main.item.mainNavigation.opacity = 1
            loader_main.item.mainNavigationSwipeview.opacity = 1
        }

        Loader {
            width: parent.width
            height: parent.height

            asynchronous: true
            active: notificationsDrawer.position > 0 ? true : false
            source: notificationsDrawer.position > 0 ? "qrc:/basic_ui/NotificationDrawer.qml" : ""
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STANDBY MODE TOUCHEVENT OVERLAY
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // captures all touch events when in standby mode. Avoids clicking on random buttons when waking up the display
    MouseArea {
        id: touchEventCatcher
        anchors.fill: parent
        enabled: true
        pressAndHoldInterval: 5000

        onPressAndHold: {
            console.debug("Disabling touch even catcher");
            touchEventCatcher.enabled = false;
        }
    }

    Timer {
        running: standbyControl.mode == "standby" || standbyControl.mode == "on" ? true : false
        repeat: false
        interval: 200

        onTriggered: {
            if (standbyControl.mode == "on") {
                touchEventCatcher.enabled = false;
            }
            if (standbyControl.mode == "standby") {
                touchEventCatcher.enabled = true;
            }
        }
    }
}
