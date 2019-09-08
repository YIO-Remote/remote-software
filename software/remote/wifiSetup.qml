import QtQuick 2.11
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

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
                color: colorText
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
                color: colorText
                opacity: 0.5
                text: qsTr("Connect to the Wi-Fi network\n") + fileio.read("/apssid") + translateHandler.emptyString
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
                color: colorText
                opacity: 0.5
                text: qsTr("Open a web browser\nand navigate to\nhttp://yio.remote") + translateHandler.emptyString
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
                    mainLauncher.launch("fbv -d 1 /bye.png")
                    buttonHandler.interruptHandler.shutdown();
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
                    color: colorText
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
                    color: colorText
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
                    color: colorText
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
                    color: colorText
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
                    color: colorText
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
                        mainLauncher.launch("/usr/bin/yio-remote/reset-wifi.sh")
                        swipeView.currentIndex = 0;
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
                color: colorText
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
                buttonText: qsTr("Next") + translateHandler.emptyString
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 60
                anchors.horizontalCenter: parent.horizontalCenter

                mouseArea.onClicked: {
                    bluetoothArea.lookForDock();
                    // show loader here
                }
            }
        }

        Item {
            width: 480
            height: 800

            Text {
                id: dockSuccessTitle
                color: colorText
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
                color: colorText
                opacity: 0.5
                text: qsTr("YIO Dock has successfully connected to your Wi-Fi network.\n\nTo continue the configuration, open a web browser on your computer and navigate to\nhttp://yioremote.local") + translateHandler.emptyString
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
                    mainLauncher.launch("fbv -d 1 /bye.png")
                    mainLauncher.launch("reboot");
                }
            }
        }
    }

    Timer {
        running: true
        repeat: true
        interval: 2000

        onTriggered: {
            if (fileio.exists("/firstsetup")) {
                checkWifi.start();
                this.stop();
            }
        }
    }

    Timer {
        id: checkWifi
        repeat: false
        interval: 6000

        onTriggered: {
            var wifiSuccess = mainLauncher.launch("/usr/bin/yio-remote/wifi_ssid.sh").trim();
            var ssid = fileio.read("/ssid").trim();
            if (wifiSuccess == ssid) {
                msg = fileio.read("/wificred");
                failurePage.visible = false;
                successPage.visible = true;
            } else {
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
