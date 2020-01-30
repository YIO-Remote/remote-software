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

//import QtQuick 2.11
//import QtQuick.Controls 2.5

//import "qrc:/basic_ui" as BasicUI

//Flickable {
//    id: itemFlickable

//    //: Name of the settings page
//    property string title: qsTr("Settings") + translateHandler.emptyString

//    property bool _isCurrentItem: parent._isCurrentItem

//    signal scrolledUp()
//    signal scrolledDown()
//    signal scrollupBegin()
//    signal scrolldownBegin()

//    onContentYChanged: {
//        if (contentY > 10) {
//            scrollupBegin();
//        } else {
//            scrolldownBegin()
//        }

//        if (contentY > 130) {
//            scrolledUp();
//        } else {
//            scrolledDown();
//        }
//    }

//    width: parent.width
//    height: parent.height
//    maximumFlickVelocity: 6000
//    flickDeceleration: 1000
//    contentHeight: settingsPage.height + mainNavigation.height + 20
//    boundsBehavior: Flickable.DragAndOvershootBounds
//    flickableDirection: Flickable.VerticalFlick

//    Behavior on contentY {
//        PropertyAnimation {
//            duration: 300
//            easing.type: Easing.OutExpo
//        }
//    }

//    ScrollBar.vertical: ScrollBar {
//        opacity: 0.5
//    }

//    BasicUI.Settings {
//        id: settingsPage
//        __isCurrentItem: _isCurrentItem
//    }
//}


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
            contentHeight: settingsFlow.height + 220
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
                        text: qsTr("Your software is up to date.") + translateHandler.emptyString
                        anchors { top:parent.top; horizontalCenter: parent.horizontalCenter }
                        font: Style.buttonFont
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
                    subtitle: "English"
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
                font: Style.buttonFont
                lineHeight: 1
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    Haptic.playEffect(Haptic.Click);
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

        Loader {
            id: secondPageLoader
            width: parent.width
            active: false
            anchors { top: backButton.bottom; topMargin: 10 }
        }
    }
}
