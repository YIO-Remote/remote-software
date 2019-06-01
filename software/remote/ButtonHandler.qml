import QtQuick 2.0

import InterruptHandler 1.0

Item {

    property bool wasPressed: false;
    property alias interruptHandler: interruptHandler

    signal buttonPress(string button)

    InterruptHandler {
        id: interruptHandler

        onButtonPressed: {
            if (!wasPressed) {
                wasPressed = true;
                buttonTimeout.start()

                if (interruptHandler.button == "apds9960") {
                    standbyControl.proximity.readInterrupt()
                } else {
                    standbyControl.buttonPressDetected = true;
                    buttonPress(interruptHandler.button);
                }
            }
        }
    }

    // timer is needed to avoid false double clicks
    Timer {
        id: buttonTimeout
        repeat: false
        interval: 200
        running: false

        onTriggered: {
            wasPressed = false;
        }
    }
}
