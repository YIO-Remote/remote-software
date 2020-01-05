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

#include <QObject>
#include <QQmlApplicationEngine>

#include "batterycharger.h"
#include "batteryfuelgauge.h"
#include "displaycontrol.h"
#include "gesturesensor.h"
#include "hapticmotor.h"
#include "interrupthandler.h"
#include "lightsensor.h"
#include "proximitysensor.h"
#include "systemservice.h"
#include "webserver_control.h"
#include "wifi_control.h"

/**
 * @brief Abstract hardware factory. Supported platforms are implemented in concrete factories.
 */
class HardwareFactory : public QObject {
    Q_OBJECT

 public:
    /**
     * @brief build Constructs the singleton hardware factory with a concrete implementation of the factory.
     * @details One of the build operations must be called once before using the singleton instance() accessor.
     * @param configFileName the JSON configuration file name to load the hardware configuration from.
     * @param schemaFileName the JSON schema file to validate the configuration file.
     * @return The concrete instance or nullptr if construction failed. In this case the application should be
     * terminated.
     */
    static HardwareFactory* build(const QString& configFileName, const QString& schemaFileName);

    /**
     * @brief build Constructs the singleton hardware factory with a concrete implementation of the factory.
     * @details One of the build operations must be called once before using the singleton instance() accessor.
     * @param config The configuration map to retrieve build configuration options.
     * @return The concrete instance or nullptr if construction failed. In this case the application should be
     * terminated.
     */
    static HardwareFactory* build(const QVariantMap& config);

    virtual int initialize() = 0;

    /**
     * @brief instance Returns a concrete HardwareFactory implementation
     * @return The hardware factory or nullptr if the factory has not yet been initialized
     */
    static HardwareFactory* instance();

    virtual WifiControl* getWifiControl() = 0;

    virtual SystemService* getSystemService() = 0;

    virtual WebServerControl* getWebServerControl() = 0;

    virtual DisplayControl* getDisplayControl() = 0;

    virtual BatteryCharger* getBatteryCharger() = 0;

    virtual BatteryFuelGauge* getBatteryFuelGauge() = 0;

    virtual InterruptHandler* getInterruptHandler() = 0;

    virtual HapticMotor* getHapticMotor() = 0;

    virtual GestureSensor* getGestureSensor() = 0;

    virtual LightSensor* getLightSensor() = 0;

    virtual ProximitySensor* getProximitySensor() = 0;

    // QML callback providers for qmlRegisterSingletonType
    static QObject* batteryChargerProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
    static QObject* batteryFuelGaugeProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
    static QObject* displayControlProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
    static QObject* interruptHandlerProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
    static QObject* hapticMotorProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
    static QObject* gestureSensorProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
    static QObject* lightSensorProvider(QQmlEngine* engine, QJSEngine* scriptEngine);
    static QObject* proximitySensorProvider(QQmlEngine* engine, QJSEngine* scriptEngine);

 protected:
    explicit HardwareFactory(QObject* parent = nullptr);
    virtual ~HardwareFactory();

    bool virtual buildDevices(const QVariantMap& config) = 0;

 protected slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void onError(Device::DeviceError deviceError, const QString &message);

 private:
    static HardwareFactory* s_instance;
};
