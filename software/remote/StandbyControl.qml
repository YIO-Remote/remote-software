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
    property int standbyTime: 40 // seconds
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
//            standbyControl.display_brightness_ambient = JSHelper.mapValues(ambientLight,0,450,15,100);
            // read the ambient light a bit later, so your hand doesn't cover the sensor
//            ambientLightReadTimer.start();
        }

        onApds9960Notify: {
            console.debug(proximity.apds9960Error);
            applicationWindow.addNotification("error", proximity.apds9960Error, "", "Restart");

        }

//        onGestureEvent: {
//            console.debug(proximity.gesture);
//        }
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
                standbyControl.display_brightness = standbyControl.display_brightness_ambient;
            } else {
                standbyControl.display_brightness = standbyControl.display_brightness_set;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // STANDBY CONTROL
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    DisplayControl {
        id: displayControl
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

    // change the display brightness
    onDisplay_brightnessChanged: {
        if (display_brightness_old >= display_brightness) {
            // dim down
            for (var i=display_brightness_old; i>display_brightness-1; i--) {
                displayControl.setBrightness(i);
            }
        } else {
            // dim up
            for (var j=display_brightness_old; j<display_brightness+1; j++) {
                displayControl.setBrightness(j);
            }
        }
        standbyControl.display_brightness_old = standbyControl.display_brightness;
    }

    function wakeUp() {
        switch (mode) {
        case "on":
            // reset timers
            displayDimTimer.restart();
            standbyTimer.stop();
            if (wifiOffTime != 0) {
                wifiOffTimer.restart();
            }
            if (shutdownTime != 0) {
                shutdownTimer.restart();
            }
            break;

        case "dim":
            // set the display brightness
            ambientLightReadTimer.start();
//            if (standbyControl.display_autobrightness) {
//                standbyControl.display_brightness = standbyControl.display_brightness_ambient;
//            } else {
//                standbyControl.display_brightness = standbyControl.display_brightness_set;
//            }

            // set the mode
            mode = "on";

            // reset timers
            displayDimTimer.restart();
            standbyTimer.stop();
            if (wifiOffTime != 0) {
                wifiOffTimer.restart();
            }
            if (shutdownTime != 0) {
                shutdownTimer.restart();
            }
            break;

        case "standby":
            // turn off standby
            if (displayControl.setmode("standbyoff")) {
                standbyoffDelay.start();
            }

            // set the mode
            mode = "on";

            // reset timers
            displayDimTimer.restart();
            standbyTimer.stop();
            if (wifiOffTime != 0) {
                wifiOffTimer.restart();
            }
            if (shutdownTime != 0) {
                shutdownTimer.restart();
            }
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
            displayDimTimer.restart();
            standbyTimer.stop();
            if (wifiOffTime != 0) {
                wifiOffTimer.restart();
            }
            if (shutdownTime != 0) {
                shutdownTimer.restart();
            }
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
//            if (standbyControl.display_autobrightness) {
//                standbyControl.display_brightness = standbyControl.display_brightness_ambient;
//            } else {
//                standbyControl.display_brightness = standbyControl.display_brightness_set;
//            }
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
        // if mode is on change processor to ondemand
        if (mode == "on") {
            var cmd = "echo -e ondemand > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
            mainLauncher.launch(cmd);
            startTime = new Date().getTime()
        }
        // if mode is standby change processor to powersave
        if (mode == "standby") {
            cmd = "echo -e powersave > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
            mainLauncher.launch(cmd);
            // add screen on time
            screenOnTime += new Date().getTime() - startTime
            screenOffTime = new Date().getTime() - baseTime - screenOnTime
        }
    }

    // dim timer
    Timer {
        id: displayDimTimer
        repeat: false
        running: true
        interval: displayDimTime * 1000

        onTriggered: {
            if (mode != "dim") {
                displayDimTimer.stop();
                // set brightness to 20
                standbyControl.display_brightness = 10;
                mode = "dim";
                standbyTimer.start();
            }
        }
    }

    // standby timer
    Timer {
        id: standbyTimer
        repeat: false
        running: false
        interval: (standbyTime - displayDimTime) * 1000

        onTriggered: {
            if (mode == "dim") {
                standbyTimer.stop()
                // turn off gesture detection
//                proximity.gestureDetection(false);
                proximity.proximityDetection(true);
                // turn off the backlight
                display_brightness = 0;
                // put display to standby
                displayControl.setmode("standbyon");
                mode = "standby";
            }
        }
    }

    // wifi_off timer
    Timer {
        id: wifiOffTimer
        repeat: false
        running: wifiOffTime != 0 ? true : false
        interval: wifiOffTime * 1000

        onTriggered: {
            if (mode == "standby") {
                wifiOffTimer.stop();
                // integration socket off
                for (var i=0; i<config.integration.length; i++) {
                    integration[config.integration[i].type].obj.disconnect();
                }
                // turn off wifi
                wifiHandler("off")

                mode = "wifi_off";
            }
        }
    }

    // shutdown timer
    Timer {
        id: shutdownTimer
        repeat: false
        running: shutdownTime != 0 ? true : false
        interval: shutdownTime * 1000
        triggeredOnStart: false

        onTriggered: {
            shutdownTimer.stop();
            // set turn on button to low
            buttonHandler.interruptHandler.shutdown();
            // halt
            mainLauncher.launch("halt");
        }
    }
}
