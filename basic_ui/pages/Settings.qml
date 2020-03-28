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
import Style 1.0

import Haptic 1.0
import Battery 1.0
import SoftwareUpdate 1.0

import "qrc:/basic_ui/settings" as Settings

SwipeView {
    id: settingsSwipeView
    width: parent.width
    height: parent.height
    interactive: false
    clip: true
    anchors.horizontalCenter: parent.horizontalCenter

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property bool _isCurrentItem: parent._isCurrentItem

    on_IsCurrentItemChanged: {
        if (!_isCurrentItem) {
            secondPageLoader.active = false;
            secondPageLoader.source = "";
            settingsSwipeView.decrementCurrentIndex();
        }
    }

    //: Name of the settings page
    property string title: ""

    property alias _contentY: settingsFirstPageFlickable.contentY
    property alias _contentHeight: settingsFirstPageFlickable.contentHeight
    property alias _height: settingsFirstPageFlickable.height

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    function loadPage(url, buttonTitle) {
        secondPageLoader.setSource(url);
        secondPageLoader.active = true;
        settingsSwipeView.incrementCurrentIndex();
        backButtonText.text = qsTr(buttonTitle) + translateHandler.emptyString
        settingsSwipeView.title = qsTr("Settings") + translateHandler.emptyString;
        settingsSwipeView.scrolledUp();
    }

    function evaluateHeader() {
        if (settingsFirstPageFlickable.contentY > 10) {
            settingsSwipeView.scrollupBegin();
        } else {
            settingsSwipeView.scrolldownBegin()
        }

        if (settingsFirstPageFlickable.contentY > 130) {
            settingsSwipeView.scrolledUp();
            settingsSwipeView.title = qsTr("Settings") + translateHandler.emptyString;
        } else {
            settingsSwipeView.scrolledDown();
            settingsSwipeView.title = "";
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SIGNALS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    signal scrolledUp()
    signal scrolledDown()
    signal scrollupBegin()
    signal scrolldownBegin()


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // SETTINGS BLOCKS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Item {
        Flickable {
            id: settingsFirstPageFlickable
            onContentYChanged: evaluateHeader()

            width: parent.width
            height: parent.height
            maximumFlickVelocity: 6000
            flickDeceleration: 1000
            contentHeight: settingsFlow.height + 80
            boundsBehavior: Flickable.DragAndOvershootBounds
            flickableDirection: Flickable.VerticalFlick

            Behavior on contentY {
                PropertyAnimation {
                    duration: 300
                    easing.type: Easing.OutExpo
                }
            }

            ScrollBar.vertical: ScrollBar {
                opacity: 0.5
            }

            Flow {
                id: settingsFlow
                width: parent.width-20
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10

                Rectangle {
                    width: parent.width
                    height: 200
                    color: Style.colorBackground

                    Text {
                        id: titleText
                        color: Style.colorText
                        text: qsTr("Settings") + translateHandler.emptyString
                        anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter; verticalCenterOffset: 20 }
                        font { family: "Open Sans Regular"; pixelSize: 60 }
                        lineHeight: 1
                    }
                }

                Item {
                    width: parent.width; height: 80
                    Text {
                        color: Style.colorText
                        text: SoftwareUpdate.updateAvailable ? qsTr("New software is available.") + translateHandler.emptyString : qsTr("Your software is up to date.") + translateHandler.emptyString
                        anchors { top:parent.top; horizontalCenter: parent.horizontalCenter }
                        font: Style.fonts.button
                        lineHeight: 1
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            loadPage("qrc:/basic_ui/settings/Softwareupdate.qml", "Software update");
                        }
                    }
                }

                Settings.RemoteConfig {}

                Settings.Button {
                    title: qsTr("Display") + translateHandler.emptyString //: Settings page title for the display settings (LCD display)
                    icon: Style.icons.tv
                    url: "qrc:/basic_ui/settings/Display.qml"
                }

                Settings.Button {
                    title: qsTr("Language") + translateHandler.emptyString //: Settings page title for language settings
                    subtitle: {
                        for (var i=0; i<translations.length; i++) {
                            if (translations[i].id === config.settings.language) {
                                return translations[i].name;
                            }
                        }
                    }
                    icon: Style.icons.language
                    url: "qrc:/basic_ui/settings/Languages.qml"
                }

                Settings.Button {
                    title: qsTr("Integrations") + translateHandler.emptyString //: Settings page title for integration settings
                    subtitle: integrations.list.length > 1 ? (integrations.list.length + " integrations loaded" + translateHandler.emptyString) : (integrations.list.length + " integration loaded" + translateHandler.emptyString)
                    icon: Style.icons.integration
                    url: "qrc:/basic_ui/settings/Integrations.qml"
                }

                Settings.Button {
                    title: qsTr("Battery & power") + translateHandler.emptyString //: Settings page title for battery and power settings
                    subtitle: {
                        var hours = Battery.remainingLife;
                        var rhours = Math.floor(hours);
                        var minutes = (hours - rhours) * 60;
                        var rminutes = Math.round(minutes);

                        return rhours + "h " + rminutes + qsTr("m remaining") + translateHandler.emptyString
                    }
                    icon: Style.icons.battery
                    url: "qrc:/basic_ui/settings/Battery.qml"
                }

                Settings.Button {
                    title: qsTr("WiFi & bluetooth") + translateHandler.emptyString //: Settings page title for wifi and bluetooth settings
                    icon: Style.icons.wifi_bluetooth
                    url: "qrc:/basic_ui/settings/Wifi.qml"
                }

                Settings.Button {
                    title: qsTr("System") + translateHandler.emptyString //: Settings page title for system settings
                    icon: Style.icons.system
                    url: "qrc:/basic_ui/settings/System.qml"
                }

                Settings.Button {
                    title: qsTr("About") + translateHandler.emptyString //: Settings page title for about
                    icon: Style.icons.about
                    url: "qrc:/basic_ui/settings/About.qml"
                }
            }
        }
    }

    Item {
        Item {
            id: backButton
            visible: secondPageLoader.active && _isCurrentItem
            width: parent.width; height: 60
            anchors { top: parent.top; topMargin: 40 }

            Text {
                id: backButtonIcon
                color: Style.colorText
                text: Style.icons.left_arrow
                renderType: Text.NativeRendering
                width: 70; height: 70
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font {family: "icons"; pixelSize: 80 }
                anchors { left: parent.left; leftMargin: -15; verticalCenter: parent.verticalCenter }
            }

            Text {
                id: backButtonText
                color: Style.colorText
                anchors { left: backButtonIcon.right; verticalCenter: parent.verticalCenter }
                font { family: "Open Sans Bold"; weight: Font.Bold; pixelSize: 27 }
                lineHeight: 1
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    loader_second.source = "";
                    loader_second.active = false;
                    inputPanel.active = false;

                    settingsSwipeView.decrementCurrentIndex();
                    backButtonTimer.start();
                    evaluateHeader();
                }

                Timer {
                    id: backButtonTimer
                    repeat: false
                    running: false
                    interval: 300

                    onTriggered: {
                        secondPageLoader.active = false;
                        secondPageLoader.source = "";
                    }
                }
            }
        }

        Flickable {
            id: settingsSecondPageFlickable
            width: parent.width - 20; height: parent.height - backButton.height
            anchors { top: backButton.bottom; topMargin: 10; horizontalCenter: parent.horizontalCenter }
            maximumFlickVelocity: 6000
            flickDeceleration: 1000
            contentHeight: secondPageLoader.height + 120
            boundsBehavior: Flickable.DragAndOvershootBounds
            flickableDirection: Flickable.VerticalFlick
            clip: true

            Behavior on contentY {
                PropertyAnimation {
                    duration: 300
                    easing.type: Easing.OutExpo
                }
            }

            ScrollBar.vertical: ScrollBar {
                opacity: 0.5
            }

            Loader {
                id: secondPageLoader
                width: parent.width
                active: false
            }
        }
    }
}
