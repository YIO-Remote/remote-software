/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
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

#include "hardwarefactory.h"

/**
 * @brief Default implementation of the HardwareFactory interface. It is the simplest concrete hardware factory creating
 * dummy implementations for all devices.
 */
class HardwareFactoryDefault : public HardwareFactory {
    Q_OBJECT

 public:
    explicit HardwareFactoryDefault(QObject *parent = nullptr);

    // HardwareFactory interface
 public:
    int initialize() override;

    WifiControl *getWifiControl() override {
        Q_ASSERT(p_wifiControl);
        return p_wifiControl;
    }

    SystemService *getSystemService() override {
        Q_ASSERT(p_systemService);
        return p_systemService;
    }

    WebServerControl *getWebServerControl() override {
        Q_ASSERT(p_webServerControl);
        return p_webServerControl;
    }

    DisplayControl *getDisplayControl() override {
        Q_ASSERT(p_displayControl);
        return p_displayControl;
    }

    BatteryCharger *getBatteryCharger() override {
        Q_ASSERT(p_batteryCharger);
        return p_batteryCharger;
    }

    BatteryFuelGauge *getBatteryFuelGauge() override {
        Q_ASSERT(p_batteryFuelGauge);
        return p_batteryFuelGauge;
    }

    InterruptHandler *getInterruptHandler() override {
        Q_ASSERT(p_interruptHandler);
        return p_interruptHandler;
    }

    HapticMotor *getHapticMotor() override {
        Q_ASSERT(p_hapticMotor);
        return p_hapticMotor;
    }

    GestureSensor *getGestureSensor() override {
        Q_ASSERT(p_gestureSensor);
        return p_gestureSensor;
    }

    LightSensor *getLightSensor() override {
        Q_ASSERT(p_lightSensor);
        return p_lightSensor;
    }

    ProximitySensor *getProximitySensor() override {
        Q_ASSERT(p_proximitySensor);
        return p_proximitySensor;
    }

    // HardwareFactory interface
 protected:
    bool buildDevices(const QVariantMap &config) override;

    // Helper methods
 protected:
    /**
     * @brief Returns a mock implementation of the WifiControl device.
     */
    WifiControl *dummyWifiControl();
    /**
     * @brief Returns a mock implementation of the SystemService device.
     */
    SystemService *dummySystemService();
    /**
     * @brief Returns a mock implementation of the WebServerControl device.
     */
    WebServerControl *dummyWebServerControl();
    /**
     * @brief Returns a mock implementation of the DisplayControl device.
     */
    DisplayControl *dummyDisplayControl();
    /**
     * @brief Returns a mock implementation of the BatteryCharger device.
     */
    BatteryCharger *dummyBatteryCharger();
    /**
     * @brief Returns a mock implementation of the BatteryFuelGauge device.
     */
    BatteryFuelGauge *dummyBatteryFuelGauge();
    /**
     * @brief Returns a mock implementation of the InterruptHandler device.
     */
    InterruptHandler *dummyInterruptHandler();
    /**
     * @brief Returns a mock implementation of the HapticMotor device.
     */
    HapticMotor *dummyHapticMotor();
    /**
     * @brief Returns a mock implementation of the GestureSensor device.
     */
    GestureSensor *dummyGestureSensor();
    /**
     * @brief Returns a mock implementation of the LightSensor device.
     */
    LightSensor *dummyLightSensor();
    /**
     * @brief Returns a mock implementation of the ProximitySensor device.
     */
    ProximitySensor *dummyProximitySensor();

 protected:
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
