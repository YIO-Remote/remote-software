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
                if (!wasPressed) {
                    buttonName = interruptHandler.button;
                    buttonPress(buttonName);
                    wasPressed = true;
                    standbyControl.buttonPressDetected = true;

                } else if (wasPressed){
                    if (buttonName != interruptHandler.button) {
                        // if it's not the same buttn, then release the old one
                        buttonRelease(buttonName);

                        // and create a press event for the new one
                        buttonName = interruptHandler.button;
                        buttonPress(buttonName);
                        standbyControl.buttonPressDetected = true;

                    } else {
                        buttonRelease(buttonName);
                        wasPressed = false;
                    }
                    standbyControl.buttonPressDetected = true;
                }
            }
        }
    }
}
