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

#include "arm/apds9960.h"
#include "hardwarefactory_rpi0.h"

class HardwareFactoryYio : public HardwareFactoryRPi0 {
    Q_OBJECT

 public:
    explicit HardwareFactoryYio(const QVariantMap &config, QObject *parent = nullptr);

    int initialize() override;

    // HardwareFactoryRPi0 interface
 protected:
    DisplayControl *buildDisplayControl(const QVariantMap &config) override;

    BatteryCharger *buildBatteryCharger(const QVariantMap &config) override;

    BatteryFuelGauge *buildBatteryFuelGauge(const QVariantMap &config) override;

    InterruptHandler *buildInterruptHandler(const QVariantMap &config) override;

    HapticMotor *buildHapticMotor(const QVariantMap &config) override;

    GestureSensor *buildGestureSensor(const QVariantMap &config) override;

    LightSensor *buildLightSensor(const QVariantMap &config) override;

    ProximitySensor *buildProximitySensor(const QVariantMap &config) override;

 private:
    // shared device for gesture / light / proximity sensors
    APDS9960 *p_apds9960;

    // default values for YIO
    static const QString DEF_I2C_DEVICE;
};
