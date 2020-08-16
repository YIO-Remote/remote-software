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

#include "../hw_factory_linux.h"
#include "apds9960.h"

/**
 * @brief Concrete hardware factory for the YIO Remote running on Raspberry Pi Zero remote-os.
 */
class HardwareFactoryYio : public HardwareFactoryLinux {
    Q_OBJECT

 public:
    explicit HardwareFactoryYio(const QVariantMap &config, const QString &profile, QObject *parent = nullptr);

    // HardwareFactory interface
 public:
    int initialize() override;

 protected:
    bool buildDevices(const QVariantMap &config) override;

    // Local helper methods
 private:
    bool initializeWiringPi(const QVariantMap &config);

    InterruptHandler *buildInterruptHandler(const QVariantMap &config);
    DisplayControl *  buildDisplayControl(const QVariantMap &config);
    BatteryCharger *  buildBatteryCharger(const QVariantMap &config);
    BatteryFuelGauge *buildBatteryFuelGauge(const QVariantMap &config);
    HapticMotor *     buildHapticMotor(const QVariantMap &config);
    GestureSensor *   buildGestureSensor(const QVariantMap &config);
    LightSensor *     buildLightSensor(const QVariantMap &config);
    ProximitySensor * buildProximitySensor(const QVariantMap &config);

 private:
    // shared device for gesture / light / proximity sensors
    APDS9960 *p_apds9960;

    // default values for YIO
    static const QString DEF_I2C_DEVICE;
    static const int     DEF_BATTERY_CAPACITY;
};
