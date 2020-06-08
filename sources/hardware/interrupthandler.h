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

#pragma once

#include "device.h"

// Error translation strings are defined here to include them on every build, independent of the platform!
static QString ERR_DEV_INTR_INIT = QObject::tr("Cannot initialize the interrupt handler. Please restart the remote.");

class InterruptHandler : public Device {
    Q_OBJECT

 public:
    enum Events {
        APDS9960,
        BATTERY,
        DPAD_UP,
        DPAD_UP_RELEASED,
        DPAD_DOWN,
        DPAD_DOWN_RELEASED,
        DPAD_LEFT,
        DPAD_LEFT_RELEASED,
        DPAD_RIGHT,
        DPAD_RIGHT_RELEASED,
        DPAD_MIDDLE,
        DPAD_MIDDLE_RELEASED,
        TOP_RIGHT,
        TOP_RIGHT_RELEASED,
        TOP_LEFT,
        TOP_LEFT_RELEASED,
        BOTTOM_RIGHT,
        BOTTOM_RIGHT_RELEASED,
        BOTTOM_LEFT,
        BOTTOM_LEFT_RELEASED,
        VOLUME_UP,
        VOLUME_UP_RELEASED,
        VOLUME_DOWN,
        VOLUME_DOWN_RELEASED,
        CHANNEL_UP,
        CHANNEL_UP_RELEASED,
        CHANNEL_DOWN,
        CHANNEL_DOWN_RELEASED
    };
    Q_ENUM(Events)

    Q_INVOKABLE virtual void shutdown() = 0;

 signals:
    void interruptEvent(int event);
    void resetEvent();

 protected:
    explicit InterruptHandler(QString name, QObject *parent = nullptr) : Device(name, parent) {}
};
