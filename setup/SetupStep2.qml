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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // UI ELEMENTS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ListView {
        id: languageListView
        anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
        width: parent.width; height: parent.height
        model: translations
        interactive: true
        //        preferredHighlightBegin: height / 2 - 37; preferredHighlightEnd: height / 2 + 37
        highlightMoveDuration: 300
        currentIndex: getLanguage(config.settings.language);
        focus: true

        delegate:
            Item {
            width: parent.width; height: 100

            Text {
                color: Style.colorText
                text: translations[index].name
                horizontalAlignment: Text.AlignHCenter
                anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
                font { family: "Open Sans Light"; weight: Font.Light; pixelSize: 60 }
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

        highlight: Rectangle { color: Style.colorHighlight1; radius: Style.cornerRadius }
    }
}
