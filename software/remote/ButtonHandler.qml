import QtQuick 2.11

import InterruptHandler 1.0

Item {

    property bool wasPressed: false
    property alias interruptHandler: interruptHandler

    signal buttonPress(string button)
    signal buttonRelease(string button)

    InterruptHandler {
        id: interruptHandler

        property string buttonName

        onButtonPressed: {
            if (interruptHandler.button == "apds9960") {
                standbyControl.proximity.readInterrupt();
            } else if (interruptHandler.button == "battery") {
                battery.checkBattery();
            } else {
                console.debug(interruptHandler.button);
                if (!wasPressed) {
                    buttonName = interruptHandler.button;
                    buttonPress(buttonName);
                    console.debug("Press: " + buttonName);
                    wasPressed = true;
                    standbyControl.buttonPressDetected = true;

                } else if (wasPressed){
                    if (buttonName != interruptHandler.button) {
                        buttonRelease(interruptHandler.button);
                        buttonRelease(buttonName);
                        console.debug("Release: " + interruptHandler.button + " & " + buttonName);
                    } else {
                        buttonRelease(buttonName);
                        console.debug("Release: " + buttonName);
                    }
                    wasPressed = false;
                    standbyControl.buttonPressDetected = true;
                }
            }
        }
    }
}
