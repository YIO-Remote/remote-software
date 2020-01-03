/******************************************************************************
 *
 * Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
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

class GestureSensor : public Device {
    Q_OBJECT

 public:
    enum Gesture {
        None
        // To be defined
    };
    Q_ENUM(Gesture)

    Q_PROPERTY(Gesture gesture READ gesture NOTIFY gestureEvent)

    /**
     * @brief gestureDetection Enable or disable gesture detection.
     * @param state true = enable detection.
     */
    Q_INVOKABLE virtual void gestureDetection(bool state) = 0;

    /**
     * @brief gesture Return gesture
     * @return The last detected gesture
     */
    virtual Gesture gesture() const = 0;

 signals:
    void gestureEvent(Gesture gesture);

 protected:
    explicit GestureSensor(QObject *parent = nullptr) : Device(parent) {}
};
