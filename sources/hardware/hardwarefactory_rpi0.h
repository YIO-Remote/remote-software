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

#include "displaycontrol.h"
#include "hardwarefactory.h"
#include "webserver_control.h"

/**
 * @brief Concrete hardware factory implementation for the Raspberry Pi Zero in the YIO Remote.
 */
class HardwareFactoryRPi0 : public HardwareFactory {
    Q_OBJECT

 public:
    explicit HardwareFactoryRPi0(const QVariantMap &config, QObject *parent = nullptr);

    // HardwareFactory interface
 public:
    int initialize() override;

    WifiControl *getWifiControl() override {
        assert(p_wifiControl);
        return p_wifiControl;
    }

    SystemService *getSystemService() override {
        assert(p_systemService);
        return p_systemService;
    }

    WebServerControl *getWebServerControl() override {
        assert(p_webServerControl);
        return p_webServerControl;
    }

    DisplayControl *getDisplayControl() override {
        assert(p_displayControl);
        return p_displayControl;
    }

    BatteryCharger *getBatteryCharger() override {
        assert(p_batteryCharger);
        return p_batteryCharger;
    }

    BatteryFuelGauge *getBatteryFuelGauge() override {
        assert(p_batteryFuelGauge);
        return p_batteryFuelGauge;
    }

    InterruptHandler *getInterruptHandler() override {
        assert(p_interruptHandler);
        return p_interruptHandler;
    }

    HapticMotor *getHapticMotor() override {
        assert(p_hapticMotor);
        return p_hapticMotor;
    }

    GestureSensor *getGestureSensor() override {
        assert(p_gestureSensor);
        return p_gestureSensor;
    }

    LightSensor *getLightSensor() override {
        assert(p_lightSensor);
        return p_lightSensor;
    }

    ProximitySensor *getProximitySensor() override {
        assert(p_proximitySensor);
        return p_proximitySensor;
    }

 protected:
    bool buildDevices(const QVariantMap& config) override;

    virtual WifiControl *     buildWifiControl(const QVariantMap &config);
    virtual SystemService *   buildSystemService(const QVariantMap &config);
    virtual WebServerControl *buildWebServerControl(const QVariantMap &config);
    virtual DisplayControl *  buildDisplayControl(const QVariantMap &config);
    virtual BatteryCharger   *buildBatteryCharger(const QVariantMap &config);
    virtual BatteryFuelGauge *buildBatteryFuelGauge(const QVariantMap &config);
    virtual InterruptHandler *buildInterruptHandler(const QVariantMap &config);
    virtual HapticMotor *     buildHapticMotor(const QVariantMap &config);
    virtual GestureSensor *   buildGestureSensor(const QVariantMap &config);
    virtual LightSensor *     buildLightSensorr(const QVariantMap &config);
    virtual ProximitySensor * buildProximitySensor(const QVariantMap &config);

 private:
    WifiControl *     p_wifiControl;
    SystemService *   p_systemService;
    WebServerControl *p_webServerControl;
    DisplayControl *  p_displayControl;
    BatteryCharger *  p_batteryCharger;
    BatteryFuelGauge *p_batteryFuelGauge;
    InterruptHandler *p_interruptHandler;
    HapticMotor *     p_hapticMotor;
    GestureSensor *   p_gestureSensor;
    LightSensor *     p_lightSensor;
    ProximitySensor * p_proximitySensor;
};
