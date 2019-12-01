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

    property int screenOnTime: 0
    property int screenOffTime: 0

    signal standByOn()
    signal standByOff()

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

    function getCurrentTime(){
        const timeSizeReduction = 1570881231088;
        return (new Date().getTime() - timeSizeReduction);
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
                wifi.on()

                // turn off standby
                if (displayControl.setmode("standbyoff")) {
                    standbyoffDelay.start();
                }

                // set the mode
                mode = "on";

                // integration socket on
                for (var i=0; i<integrations.list.length; i++) {
                    integrations.list[i].connect();
                }

                // turn on API
                api.start();

                break;
        }

        // reset elapsed time
        standbyBaseTime = getCurrentTime();

        // start bluetooth scanning
        if (config.settings.bluetootharea) bluetoothArea.startScan();

        // reset battery charging screen
        chargingScreen.item.showClock.start();
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
        console.debug("Mode changed: " + mode);
        // TODO create a device class for power saving instead of launching hard coded shell scripts from QML
        // if mode is on change processor to ondemand
        if (mode === "on") {
            standbyLauncher.launch("/usr/bin/yio-remote/ondemand.sh");
            standByOff();
        }
        // if mode is standby change processor to powersave
        if (mode === "standby") {
            standbyLauncher.launch("/usr/bin/yio-remote/powersave.sh");
            standByOn();
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
            let time = getCurrentTime();


            if (mode == "on" || mode == "dim"){
                screenOnTime += 1000;
            }
            if (mode == "standby" || mode == "wifi_off"){
                screenOffTime += 1000;
            }


            // mode = dim
            if (time - standbyBaseTime > displayDimTime * 1000 && mode == "on") {
                // dim the display
                setBrightness(10);
                mode = "dim";
            }

            // mode = standby
            if (time - standbyBaseTime > standbyTime * 1000 && mode == "dim") {
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
                for (var i=0; i<integrations.list.length; i++) {
                    integrations.list[i].disconnect();
                }
                // turn off API
                api.stop();

                // turn off wifi
                wifi.off()

                mode = "wifi_off";
            }

            // mode = shutdown
            if (time-standbyBaseTime > shutdownTime * 1000 && shutdownTime != 0 && (mode == "standby" || mode =="wifi_off") && battery_averagepower <= 0) {
                logger.write("TIMER SHUTDOWN initiated");
                logger.write("time variable: " + time);
                logger.write("standbyBaseTime variable: " + standbyBaseTime);
                logger.write("shutdownTime variable:" + shutdownTime);
                logger.write("battery_averagepower: " + battery_averagepower);

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
            standbyBaseTime = getCurrentTime();
            if (loader_main.source != "qrc:/wifiSetup.qml") {
                standbyTimer.start()
            }
        }
    }
}
