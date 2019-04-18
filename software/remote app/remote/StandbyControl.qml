import QtQuick 2.11

Item {
    property bool proximityDetected: true
    property bool touchDetected: false
    property bool buttonPressDetected: false

    property string mode: "on" // on, dim, standby, wifi_off

    property int displayDimTime: 10 // seconds
    property int standbyTime: 20 // seconds
    property int wifiOffTime: 600 // seconds
    property int shutdownTime: 5400 // seconds

    onTouchDetectedChanged: {
        // if there was a touch event, reset the timers
        if (touchDetected) {
            // if mode standby then turn on the display
            if (mode == "standby") {
                //
            }
            // if mode wifi_off then turn on wifi and display
            if (mode == "wifi_off") {
                socketServer.clientId.sendTextMessage("wifi on");
            }
            // set the display brightness
            //
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
                socketServer.clientId.sendTextMessage("wifi on");
            }
            // set the display brightness
            //
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
            // if mode wifi_off then turn on wifi and display
            if (mode == "wifi_off") {
                socketServer.clientId.sendTextMessage("wifi on");
            }

            // set the display brightness
            //
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
            socketServer.clientId.sendTextMessage("ondemand");
        }
        // if mode is standby change processor to powersave
        if (mode == "standby") {
            socketServer.clientId.sendTextMessage("powersave");
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
                //
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
                socketServer.clientId.sendTextMessage("wifi off");
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
            //
        }
    }
}
