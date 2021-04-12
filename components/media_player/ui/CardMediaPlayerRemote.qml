/******************************************************************************
 *
 * Copyright (C) 2020 Michael Lörcher <MichaelLoercher@web.de>
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
import QtQuick 2.12
import QtQuick.Controls 2.12
import Style 1.0


import Haptic 1.0

import "qrc:/basic_ui" as BasicUI
import "qrc:/components/remote/ui/ir"

Rectangle {
    id: main
    width: parent.width; height: parent.height
    color: Style.color.dark
    radius: Style.cornerRadius

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // VARIABLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    function onTextInput() {
        obj.textInputMediaPlayer(textInputTextField.text);
        textInputTextField.focus = false;
        textInputTextField.text = "";
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property alias swipeView: swipeView

    SwipeView {
        id: swipeView
        width: parent.width; height: 500
        currentIndex: 0
        interactive: false
        clip: true



}
    Rectangle {
        id: commandContainer
        width: 300; height: 80
        anchors { top: parent.top; topMargin: 100; horizontalCenter: parent.horizontalCenter }
        color: Style.color.text
        radius: Style.cornerRadius


        Text {
            id: textInputIcon
            color: Style.color.background
            text: Style.icon.down_arrow_bold
            renderType: Text.NativeRendering
            width: 70; height: 70
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            font {family: "icons"; pixelSize: 80 }
            anchors { left: parent.left; leftMargin: 10; verticalCenter: parent.verticalCenter }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    onTextInput();
                }
            }
        }

        TextField {
            id: textInputTextField
            cursorVisible: false
            width: parent.width-90; height: parent.height
            anchors { left: textInputIcon.right; leftMargin: 10; verticalCenter: parent.verticalCenter }
            placeholderText: "Textinput" + translateHandler.emptyString
            color: Style.color.background
            font { family: "Open Sans Regular"; pixelSize: 27 }

            background: Rectangle {
                color: Style.color.backgroundTransparent
                border.width: 0
            }

            onAccepted: {
                onTextInput();
            }

            onFocusChanged: {
                if (focus) {
                    inputPanel.active = true
                    itemFlickable.contentY = 0;
                } else
                    inputPanel.active = false
            }
        }
    }
    Item {
            id: tooltips
            width: parent.width; height: 80
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 60

            Item {
                id: leftToolTip
                width: childrenRect.width; height: 60
                anchors { left: parent.left; leftMargin: 20; verticalCenter: parent.verticalCenter }

                Text {
                    color: Style.color.text
                    text: Style.icon.circle
                    width: 60; height: 60
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    font { family: "icons"; pixelSize: 60 }
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                }

                Text {
                    color: Style.color.text
                    opacity: 0.5
                    text: qsTr("Mute") + translateHandler.emptyString
                    verticalAlignment: Text.AlignVCenter
                    anchors { left: parent.left; leftMargin: 50; verticalCenter: parent.verticalCenter }
                    font { family: "Open Sans Regular"; pixelSize: 24 }
                    lineHeight: 1
                }
            }

            Item {
                id: middleToolTip
                width: childrenRect.width; height: 60
                anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }

                Text {
                    color: Style.color.text
                    text: Style.icon.square
                    width: 60; height: 60
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    font {family: "icons"; pixelSize: 60 }
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                }

                Text {
                    color: Style.color.text
                    opacity: 0.5
                    text: qsTr("Back") + translateHandler.emptyString
                    verticalAlignment: Text.AlignVCenter
                    anchors { left: parent.left; leftMargin: 50; verticalCenter: parent.verticalCenter }
                    font { family: "Open Sans Regular"; pixelSize: 24 }
                    lineHeight: 1
                }
            }

            Item {
                id: rightToolTip
                width: childrenRect.width; height: 60
                anchors { right: parent.right; rightMargin: 40; verticalCenter: parent.verticalCenter }

                Text {
                    color: Style.color.text
                    text: Style.icon.square_full
                    width: 60; height: 60
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    font { family: "icons"; pixelSize: 60 }
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                }

                Text {
                    color: Style.color.text
                    opacity: 0.5
                    text: qsTr("Menu") + translateHandler.emptyString
                    verticalAlignment: Text.AlignVCenter
                    anchors { left: parent.left; leftMargin: 50; verticalCenter: parent.verticalCenter }
                    font { family: "Open Sans Regular"; pixelSize: 24 }
                    lineHeight: 1
                }
            }
    }
}
