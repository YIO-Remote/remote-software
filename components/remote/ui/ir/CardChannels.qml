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

import Entity.Remote 1.0

Item {
    ListModel {
        id: channelsModel
    }

    GridView {
        id: channelGridView
        width: parent.width-50; height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        cellWidth: width/3; cellHeight: cellWidth
        clip: true

        model: channelsModel

        delegate: Item {
            id: main
            width: channelGridView.cellWidth; height: width

            Rectangle {
                id: imageContainer
                width: 80; height: 80
                anchors.centerIn: parent
                radius: Style.cornerRadius
                color: Style.colorMedium

                Text {
                    color: Style.colorText
                    text: number
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    anchors.centerIn: parent
                    font { family: "Open Sans Bold"; weight: Font.Bold; pixelSize: 40 }
                    lineHeight: 1
                }

                Image {
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectCrop
                    asynchronous: true
                    source: "file:/" + image
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        haptic.playEffect(Haptic.Click);
                        obj.channel(number);
                    }
                }
            }
        }

        add: Transition {
            PropertyAnimation { properties: "opacity"; from: 0; to: 1; duration: 400; easing.type: Easing.OutExpo }
        }

        displaced: Transition {
            PropertyAnimation { properties: "x,y"; duration: 400; easing.type: Easing.OutBounce }
        }
    }

    Component.onCompleted: {
        for (let i=0; i<obj.channels.length; i++) {
            var o = obj.channels[i];
            o["id"] = i;
            channelsModel.append(o);
        }
    }
}
