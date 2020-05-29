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

import "qrc:/components" as Comp

Comp.ButtonBase {
    id: weatherButton
    icon: ""
    title.text: obj.friendly_name + "\r\n" + obj.current.temp
    cardLoader.source: "qrc:/components/weather/ui/Card.qml"

    Image {
       id: image
       asynchronous: true
       width: 85
       height: 85
       fillMode: Image.PreserveAspectFit
       anchors { left: parent.left; leftMargin: 20; verticalCenter: parent.verticalCenter }
       source: obj.current.imageurl;
       }
}
