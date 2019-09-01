import QtQuick 2.11
import QtQuick.Controls 2.5

import "qrc:/basic_ui" as BasicUI

Item {
    width: 480
    height: 800
    visible: true

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
                anchors.bottomMargin: 30
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
                        topMargin: 210
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

                Text {
                    id: smallText2
                    color: colorText
                    opacity: 0.5
                    text: qsTr("To continue the configuration,\nopen a web browser\nand navigate to\nhttp://yioremote.local") + translateHandler.emptyString
                    horizontalAlignment: Text.AlignHCenter
                    anchors {
                        top: smallText.bottom
                        topMargin: 30
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
                    anchors.bottomMargin: 30
                    anchors.horizontalCenter: parent.horizontalCenter

                    mouseArea.onClicked: {
                        mainLauncher.launch("reboot");
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
                    anchors.bottomMargin: 30
                    anchors.horizontalCenter: parent.horizontalCenter

                    mouseArea.onClicked: {
                        swipeView.currentIndex = 0;
                        mainLauncher.launch("rm /networklist");
                        mainLauncher.launch("rm /firstsetup");
                        mainLauncher.launch("rm /apssid");
                        mainLauncher.launch("touch /wifisetup");
                        mainLauncher.launch("/usr/bin/yio-remote/first-time-setup/firstrun.sh")
                    }
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
        interval: 4000

        onTriggered: {
            swipeView.currentIndex = 1;
            var wifiSuccess = mainLauncher.launch("cat /sys/class/net/wlan0/operstate");
            if (wifiSuccess == "up\n") {
                failurePage.visible = false;
                successPage.visible = true;
            } else {
                failurePage.visible = true;
                successPage.visible = false;
            }
        }
    }
}
