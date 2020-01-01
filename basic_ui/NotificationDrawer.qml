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
import QtGraphicalEffects 1.0

import "qrc:/basic_ui" as BasicUI

Item {
    id: notificationDrawerContent
    width: parent.width
    height: parent.height

    // TITLE

    Text {
        id: title
        text: qsTr("Notifications") + translateHandler.emptyString
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 40

        color: Style.colorText
        verticalAlignment: Text.AlignVCenter
        font.family: "Open Sans"
        font.weight: Font.Normal
        font.pixelSize: 60
        lineHeight: 1
    }

    // HERE COMES THE NOTIFICATIONS

    Component {
        id: notificationDelegate

        BasicUI.Notification {
            _state: "permanent"
            type: notifications.list[index].error
            text: notifications.list[index].text
            action: notifications.list[index].action
            actionlabel: notifications.list[index].actionlabel
            timestamp: notifications.list[index].timestamp
            idN: notifications.list[index].id
        }
    }


    ListView {
        id: notificationsListView
        width: parent.width
        height: parent.height - title.height - 70
        anchors.top: title.bottom
        anchors.topMargin: 30
        maximumFlickVelocity: 4000
        flickDeceleration: 2000
//        clip: true
        flickableDirection: Flickable.VerticalFlick
        contentHeight: count * notificationDelegate.height
        spacing: 10

        ScrollBar.vertical: ScrollBar {
            opacity: 0.5
        }

        model: notifications.list

        delegate: notificationDelegate
    }
}
