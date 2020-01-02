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

#include <cassert>

#include "../gesturesensor.h"
#include "apds9960.h"

class Apds9960GestureSensor : public GestureSensor {
    Q_OBJECT

 public:
    explicit Apds9960GestureSensor(APDS9960* apds, QObject* parent = nullptr) : GestureSensor(parent), p_apds(apds) {
        assert(apds);
    }

    // GestureSensor interface
 public:
    void gestureDetection(bool state) override {
        m_gestureDetection = state;

        if (state) {
            // turn on
        } else {
            // turn off
        }
    }

    Gesture gesture() override { return m_gesture; }

 private:
    APDS9960* p_apds;
    Gesture   m_gesture;
    bool      m_gestureDetection = false;
};
