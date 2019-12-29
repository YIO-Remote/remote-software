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

#ifndef BATTERYFUELGAUGEMOCK_H
#define BATTERYFUELGAUGEMOCK_H


#include "batteryfuelgauge.h"

class BatteryFuelGaugeMock : public BatteryFuelGauge
{
    Q_OBJECT
public:
    BatteryFuelGaugeMock(QObject* parent = nullptr) : BatteryFuelGauge(parent)
    {
        setCapacity(2500);
    }

    ~BatteryFuelGaugeMock() {

    }

    // BatteryFuelGauge interface
public:
    virtual void begin() override {}
    virtual int getVoltage() override { return 5000; }
    virtual int getFullChargeCapacity() override { return 2500; }
    virtual int getAverageCurrent() override { return -1600; }
    virtual int getAveragePower() override { return -2500; }
    virtual int getStateOfCharge() override { return 100; }
    virtual int16_t getInternalTemperatureC() override  { return 23; }
    virtual int getStateOfHealth() override { return 100; }
    virtual int getFullAvailableCapacity() override { return 2500; }
    virtual int getRemainingCapacity() override { return 2000; }
    virtual int getDesignCapacity() override  { return 2500; }
    virtual void changeCapacity(int newCapacity) override { Q_UNUSED(newCapacity) }
};

#endif // BATTERYFUELGAUGEMOCK_H
