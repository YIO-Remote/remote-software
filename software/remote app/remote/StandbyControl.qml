import QtQuick 2.11

Item {
    property bool proximityDetected: false
    property bool touchDetected: false
    property bool buttonPressDetected: false

    property string mode: "on" // on, dim, standby, wifi_off

    property int displayDimTime: 20 // seconds
    property int standbyTime: 30 // seconds
    property int wifiOffTime: 600 // seconds
    property int shutdownTime: 5400 // seconds

    property int display_brightness: 100
    property int display_brightness_old: 100
    property bool display_autobrightness: true
    property int display_brightness_ambient: 100
    property int display_brightness_set: 100

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
        var cmd = "/usr/bin/remote/display_brightness.sh " + display_brightness_old + " " + display_brightness;
        mainLauncher.launch(cmd);
        display_brightness_old = display_brightness;
    }

    onTouchDetectedChanged: {
        // if there was a touch event, reset the timers
        if (touchDetected) {
            // if mode standby then turn on the display
            if (mode == "standby") {
                //
            }
            // if mode wifi_off then turn on wifi and display
            if (mode == "wifi_off") {
                wifiHandler("on")
                // integration socket on
                for (var i=0; i<config.integration.length; i++) {
                    integration[config.integration[i].type].connectionOpen = true;
                }
            }
            // set the display brightness
            if (display_autobrightness) {
                display_brightness = display_brightness_ambient;
            } else {
                display_brightness = display_brightness_set;
            }
            mode = "on";
            displayDimTimer.restart();
            standbyTimer.stop();
            wifiOffTimer.restart();
            shutdownTimer.restart();
            touchDetected = false;
        }
    }

    onProximityDetectedChanged: {
        // if there was a proximity event, reset the timers
        if (proximityDetected) {
            // if mode standby then turn on the display
            if (mode == "standby") {
                //
            }

            // if mode wifi_off then turn on wifi and display
            if (mode == "wifi_off") {
                wifiHandler("on")
                // integration socket on
                for (var i=0; i<config.integration.length; i++) {
                    integration[config.integration[i].type].connectionOpen = true;
                }
            }
            // set the display brightness
            if (display_autobrightness) {
                display_brightness = display_brightness_ambient;
            } else {
                display_brightness = display_brightness_set;
            }
            mode = "on";
            displayDimTimer.restart();
            standbyTimer.stop();
            wifiOffTimer.restart();
            shutdownTimer.restart();
            proximityDetected = false;
        }
    }

    onButtonPressDetectedChanged: {
        // if there was a button press event, reset the timers
        if (buttonPressDetected) {
            // if mode standby then turn on the display
            if (mode == "standby") {
                //
            }

            // if mode wifi_off then turn on wifi and display
            if (mode == "wifi_off") {
                wifiHandler("on")
                // integration socket on
                for (var i=0; i<config.integration.length; i++) {
                    integration[config.integration[i].type].connectionOpen = true;
                }
            }

            // set the display brightness
            if (display_autobrightness) {
                display_brightness = display_brightness_ambient;
            } else {
                display_brightness = display_brightness_set;
            }
            mode = "on";
            displayDimTimer.restart();
            standbyTimer.stop();
            wifiOffTimer.restart();
            shutdownTimer.restart();
            buttonPressDetected = false;
        }
    }

    onModeChanged: {
        // if mode is on change processor to ondemand
        if (mode == "on") {
            var cmd = "echo -e ondemand > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
            mainLauncher.launch(cmd);
        }
        // if mode is standby change processor to powersave
        if (mode == "standby") {
            cmd = "echo -e powersave > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
            mainLauncher.launch(cmd);
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
                console.debug("Dim the display");
                // set brightness to 20
                display_brightness = 20;
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
                console.debug("Standby the display");
                // turn off gesture detection
                socketServer.clientId.sendTextMessage("gesture off");
                socketServer.clientId.sendTextMessage("proximity detect on");
                // put display to standby
                //
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
                console.debug("Wifi off");
                // turn off wifi
                wifiHandler("off")
                // integration socket off
                for (var i=0; i<config.integration.length; i++) {
                    integration[config.integration[i].type].connectionOpen = false;
                }
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

        onTriggered: {
            shutdownTimer.stop();
            console.debug("Shutdown");
            // halt
            mainLauncher.launch("halt");
        }
    }
}
