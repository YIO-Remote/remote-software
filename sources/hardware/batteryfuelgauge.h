/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
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
static QString ERR_DEV_BATTERY_INIT = QObject::tr("Cannot initialize the battery sensor. Please restart the remote.");
static QString ERR_DEV_BATTERY_COMM = QObject::tr("Battery sensor communication error. Please restart the remote.");

class BatteryFuelGauge : public Device {
    Q_OBJECT
    Q_PROPERTY(int capacity READ getCapacity WRITE setCapacity)
    Q_PROPERTY(float level READ getLevel NOTIFY levelChanged)
    Q_PROPERTY(int remainingLife READ remainingLife NOTIFY remainingLifeChanged)
    Q_PROPERTY(int health READ getHealth NOTIFY healthChanged)
    Q_PROPERTY(int averagePower READ getAveragePower NOTIFY averagePowerChanged)
    Q_PROPERTY(bool isCharging READ getIsCharging NOTIFY isChargingChanged)

 public:
    virtual void    begin()                         = 0;
    virtual int     getVoltage()                    = 0;
    virtual int     getFullChargeCapacity()         = 0;
    virtual int     getAverageCurrent()             = 0;
    virtual int     getAveragePower()               = 0;
    virtual int     getStateOfCharge()              = 0;
    virtual int16_t getInternalTemperatureC()       = 0;  // Result in 0.1 Celsius
    virtual int     getStateOfHealth()              = 0;
    virtual int     getFullAvailableCapacity()      = 0;
    virtual int     getRemainingCapacity()          = 0;
    virtual int     getDesignCapacity()             = 0;
    virtual void    changeCapacity(int newCapacity) = 0;
    virtual int     getLevel()                      = 0;
    virtual int     getHealth()                     = 0;
    virtual bool    getIsCharging()                 = 0;
    virtual int     remainingLife()                 = 0;  // result in hours

    void setCapacity(int capacity) { m_capacity = capacity; }

    int getCapacity() { return m_capacity; }

 signals:
    void levelChanged();
    void healthChanged();
    void averagePowerChanged();
    void lowBattery();          // Signal emitted around 10%
    void criticalLowBattery();  // At this point the remote needs to shut down
    void isChargingChanged();
    void remainingLifeChanged();
    void chargingDone();

 protected:
    explicit BatteryFuelGauge(QString name, QObject *parent = nullptr) : Device(name, parent) {}

    int m_capacity = 2500;
};
