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

#include "hardwarefactory_default.h"

// TODO(zehnm) This is more a Linux factory than a specific Raspberry Pi factory.
//             Should be renamed and once there is specific RPi functionality create a dedicated factory.
/**
 * @brief Concrete hardware factory implementation for the Raspberry Pi Zero in the YIO Remote.
 */
class HardwareFactoryRPi0 : public HardwareFactoryDefault {
    Q_OBJECT

 public:
    explicit HardwareFactoryRPi0(const QVariantMap &config, QObject *parent = nullptr);

    // HardwareFactory interface
 protected:
    bool buildDevices(const QVariantMap &config) override;

    // Helper methods
 protected:
    virtual WifiControl *     buildWifiControl(const QVariantMap &config);
    virtual SystemService *   buildSystemService(const QVariantMap &config);
    virtual WebServerControl *buildWebServerControl(const QVariantMap &config);
    virtual DisplayControl *  buildDisplayControl(const QVariantMap &config);
    virtual BatteryCharger *  buildBatteryCharger(const QVariantMap &config);
    virtual BatteryFuelGauge *buildBatteryFuelGauge(const QVariantMap &config);
    virtual InterruptHandler *buildInterruptHandler(const QVariantMap &config);
    virtual HapticMotor *     buildHapticMotor(const QVariantMap &config);
    virtual GestureSensor *   buildGestureSensor(const QVariantMap &config);
    virtual LightSensor *     buildLightSensor(const QVariantMap &config);
    virtual ProximitySensor * buildProximitySensor(const QVariantMap &config);
};
