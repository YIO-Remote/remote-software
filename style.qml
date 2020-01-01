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
pragma Singleton

import QtQuick 2.11

QtObject {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // COLORS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    property int cornerRadius: 18 // radius of the buttons, defined here

    property bool darkMode: true

    property color colorGreen: "#19D37B"
    property color colorRed: "#EA003C"
    property color colorOrange: "#FF7241"
    property color colorBlue: "#19435E"

    property color colorBackground: darkMode ? "#000000" : "#ffffff"
    property color colorBackgroundTransparent: darkMode ? "#00000000" :  "#00000000"

    property color colorText: darkMode ? "#ffffff" : "#000000"
    property color colorLine: darkMode ? "#ffffff" : "#000000"

    property color colorHighlight1: "#918682"
    property color colorHighlight2: "#313247"

    property color colorLight: darkMode ? "#484848" : "#CBCBCB"
    property color colorMedium: darkMode ? "#282828" : "#D4D4D4"
    property color colorDark: darkMode ? "#1C1C1C" : "#ffffff"

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FONT STYLES
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    property font buttonFont: Qt.font({
                                          family: "Open Sans",
                                          weight: Font.Normal,
                                          pixelSize: 27,
                                          lineHeight: 1
                                      })
}
