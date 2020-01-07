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

import ButtonHandler 1.0
import Style 1.0
import Entity.Light 1.0

import "qrc:/components" as Comp

Comp.ButtonBase {
    id: lightButton
    icon: Style.icons.light
    button.enabled: obj.supportsOn

    cardLoader.source: {
        if (obj.isSupported(Light.F_COLOR)) {
            return "qrc:/components/light/ui/CardColor.qml";
        } else if (obj.isSupported(Light.F_BRIGHTNESS)) {
            return "qrc:/components/light/ui/CardDimmable.qml";
        } else {
            return "qrc:/components/light/ui/CardSwitch.qml";
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CONNECT TO BUTTONS
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Connections {
        target: ButtonHandler
        enabled: state == "open" ? true : false

        onButtonPressed: {
            var tmp;

            switch (button) {
            case ButtonHandler.DPAD_UP:
                if (obj.isSupported(Light.F_BRIGHTNESS)) {
                    tmp = obj.brightness+10;
                    if (tmp > 100) {
                        tmp = 100;
                    }
                    obj.setBrightness(tmp);
                }
                break;
            case ButtonHandler.DPAD_DOWN:
                if (obj.isSupported(Light.F_BRIGHTNESS)) {
                    tmp = obj.brightness-10;
                    if (tmp < 0) {
                        tmp = 0;
                    }
                    obj.setBrightness(tmp);
                }
                break;
            case ButtonHandler.DPAD_MIDDLE:
                if (obj.supportsOn) {
                    obj.toggle();
                }
                break;
            }
        }
    }
}
