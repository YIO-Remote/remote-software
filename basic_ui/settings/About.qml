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
    width: parent.width; height: childrenRect.height+40
    radius: Style.cornerRadius
    color: Style.color.dark

    Text {
        id: title
        color: Style.color.text
        text: qsTr("Thank you!") + translateHandler.emptyString
        anchors { left: parent.left; leftMargin: 20; top: parent.top; topMargin: 20 }
        font: Style.font.button
    }

    Text {
        id: smallText
        color: Style.color.text
        opacity: 0.5
        text: qsTr("YIO being an open source project, benefits from all the contributors. Big thanks to everyone who added something to the project so far:\n%1").arg("
Christian Riedl
Christopher Shepherd
Markus Mahr
Markus Zehnder
Niels de Klerk
Marton Borzak") + qsTr("\n\nTo learn more about the project, visit\n%1").arg("www.yio-remote.com") + translateHandler.emptyString
        wrapMode: Text.WordWrap
        width: parent.width - 40
        anchors { left: parent.left; leftMargin: 20; top: title.bottom; topMargin: 10 }
        font { family: "Open Sans Regular"; pixelSize: 20 }
        lineHeight: 1
    }
}
