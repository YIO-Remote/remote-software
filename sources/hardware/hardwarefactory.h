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
     * @brief Constructs the singleton hardware factory with a concrete implementation of the factory.
     * @details One of the build operations must be called once before using the singleton instance() accessor.
     * @param configFileName the JSON configuration file name to load the hardware configuration from.
     * @param schemaFileName the JSON schema file to validate the configuration file.
     * @param profile Optional profile name for further customization.
     * @return The concrete instance or nullptr if construction failed. In this case the application MUST be
     * terminated.
     */
    static HardwareFactory* build(const QString& configFileName, const QString& schemaFileName, const QString& profile);

    /**
     * @brief Constructs the singleton hardware factory with a concrete implementation of the factory.
     * @details One of the build operations must be called once before using the singleton instance() accessor.
     * @param config The configuration map to retrieve build configuration options.
     * @param profile Optional profile name for further customization.
     * @return The concrete instance or nullptr if construction failed. In this case the application MUST be
     * terminated.
     */
    static HardwareFactory* build(const QVariantMap& config, const QString& profile);

    /**
     * @brief Initialization hook function for the concrete factory to initialize all devices.
     * All devices must be ready to use after this call.
     * @return Initialization error code. 0 = no error.
     */
    virtual int initialize() = 0;

    /**
     * @brief Returns a concrete HardwareFactory implementation.
     * @return The hardware factory or nullptr if the factory has not yet been initialized.
     */
    static HardwareFactory* instance();

    /**
     * @brief Returns the concrete WifiControl device implementation.
     * @details Needs to be overriden by a concrete hardware factory.
     */
    virtual WifiControl* getWifiControl() = 0;

    /**
     * @brief Returns the concrete SystemService device implementation.
     * @details Needs to be overriden by a concrete hardware factory.
     */
    virtual SystemService* getSystemService() = 0;

    /**
     * @brief Returns the concrete WebServerControl device implementation.
     * @details Needs to be overriden by a concrete hardware factory.
     */
    virtual WebServerControl* getWebServerControl() = 0;

    /**
     * @brief Returns the concrete DisplayControl device implementation.
     * @details Needs to be overriden by a concrete hardware factory.
     */
    virtual DisplayControl* getDisplayControl() = 0;

    /**
     * @brief Returns the concrete BatteryCharger device implementation.
     * @details Needs to be overriden by a concrete hardware factory.
     */
    virtual BatteryCharger* getBatteryCharger() = 0;

    /**
     * @brief Returns the concrete BatteryFuelGauge device implementation.
     * @details Needs to be overriden by a concrete hardware factory.
     */
    virtual BatteryFuelGauge* getBatteryFuelGauge() = 0;

    /**
     * @brief Returns the concrete InterruptHandler device implementation.
     * @details Needs to be overriden by a concrete hardware factory.
     */
    virtual InterruptHandler* getInterruptHandler() = 0;

    /**
     * @brief Returns the concrete HapticMotor device implementation.
     * @details Needs to be overriden by a concrete hardware factory.
     */
    virtual HapticMotor* getHapticMotor() = 0;

    /**
     * @brief Returns the concrete GestureSensor device implementation.
     * @details Needs to be overriden by a concrete hardware factory.
     */
    virtual GestureSensor* getGestureSensor() = 0;

    /**
     * @brief Returns the concrete LightSensor device implementation.
     * @details Needs to be overriden by a concrete hardware factory.
     */
    virtual LightSensor* getLightSensor() = 0;

    /**
     * @brief Returns the concrete ProximitySensor device implementation.
     * @details Needs to be overriden by a concrete hardware factory.
     */
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

    /**
     * @brief Hook function for the concrete implementation to construct all devices before the client can access an
     * instance of the factory.
     * @details This method is called from the build() operation.
     * @see initialize() for device initialization.
     * @param config The configuration map to retrieve build configuration options.
     * @return
     */
    virtual bool buildDevices(const QVariantMap& config) = 0;

 protected slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    /**
     * @brief Error callback function for device drivers. InitializationError and CommunicationError are propagated as
     * user notifications.
     * @param deviceError The error type.
     * @param message A human readable message to display. Preferrably a translated text.
     */
    void onError(Device::DeviceError deviceError, const QString& message);

 private:
    static HardwareFactory* s_instance;
};
