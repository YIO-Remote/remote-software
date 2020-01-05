/******************************************************************************
 *
 * Copyright (C) 2019-2020 Markus Zehnder <business@markuszehnder.ch>
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

#include "../batteryfuelgauge.h"

class BatteryFuelGaugeMock : public BatteryFuelGauge {
    Q_OBJECT
 public:
    explicit BatteryFuelGaugeMock(QObject* parent = nullptr) : BatteryFuelGauge("BatteryFuelGaugeMock", parent) {
        setCapacity(2500);
    }

    ~BatteryFuelGaugeMock() override {}

    // BatteryFuelGauge interface
 public:
    void    begin() override{};
    int     getVoltage() override { return 5000; }
    int     getFullChargeCapacity() override { return 2500; }
    int     getAverageCurrent() override { return -1600; }
    int     getAveragePower() override { return -2500; }
    int     getStateOfCharge() override { return 100; }
    int16_t getInternalTemperatureC() override { return 23; }
    int     getStateOfHealth() override { return 100; }
    int     getFullAvailableCapacity() override { return 2500; }
    int     getRemainingCapacity() override { return 2000; }
    int     getDesignCapacity() override { return 2500; }
    void    changeCapacity(int newCapacity) override { Q_UNUSED(newCapacity) }
};
