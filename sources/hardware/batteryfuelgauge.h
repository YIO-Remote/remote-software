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

class BatteryFuelGauge : public Device {
    Q_OBJECT
    Q_PROPERTY(int capacity READ getCapacity WRITE setCapacity)

 public:
    Q_INVOKABLE virtual void    begin() = 0;
    Q_INVOKABLE virtual int     getVoltage() = 0;
    Q_INVOKABLE virtual int     getFullChargeCapacity() = 0;
    Q_INVOKABLE virtual int     getAverageCurrent() = 0;
    Q_INVOKABLE virtual int     getAveragePower() = 0;
    Q_INVOKABLE virtual int     getStateOfCharge() = 0;
    Q_INVOKABLE virtual int16_t getInternalTemperatureC() = 0;  // Result in 0.1 Celsius
    Q_INVOKABLE virtual int     getStateOfHealth() = 0;
    Q_INVOKABLE virtual int     getFullAvailableCapacity() = 0;
    Q_INVOKABLE virtual int     getRemainingCapacity() = 0;
    Q_INVOKABLE virtual int     getDesignCapacity() = 0;
    Q_INVOKABLE virtual void    changeCapacity(int newCapacity) = 0;

    void setCapacity(int capacity) { m_capacity = capacity; }

    int getCapacity() { return m_capacity; }

 protected:
    explicit BatteryFuelGauge(QObject *parent = nullptr) : Device(parent) {}

    int m_capacity;
};
