import QtQuick 2.0

import InterruptHandler 1.0

Item {

    property bool wasPressed: false;

    InterruptHandler {
        id: interruptHandler

        onButtonPressed: {
            if (!wasPressed) {
                wasPressed = true;
                buttonTimeout.start()

                standbyControl.buttonPressDetected = true;
                console.debug(interruptHandler.button)

                if (standbyControl.state == "standby") {
                    // do nothing
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
