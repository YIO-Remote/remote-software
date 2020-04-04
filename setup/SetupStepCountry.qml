/******************************************************************************
 *
 * Copyright (C) 2018-2020 Marton Borzak <hello@martonborzak.com>
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
import Style 1.0
import Haptic 1.0
import WifiControl 1.0

Item {
    id: container
    width: 480; height: 800


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    function getCountry(id) {
        for (var i=0; i<countries.length; i++) {
            if (id.includes(countries[i].Code)) {
                return i;
            }
        }
    }

    function calculateOpacity(index) {
        if (countryListView.currentIndex === index) {
            return 1;
        } else {
            if (Math.abs(countryListView.currentIndex - index) === 1) {
                return 0.7;
            } else if (Math.abs(countryListView.currentIndex - index) === 2) {
                return 0.4;
            } else {
                return 0.2;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Text {
        id: smalltext
        color: Style.color.text
        opacity: 0.5
        text: qsTr("Select your country or region") + translateHandler.emptyString
        horizontalAlignment: Text.AlignHCenter
        anchors { top: parent.top; topMargin: 30; horizontalCenter: parent.horizontalCenter }
        font { family: "Open Sans Regular"; weight: Font.Normal; pixelSize: 27 }
        lineHeight: 1
    }

    ListView {
        id: countryListView
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }
        width: parent.width; height: parent.height-100
        model: countries
        interactive: true
        highlightMoveDuration: 300
        currentIndex: getCountry(config.settings.language)
        preferredHighlightBegin: height / 2 - 37; preferredHighlightEnd: height / 2 + 37
        highlightRangeMode: ListView.StrictlyEnforceRange
        clip: true

        Component.onCompleted: {
            positionViewAtIndex(getCountry(config.settings.language), ListView.Center)
        }

        delegate:
            Item {
            x: 20
            width: parent.width-40; height: 100

            Text {
                color: Style.color.text
                opacity: calculateOpacity(index)
                text: countries[index].Name
                width: parent.width
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
                anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
                font { family: "Open Sans Light"; weight: Font.Light; pixelSize: 50 }
                lineHeight: 1
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    countryListView.currentIndex = index;
                    wifi.countryCode = countries[index].Code;
                    console.debug("New WiFi country code: " + wifi.countryCode);
                    container.parent.parent.incrementCurrentIndex();
                }
            }
        }

        highlight: Rectangle { x: 20; color: Style.color.highlight1; radius: Style.cornerRadius }
    }
}
