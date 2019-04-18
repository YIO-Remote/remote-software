import QtQuick 2.0

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
            mode = "on";
            displayDimTimer.restart();
            standbyTimer.stop();
            wifiOffTimer.restart();
            shutdownTimer.restart();
        }
    }

    onProximityDetectedChanged: {
        // if there was a proximity event, reset the timers
        if (proximityDetected) {
            mode = "on";
            displayDimTimer.restart();
            standbyTimer.stop();
            wifiOffTimer.restart();
            shutdownTimer.restart();
        }
    }

    onButtonPressDetectedChanged: {
        // if there was a button press event, reset the timers
        if (buttonPressDetected) {
            mode = "on";
            displayDimTimer.restart();
            standbyTimer.stop();
            wifiOffTimer.restart();
            shutdownTimer.restart();
        }
    }

    // dim timer
    Timer {
        id: displayDimTimer
        repeat: false
        running: true
        interval: displayDimTime * 1000

        onTriggered: {
            if (proximityDetected && mode != "dim") {
                displayDimTimer.stop();
                console.debug("Dim the display");
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
        }
    }
}
