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

Item {
    id: container
    width: 480; height: 800

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FUNCTIONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    function getLanguage(id) {
        for (var i=0; i<translations.length; i++) {
            if (translations[i].id === id) {
                return i;
            }
        }
    }

    function calculateOpacity(index) {
        if (languageListView.currentIndex === index) {
            return 1;
        } else {
            if (Math.abs(languageListView.currentIndex - index) === 1) {
                return 0.7;
            } else if (Math.abs(languageListView.currentIndex - index) === 2) {
                return 0.4;
            } else {
                return 0.2;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ListView {
        id: languageListView
        anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
        width: parent.width; height: parent.height
        model: translations
        interactive: true
        highlightMoveDuration: 300
        currentIndex: getLanguage(config.settings.language);
        preferredHighlightBegin: height / 2 - 37; preferredHighlightEnd: height / 2 + 37
        highlightRangeMode: ListView.StrictlyEnforceRange

        delegate:
            Item {
            x: 20
            width: parent.width-40; height: 100

            Text {
                color: Style.color.text
                opacity: calculateOpacity(index)
                text: translations[index].name
                horizontalAlignment: Text.AlignHCenter
                anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
                font { family: "Open Sans Light"; weight: Font.Light; pixelSize: 50 }
                lineHeight: 1
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    Haptic.playEffect(Haptic.Click);
                    languageListView.currentIndex = index;
                    translateHandler.selectLanguage(translations[index].id);
                    var tmp = config.settings;
                    tmp.language = translations[index].id;
                    config.settings = tmp;
                    config.writeConfig();
                    container.parent.parent.incrementCurrentIndex();
                }
            }
        }

        highlight: Rectangle { x: 20; color: Style.color.highlight1; radius: Style.cornerRadius }
    }
}
