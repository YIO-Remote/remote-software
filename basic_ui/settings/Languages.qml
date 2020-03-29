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
import Style 1.0

Rectangle {
    id: container
    width: parent.width; height: languageListView.height
    radius: Style.cornerRadius
    color: Style.colorDark

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ListView {
        id: languageListView
        anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
        width: parent.width; height: childrenRect.height
        model: translations
        interactive: false
        preferredHighlightBegin: height / 2 - 37; preferredHighlightEnd: height / 2 + 37
        highlightMoveDuration: 300
        currentIndex: getLanguage(config.settings.language);
        focus: true

        delegate:
            Rectangle {
            width: parent.width; height: 74
            color: Style.colorBackgroundTransparent

            Text {
                text: translations[index].name
                color: Style.colorText
                anchors { left: parent.left; leftMargin: 20; verticalCenter: parent.verticalCenter }
                font: Style.fonts.button
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    languageListView.currentIndex = index;
                    translateHandler.selectLanguage(translations[index].id);
                    var tmp = config.settings;
                    tmp.language = translations[index].id;
                    config.settings = tmp;
                    config.writeConfig();
                }
            }
        }

        highlight: Rectangle { color: Style.colorHighlight1; radius: Style.cornerRadius }
    }
}
