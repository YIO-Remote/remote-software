import QtQuick 2.11

import DisplayControl 1.0
import TouchEventFilter 1.0
import Proximity 1.0

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

    property double startTime: new Date().getTime()
    property double baseTime: new Date().getTime()
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

        onApds9960Notify: {
            console.debug(proximity.apds9960Error);
            applicationWindow.addNotification("error", proximity.apds9960Error, "", "Restart");
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
    function wifiHandler(state) {
        var cmd;

        if (state == "on") {
            cmd = "systemctl start wpa_supplicant@wlan0.service"
        } else {
            cmd = "systemctl stop wpa_supplicant@wlan0.service"
        }
        mainLauncher.launch(cmd);
    }

    function wakeUp() {
        switch (mode) {
        case "on":
            // reset timers
            secondsPassed = 0;
            break;

        case "dim":
            // set the display brightness
            ambientLightReadTimer.start();

            // set the mode
            mode = "on";

            // reset timers
            secondsPassed = 0;
            break;

        case "standby":
            // turn off standby
            if (displayControl.setmode("standbyoff")) {
                standbyoffDelay.start();
            }

            // set the mode
            mode = "on";

            // reset timers
            secondsPassed = 0;
            break;

        case "wifi_off":
            wifiHandler("on")
            // integration socket on
            for (var i=0; i<config.integration.length; i++) {
                integration[config.integration[i].type].obj.connect();
            }
            // turn off standby
            if (displayControl.setmode("standbyoff")) {
                standbyoffDelay.start();
            }

            // set the mode
            mode = "on";

            // reset timers
            secondsPassed = 0;
            break;
        }
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
            mainLauncher.launch("/usr/bin/yio-remote/ondemand.sh");
            startTime = new Date().getTime()
        }
        // if mode is standby change processor to powersave
        if (mode == "standby") {
            mainLauncher.launch("/usr/bin/yio-remote/powersave.sh");

            // add screen on time
            screenOnTime += new Date().getTime() - startTime
            screenOffTime = new Date().getTime() - baseTime - screenOnTime
        }
    }

    // standby timer
    property int secondsPassed: 0
    Timer {
        id: standbyTimer
        repeat: true
        running: true
        interval: 1000

        onTriggered: {
            secondsPassed += 1000;

            // mode = dim
            if (secondsPassed == displayDimTime * 1000) {
                // dim the display
                setBrightness(10);
                mode = "dim";
            }

            // mode = standby
            if (secondsPassed == standbyTime * 1000) {
                // turn on proximity detection
                proximity.proximityDetection(true);

                // turn off the backlight
                setBrightness(0);

                // put the display to standby mode
                displayControl.setmode("standbyon");
                mode = "standby";
            }

            // mode = wifi_off
            if (secondsPassed == wifiOffTime * 1000) {
                // integration socket off
                for (var i=0; i<config.integration.length; i++) {
                    integration[config.integration[i].type].obj.disconnect();
                }
                // turn off wifi
                wifiHandler("off")

                mode = "wifi_off";
            }

            // mode = shutdown
            if (secondsPassed == shutdownTime * 1000) {
                // set turn on button to low
                buttonHandler.interruptHandler.shutdown();
                // halt
                mainLauncher.launch("halt");
            }
        }
    }
}
