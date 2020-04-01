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

import "qrc:/basic_ui" as BasicUI

Item {
    width: parent.width
    height: childrenRect.height

    property string groupID

    function checkGroupState() {
        var s = false;

        var eArr = config.groups[groupID].entities;

        for (var i=0; i<eArr.length; i++) {
            var eid = entities.get(eArr[i]);

            if (eid && eid.isOn) {
                s = true;
            }
        }

        return s;
    }

    Timer {
        repeat: false
        running: true
        interval: 2000

        onTriggered: {
            customSwitch.checked = Qt.binding(function() { return checkGroupState()})
        }
    }

    // HEADER
    Item {
        id: header
        width: parent.width
        height: 80

        Text {
            color: Style.color.text
            text: qsTr(config.groups[groupID].name) + translateHandler.emptyString
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Open Sans Regular"
            font.weight: Font.Normal
            font.pixelSize: 32
            lineHeight: 1
        }

        property alias customSwitch: customSwitch

        BasicUI.CustomSwitch {
            id: customSwitch
            visible: config.groups[groupID].switch
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter

            checked: false
            mouseArea.onClicked: {
                if (!customSwitch.checked) {
                    // turn off
                    var eArr = config.groups[groupID].entities;

                    for (var i=0; i<eArr.length; i++) {
                        var eid = entities.get(eArr[i]);

                        if (eid) {
                            eid.turnOff();
                        }
                    }
                } else {
                    // turn on
                    var eArr = config.groups[groupID].entities;

                    for (var i=0; i<eArr.length; i++) {
                        var eid = entities.get(eArr[i]);

                        if (eid) {
                            eid.turnOn();
                        }
                    }
                }
            }
        }
    }

    // ENTITIES
    ListView {
        width: parent.width
        height: contentHeight
        anchors.top: header.bottom
        interactive: false
        spacing: 10

        model: config.groups[groupID].entities
        delegate: entityDelegate
    }

    // LISTVIEW DELEGATE
    Component {
        id: entityDelegate

        Loader {
            id: entityLoader
            width: 460
            height: 125
            anchors.horizontalCenter: parent.horizontalCenter

            Component.onCompleted: {
                var e = entities.get(config.groups[groupID].entities[index]);
                if (e)
                    this.setSource("qrc:/components/"+ e.type +"/ui/Button.qml", { "obj": e });
            }
        }
    }

}
