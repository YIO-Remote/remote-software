/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

import QtQuick 2.11

import InterruptHandler 1.0

Item {

    property bool wasPressed: false
    property alias interruptHandler: interruptHandler

    signal buttonPress(string button)
    signal buttonRelease(string button)

    // Simulation via YIO API
    Connections {
        target: api
        enabled: true

        onButtonPressed: {
            buttonPress(button);
        }
        onButtonReleased: {
            buttonRelease(button);
        }
    }

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
