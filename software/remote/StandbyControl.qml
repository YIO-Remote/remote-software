import QtQuick 2.11

import DisplayControl 1.0
import TouchEventFilter 1.0
import Proximity 1.0
import Launcher 1.0

import "qrc:/scripts/helper.js" as JSHelper

Item {
    id: standbyControl
    property bool proximityDetected: false
    property bool touchDetected: false
    property bool buttonPressDetected: false

    property string mode: "on" // on, dim, standby, wifi_off

    property int displayDimTime: 20 // seconds
    property int standbyTime: 30 // seconds
    property int wifiOffTime: 0 // seconds
    property int shutdownTime: 0 // seconds

    property int display_brightness: 100
    property int display_brightness_old: 100
    property bool display_autobrightness
    property int display_brightness_ambient: 100
    property int display_brightness_set: 100

    property double onStartTime: new Date().getTime()
    property double standbyStartTime: new Date().getTime()
    property double screenOnTime: 0
    property double screenOffTime: 0

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // TOUCH EVENT DETECTOR
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TouchEventFilter {
        id: touchFilter
        source: applicationWindow

        onDetectedChanged: {
            touchDetected = true;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // PROXIMITY SENSOR APDS9960
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias proximity: proximity

    Proximity {
        id: proximity

        onProximityEvent: {
            standbyControl.proximityDetected = true;
        }
    }

    Timer {
        id: ambientLightReadTimer
        running: false
        repeat: false
        interval: 400

        onTriggered: {
            standbyControl.display_brightness_ambient = JSHelper.mapValues(proximity.readAmbientLight(),0,30,15,100);
            // set the display brightness
            if (standbyControl.display_autobrightness) {
                setBrightness(display_brightness_ambient);
            } else {
                setBrightness(display_brightness_set);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STANDBY CONTROL
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias displayControl: displayControl
    DisplayControl {
        id: displayControl
    }

    function setBrightness(brightness) {
        displayControl.setBrightness(display_brightness_old, brightness);
        display_brightness_old = brightness;
        display_brightness = brightness;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Launcher { id: standbyLauncher }
    Launcher { id: wifiLauncher }

    function wifiHandler(state) {
        var cmd;

        if (state == "on") {
            cmd = "systemctl start wpa_supplicant@wlan0.service"
            wifiLauncher.launch(cmd);
        } else {
            cmd = "systemctl stop wpa_supplicant@wlan0.service"
            wifiLauncher.launch(cmd);
        }
    }

    function wakeUp() {
        // get battery readings
        //        battery.checkBattery();

        switch (mode) {

        case "dim":
            // set the display brightness
            ambientLightReadTimer.start();

            // set the mode
            mode = "on";
            break;

        case "standby":
            // turn off standby
            if (displayControl.setmode("standbyoff")) {
                standbyoffDelay.start();
            }

            // set the mode
            mode = "on";
            break;

        case "wifi_off":
            wifiHandler("on")

            // turn off standby
            if (displayControl.setmode("standbyoff")) {
                standbyoffDelay.start();
            }

            // set the mode
            mode = "on";

            // integration socket on
            for (var i=0; i<config.integration.length; i++) {
                integration[config.integration[i].type].obj.connect();
            }

            // turn on API
            api.start();

            break;
        }

        // reset elapsed time
        standbyBaseTime = new Date().getTime()

        // start bluetooth scanning
        if (config.settings.bluetootharea) bluetoothArea.startScan();
    }

    Timer {
        id: standbyoffDelay
        repeat: false
        running: false
        interval: 300

        onTriggered: {
            ambientLightReadTimer.start();
        }
    }

    onTouchDetectedChanged: {
        // if there was a touch event, reset the timers
        if (touchDetected) {
            wakeUp();
            touchDetected = false;
            proximity.proximityDetection(false);
        }
    }

    onProximityDetectedChanged: {
        // if there was a proximity event, reset the timers
        if (proximityDetected) {
            wakeUp();
            proximityDetected = false;
        }
    }

    onButtonPressDetectedChanged: {
        // if there was a button press event, reset the timers
        if (buttonPressDetected) {
            wakeUp();
            buttonPressDetected = false;
            proximity.proximityDetection(false);
        }
    }

    onModeChanged: {
        console.debug("Mode: " + mode);
        // if mode is on change processor to ondemand
        if (mode == "on") {
            standbyLauncher.launch("/usr/bin/yio-remote/ondemand.sh");

            // start screen on timer and calculate off time
            onStartTime = new Date().getTime()
            screenOffTime += new Date().getTime() - standbyStartTime
        }
        // if mode is standby change processor to powersave
        if (mode == "standby") {
            standbyLauncher.launch("/usr/bin/yio-remote/powersave.sh");

            // start standby timer and calculate on time
            standbyStartTime = new Date().getTime()
            screenOnTime += new Date().getTime() - onStartTime
        }
    }

    // standby timer
    property var standbyBaseTime
    property alias standbyTimer: standbyTimer

    Timer {
        id: standbyTimer
        repeat: true
        running: false
        interval: 1000

        onTriggered: {
            var time = new Date().getTime()

            // mode = dim
            if (time-standbyBaseTime > displayDimTime * 1000 && mode == "on") {
                // dim the display
                setBrightness(10);
                mode = "dim";
            }

            // mode = standby
            if (time-standbyBaseTime > standbyTime * 1000 && mode == "dim") {
                // turn on proximity detection
                proximity.proximityDetection(true);

                // turn off the backlight
                setBrightness(0);

                // put the display to standby mode
                displayControl.setmode("standbyon");

                // stop bluetooth scanning
                if (config.settings.bluetootharea) bluetoothArea.stopScan();

                mode = "standby";

                // reset battery charging screen
                chargingScreen.item.resetClock.start();
            }

            // bluetooth turn off
            if (time-standbyBaseTime > (standbyTime+20)* 1000 && config.settings.bluetootharea) {
                // turn off bluetooth
                bluetoothArea.turnOff()
            }

            // mode = wifi_off
            if (time-standbyBaseTime > wifiOffTime * 1000 && wifiOffTime != 0 && mode == "standby" && battery_averagepower <= 0) {
                // integration socket off
                for (var i=0; i<config.integration.length; i++) {
                    integration[config.integration[i].type].obj.disconnect();
                }
                // turn off API
                api.stop();

                // turn off wifi
                wifiHandler("off")

                mode = "wifi_off";
            }

            // mode = shutdown
            if (time-standbyBaseTime > shutdownTime * 1000 && shutdownTime != 0 && (mode == "standby" || mode =="wifi_off") && battery_averagepower <= 0) {
                loadingScreen.source = "qrc:/basic_ui/ClosingScreen.qml";
                loadingScreen.active = true;
            }
        }
    }

    Timer {
        running: true
        repeat: false
        interval: 20000

        onTriggered: {
            standbyBaseTime = new Date().getTime()
            if (loader_main.source != "qrc:/wifiSetup.qml") {
                standbyTimer.start()
            }
        }
    }
}
