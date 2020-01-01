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
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

// FIXME rewrite using MVC pattern.
// TODO direct wpa_supplicant integration instead of shell scripts
// see: https://github.com/YIO-Remote/remote-software/issues/322
Item {
    width: 480
    height: 800
    visible: true

    property string msg

    SwipeView {
        id: swipeView
        width: 480
        height: 800
        interactive: false
        currentIndex: 0

        Item {
            width: 480
            height: 800

            Text {
                id: titleText
                color: Style.colorText
                text: qsTr("Wi-Fi setup") + translateHandler.emptyString
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    top: parent.top
                    topMargin: 210
                    horizontalCenter: parent.horizontalCenter
                }
                font.family: "Open Sans"
                font.weight: Font.Normal
                font.pixelSize: 60
                lineHeight: 1
            }

            Text {
                id: step1
                color: Style.colorText
                opacity: 0.5
                // TODO read apssid from WifiControl instance once implemented
                text: qsTr("Connect to the Wi-Fi network\n") + fileio.read("/apssid").trim() + translateHandler.emptyString
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    top: titleText.bottom
                    topMargin: 10
                    horizontalCenter: parent.horizontalCenter
                }
                font.family: "Open Sans"
                font.weight: Font.Normal
                font.pixelSize: 27
                lineHeight: 1
            }

            Text {
                id: step2
                color: Style.colorText
                opacity: 0.5
                // TODO read hostname from WifiControl or configuration once implemented
                text: qsTr("Open a web browser\nand navigate to\nyio.remote") + translateHandler.emptyString
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    top: step1.bottom
                    topMargin: 30
                    horizontalCenter: parent.horizontalCenter
                }
                font.family: "Open Sans"
                font.weight: Font.Normal
                font.pixelSize: 27
                lineHeight: 1
            }

            BasicUI.CustomButton {
                id: buttonShutdown
                buttonText: qsTr("Shutdown") + translateHandler.emptyString
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 60
                anchors.horizontalCenter: parent.horizontalCenter

                mouseArea.onClicked: {
                    // TODO create a framebuffer device class instead of launching hard coded shell scripts from QML
                    mainLauncher.launch("fbv -d 1 /bye.png")
                    buttonHandler.interruptHandler.shutdown();
                    // TODO create a system device class for shutdown instead of launching hard coded shell scripts from QML
                    mainLauncher.launch("halt");
                }
            }
        }

        Item {
            width: 480
            height: 800

            Item {
                id: successPage
                anchors.fill: parent
                visible: false

                Text {
                    id: titleText2
                    color: Style.colorText
                    text: qsTr("Connected") + translateHandler.emptyString
                    horizontalAlignment: Text.AlignHCenter
                    anchors {
                        top: parent.top
                        topMargin: 300
                        horizontalCenter: parent.horizontalCenter
                    }
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 60
                    lineHeight: 1
                }

                Text {
                    id: smallText
                    color: Style.colorText
                    opacity: 0.5
                    text: qsTr("YIO Remote has successfully\nconnected to your Wi-Fi network.") + translateHandler.emptyString
                    horizontalAlignment: Text.AlignHCenter
                    anchors {
                        top: titleText2.bottom
                        topMargin: 10
                        horizontalCenter: parent.horizontalCenter
                    }
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 27
                    lineHeight: 1
                }

                BasicUI.CustomButton {
                    buttonText: qsTr("Next") + translateHandler.emptyString
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 60
                    anchors.horizontalCenter: parent.horizontalCenter

                    mouseArea.onClicked: {
                        swipeView.currentIndex = 2;
                    }
                }
            }

            Item {
                id: failurePage
                anchors.fill: parent
                visible: false

                Text {
                    id: titleText3
                    color: Style.colorText
                    text: qsTr("Not connected") + translateHandler.emptyString
                    horizontalAlignment: Text.AlignHCenter
                    anchors {
                        top: parent.top
                        topMargin: 210
                        horizontalCenter: parent.horizontalCenter
                    }
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 60
                    lineHeight: 1
                }

                Text {
                    id: smallTextf
                    color: Style.colorText
                    opacity: 0.5
                    text: qsTr("YIO Remote could not\nconnect to your Wi-Fi network.") + translateHandler.emptyString
                    horizontalAlignment: Text.AlignHCenter
                    anchors {
                        top: titleText3.bottom
                        topMargin: 10
                        horizontalCenter: parent.horizontalCenter
                    }
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 27
                    lineHeight: 1
                }

                Text {
                    id: smallTextf2
                    color: Style.colorText
                    opacity: 0.5
                    text: qsTr("Please try again") + translateHandler.emptyString
                    horizontalAlignment: Text.AlignHCenter
                    anchors {
                        top: smallTextf.bottom
                        topMargin: 30
                        horizontalCenter: parent.horizontalCenter
                    }
                    font.family: "Open Sans"
                    font.weight: Font.Normal
                    font.pixelSize: 27
                    lineHeight: 1
                }

                BasicUI.CustomButton {
                    buttonText: qsTr("Try again") + translateHandler.emptyString
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 60
                    anchors.horizontalCenter: parent.horizontalCenter

                    mouseArea.onClicked: {
                        swipeView.currentIndex = 0;
                        wifi.clearConfiguredNetworks()
                        wifi.startAccessPoint()
                        firstSetupCheck.start();
                    }
                }
            }
        }

        Item {
            width: 480
            height: 800

            Image {
                id: dockLed
                asynchronous: true
                source: "qrc:/images/settings/setup-dock-led.png"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 320
            }

            Image {
                id: dockIllustration
                asynchronous: true
                source: "qrc:/images/settings/setup-dock.png"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 150
            }

            Timer {
                running: true
                repeat: true
                interval: 800

                onTriggered: {
                    if (dockLed.opacity == 1) {
                        dockLed.opacity = 0
                    } else {
                        dockLed.opacity = 1
                    }
                }
            }

            Text {
                id: smallTextDock
                color: Style.colorText
                opacity: 0.5
                text: qsTr("Connect the YIO Dock to a power source and wait until it starts blinking. To share the Wi-Fi network credentials with the dock, tap next.") + translateHandler.emptyString
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                width: 420
                anchors {
                    top: dockLed.bottom
                    topMargin: 80
                    horizontalCenter: parent.horizontalCenter
                }
                font.family: "Open Sans"
                font.weight: Font.Normal
                font.pixelSize: 27
                lineHeight: 1
            }

            BasicUI.CustomButton {
                id: dockButtonNext
                buttonText: qsTr("Next") + translateHandler.emptyString
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 60
                anchors.horizontalCenter: parent.horizontalCenter

                mouseArea.onClicked: {
                    bluetoothArea.lookForDock();
                    // show loader here
                }
            }

            BasicUI.CustomButton {
                buttonText: qsTr("Reboot") + translateHandler.emptyString
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 60
                anchors.left: dockButtonNext.right
                anchors.leftMargin: 20

                mouseArea.onClicked: {
                    // TODO create a system device class for reboot instead of launching hard coded shell scripts from QML
                    mainLauncher.launch("reboot");
                }
            }
        }

        Item {
            width: 480
            height: 800

            Text {
                id: dockSuccessTitle
                color: Style.colorText
                text: qsTr("Wi-Fi setup completed") + translateHandler.emptyString
                width: 420
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    top: parent.top
                    topMargin: 210
                    horizontalCenter: parent.horizontalCenter
                }
                font.family: "Open Sans"
                font.weight: Font.Normal
                font.pixelSize: 60
                lineHeight: 1
            }

            Text {
                id: dockSuccessText
                color: Style.colorText
                opacity: 0.5
                text: qsTr("YIO Dock has successfully connected to your Wi-Fi network.\n\nTo continue the configuration, open a web browser on your computer and navigate to\n" + fileio.read("/apssid").trim() + "local") + translateHandler.emptyString
                width: 420
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors {
                    top: dockSuccessTitle.bottom
                    topMargin: 10
                    horizontalCenter: parent.horizontalCenter
                }
                font.family: "Open Sans"
                font.weight: Font.Normal
                font.pixelSize: 27
                lineHeight: 1
            }

            BasicUI.CustomButton {
                buttonText: qsTr("Reboot") + translateHandler.emptyString
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 60
                anchors.horizontalCenter: parent.horizontalCenter

                mouseArea.onClicked: {
                    // TODO create a framebuffer device class instead of launching hard coded shell scripts from QML
                    mainLauncher.launch("fbv -d 1 /bye.png")
                    // TODO create a system device class for reboot instead of launching hard coded shell scripts from QML
                    mainLauncher.launch("reboot");
                }
            }
        }
    }

    Timer {
        id: firstSetupCheck
        running: true
        repeat: true
        interval: 2000

        onTriggered: {
            // /firstsetup marker file is created by first-time-setup/wifi_network_setup.sh which is called from wifi.php after the user entered network information
            if (fileio.exists("/firstsetup")) {
                console.debug("WifiSetup: initiating firstsetup");
                // FIXME refactor captive portal and either use existing web-socket communication or create a REST API.
                //       Until this is done we have to re-initialize WifiControl to make sure the wpa_ctrl interface is re-attached after the external restart.
                wifi.reset()
                checkWifi.start();
                this.stop();
            }
        }
    }

    Timer {
        id: checkWifi
        repeat: false
        // TODO check multiple times instead of just once. Give option to continue waiting instead of starting from the beginning again
        interval: 10000

        onTriggered: {
            if (wifi.isConnected()) {
                console.debug("WifiSetup: connected to network " + wifi.getCurrentSSID());
                // FIXME Bluetooth credential handling
                msg = fileio.read("/wificred");
                msg += "\"remote_id\":\"" + api.hostname + "\"}";
                failurePage.visible = false;
                successPage.visible = true;
            } else {
                console.debug("WifiSetup: could not connect to network!");
                failurePage.visible = true;
                successPage.visible = false;
            }
            swipeView.currentIndex = 1;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // BLUETOOTH CONNECTION TO THE DOCK
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: bluetoothArea
        onDockFound: {
            console.debug("WifiSetup: Dock found! Sending credentials...");
            // show dock page
            bluetoothArea.sendInfoToDock(msg);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECTION TO THE API
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: api
        onMessageReceived: {
            if (message["type"] == "dock" && message["message"] == "connected") {
                swipeView.currentIndex = 3;
            }
        }
    }

}
