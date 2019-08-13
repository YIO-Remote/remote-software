import QtQuick 2.11
import QtQuick.Controls 2.5

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
    property int  battery_design_capacity: 0
    property int  battery_remaining_capacity: 0
    property int  battery_averagepower: 0
    property int  battery_averagecurrent: 0
    property bool wasBatteryWarning: false

    property var battery_data: []

    signal batteryDataUpdated()

    Battery {
        id: battery
        capacity: 2500

        Component.onCompleted: {
            battery.begin();
        }

        function checkBattery() {
            // read battery data
            battery_voltage = battery.getVoltage() / 1000
            battery_level = battery.getStateOfCharge() / 100
            battery_health = battery.getStateOfHealth()
            battery_design_capacity = battery.getDesignCapacity()
            battery_remaining_capacity = battery.getRemainingCapacity()
            battery_averagepower = battery.getAveragePower()
            battery_averagecurrent = battery.getAverageCurrent()

            // if the designcapacity is off correct it
            if (battery_design_capacity != battery.capacity) {
                console.debug("Design capacity doesn't match. Recalibrating battery.");
                battery.changeCapacity(battery.capacity);
            }

            // if voltage is too low and we are sourcing power, turn off the remote after timeout
            if (0 < battery_voltage && battery_voltage <= 3.4 && battery_averagepower < 0) {
                shutdownDelayTimer.start();
            }

            // hide and show the charging screen
            if (battery_averagepower >= 0) {
                chargingScreen.item.state = "visible";
                // cancel shutdown when started charging
                if (shutdownDelayTimer.running) {
                    shutdownDelayTimer.stop();
                }
            } else {
                chargingScreen.item.state = "hidden";
            }

            console.debug("Average power:" + battery_averagepower + "mW");
            console.debug("Average current:" + battery_averagecurrent + "mA");
        }
    }

    Timer {
        id: shutdownDelayTimer
        running: false
        repeat: false
        interval: 20000

        onTriggered: {
            loadingScreen.source = "qrc:/basic_ui/ClosingScreen.qml";
            loadingScreen.active = true;
        }
    }

    // battery data logger
    Timer {
        running: true
        repeat: true
        interval: 600000

        onTriggered: {
            if (battery_data.length > 35) {
                battery_data.splice(0, 1);
            }

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

    property string colorGreen: "#19D37B"
    property string colorRed: "#EA003C"

    property string colorBackground: darkMode ? "#000000" : "#ffffff"
    property string colorBackgroundTransparent: darkMode ? "#00000000" :  "#00000000"

    property string colorText: darkMode ? "#ffffff" : "#000000"
    property string colorLine: darkMode ? "#ffffff" : "#000000"

    property string colorHighlight1: "#918682"
    property string colorHighlight2: "#313247"

    property string colorLight: darkMode ? "#484848" : "#CBCBCB"
    property string colorMedium: darkMode ? "#282828" : "#D4D4D4"
    property string colorDark: darkMode ? "#1C1C1C" : "#ffffff"

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
        interval: 3600000
        triggeredOnStart: true

        onTriggered: {
            if (config.settings.softwareupdate) {
                JSUpdate.checkForUpdate();

                if (updateAvailable) {
                    var hour = new Date().getHours();
                    if (hour >= 3 && hour <= 5) {
                        fileio.write("/usr/bin/updateURL", updateURL);
                        mainLauncher.launch("systemctl restart update.service");
                        Qt.quit();
                    }
                }
            }
        }
    }

    Launcher { id: mainLauncher }

    onUpdateAvailableChanged: {
        if (updateAvailable) {
            //: Notification text when new software update is available
            //~ "Update" is a label for the button
            notifications.add(qsTr("New software version is available!") + translateHandler.emptyString);
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

            // if plugin integration exists, load that
            if (config.integration[i].plugin) {
                comp = mainLauncher.loadIntegration(appPath, config.integration[i].plugin, i, config.integration[i], entities, notifications);
                integration[config.integration[i].type].obj = comp;

                // otherwise load qml based integration
            } else {
                comp = Qt.createComponent("qrc:/integrations/"+ config.integration[i].type + "/" + config.integration[i].type +".qml");
                if (comp.status !== Component.Ready) {
                    console.debug("Error: " + comp.errorString() );
                }
                integration[config.integration[i].type].obj = comp.createObject(applicationWindow, {integrationId: i});
            }
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
            for (var k=0; k<entities.supported_entities.length; k++) {
                if (config.entities[i].type == entities.supported_entities[k]) {
                    for (var j=0; j < config.entities[i].data.length; j++) {
                        const en = config.entities[i].data[j];
                        entities.add(en, integration[en.integration].obj);
                    }

                    // store which entity type was loaded. Not all supported entities are loaded.
                    //                    loaded_entities.push({ obj: entities.supported_entities[k], id : k });
                    entities.addLoadedEntity(entities.supported_entities[k]);
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
        //        loader_main.active = true;

        // load bluetooth
        bluetoothArea.init(config);
        bluetoothArea.startScan();
    }

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
                             loadingScreen.item.state = "loaded";
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
    // VOLUME
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias volume: volume

    BasicUI.Volume {
        id: volume
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
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
    // NOTIFICATIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    function showNotification(data) {
        var comp = Qt.createComponent("qrc:/basic_ui/Notification.qml");
        var obj = comp.createObject(notificationsRow, {type: data.error, text: data.text, actionlabel: data.actionlabel, action: data.action, timestamp: data.timestamp, idN: data.id, _state: "visible"});
    }

    Column {
        objectName: "notificationsRow"
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
        height: notifications.list.length > 5 ? 100 + 5 * 104 : 100 + (notifications.list.length + 1) * 104
        edge: Qt.TopEdge
        dragMargin: 20
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

        property int n: notifications.list.length

        onNChanged: {
            if (n==0) {
                notificationsDrawer.close();
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // LOADING SCREEN
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias loadingScreen: loadingScreen
    Loader {
        id: loadingScreen
        width: parent.width
        height: parent.height

        asynchronous: true
        active: true
        source: "qrc:/basic_ui/LoadingScreen.qml"

        onSourceChanged: {
            if (source == "") {
                console.debug("Now load the rest off stuff");
                battery.checkBattery();
            }
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
