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

                if (standbyControl.mode == "on") {
                    // do button stuff here
                    /////////////////////////////////////////////////////////////////////////////////////////////////
                    // MAIN SCREEN
                    /////////////////////////////////////////////////////////////////////////////////////////////////
                    if (loader_main.state == "visible") {
                        switch (interruptHandler.button) {
                        case "dpad right":
                                loader_main.item.mainNavigationSwipeview.currentIndex += 1;
                            break;
                        case "dpad left":
                                loader_main.item.mainNavigationSwipeview.currentIndex -= 1;
                            break;
                        }
                    }

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
