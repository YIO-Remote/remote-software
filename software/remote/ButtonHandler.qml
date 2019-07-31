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
                standbyControl.proximity.readInterrupt()
            } else {
                if (!wasPressed) {
                    wasPressed = true;
                    buttonName = interruptHandler.button;
                    standbyControl.buttonPressDetected = true;
                    buttonPress(buttonName);
                } else if (wasPressed){
                    wasPressed = false;
                    standbyControl.buttonPressDetected = true;
                    buttonRelease(buttonName);
                }
            }
        }
    }
}
