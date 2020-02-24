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

#include "hardwarefactory_default.h"

#include <QLoggingCategory>
#include <QtDebug>

#include "../notifications.h"
#include "mock/batterycharger_mock.h"
#include "mock/batteryfuelgauge_mock.h"
#include "mock/displaycontrol_mock.h"
#include "mock/gesturesensor_mock.h"
#include "mock/hapticmotor_mock.h"
#include "mock/interrupthandler_mock.h"
#include "mock/lightsensor_mock.h"
#include "mock/proximitysensor_mock.h"
#include "mock/systemservice_mock.h"
#include "mock/webserver_mock.h"
#include "mock/wifi_mock.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "hw.factory");

HardwareFactoryDefault::HardwareFactoryDefault(QObject *parent)
    : HardwareFactory(parent),
      p_wifiControl(nullptr),
      p_systemService(nullptr),
      p_webServerControl(nullptr),
      p_displayControl(nullptr),
      p_batteryCharger(nullptr),
      p_batteryFuelGauge(nullptr),
      p_interruptHandler(nullptr),
      p_hapticMotor(nullptr),
      p_gestureSensor(nullptr),
      p_lightSensor(nullptr),
      p_proximitySensor(nullptr) {
}

int HardwareFactoryDefault::initialize() {
    int state = 0;
    if (!p_wifiControl->init()) {
        Notifications::getInstance()->add(true, ERR_DEV_INIT_WIFI);
        state = Device::InitializationError;
    }

    // Quick and dirty for now
    getHapticMotor()->open();
    getLightSensor()->open();
    getGestureSensor()->open();
    getBatteryCharger()->open();
    getDisplayControl()->open();
    getProximitySensor()->open();
    getBatteryFuelGauge()->open();
    getInterruptHandler()->open();

    return state;
}

bool HardwareFactoryDefault::buildDevices(const QVariantMap &config) {
    Q_UNUSED(config)

    p_wifiControl = dummyWifiControl();
    p_systemService = dummySystemService();
    p_webServerControl = dummyWebServerControl();
    p_displayControl = dummyDisplayControl();
    p_batteryCharger = dummyBatteryCharger();
    p_batteryFuelGauge = dummyBatteryFuelGauge();
    p_interruptHandler = dummyInterruptHandler();
    p_hapticMotor = dummyHapticMotor();
    p_gestureSensor = dummyGestureSensor();
    p_lightSensor = dummyLightSensor();
    p_proximitySensor = dummyProximitySensor();

    return true;
}

// Dummy default implementations of all devices.
// They may also be used by concrete sub factories needing a default implementation of a driver.
WifiControl *HardwareFactoryDefault::dummyWifiControl() {
    qCDebug(CLASS_LC) << "Using WifiMock";
    return new WifiMock(this);
}

SystemService *HardwareFactoryDefault::dummySystemService() {
    qCDebug(CLASS_LC) << "Using SystemServiceMock";
    return new SystemServiceMock(this);
}

WebServerControl *HardwareFactoryDefault::dummyWebServerControl() {
    qCDebug(CLASS_LC) << "Using WebServerMock";
    return new WebServerMock(this);
}

DisplayControl *HardwareFactoryDefault::dummyDisplayControl() {
    qCDebug(CLASS_LC) << "Using DisplayControlMock";
    return new DisplayControlMock(this);
}

BatteryCharger *HardwareFactoryDefault::dummyBatteryCharger() {
    qCDebug(CLASS_LC) << "Using BatteryChargerMock";
    return new BatteryChargerMock(this);
}

BatteryFuelGauge *HardwareFactoryDefault::dummyBatteryFuelGauge() {
    qCDebug(CLASS_LC) << "Using BatteryFuelGaugeMock";
    return new BatteryFuelGaugeMock(this);
}

InterruptHandler *HardwareFactoryDefault::dummyInterruptHandler() {
    qCDebug(CLASS_LC) << "Using InterruptHandlerMock";
    return new InterruptHandlerMock(this);
}

HapticMotor *HardwareFactoryDefault::dummyHapticMotor() {
    qCDebug(CLASS_LC) << "Using HapticMotorMock";
    return new HapticMotorMock(this);
}

GestureSensor *HardwareFactoryDefault::dummyGestureSensor() {
    qCDebug(CLASS_LC) << "Using GestureSensorMock";
    return new GestureSensorMock(this);
}

LightSensor *HardwareFactoryDefault::dummyLightSensor() {
    qCDebug(CLASS_LC) << "Using LightSensorMock";
    return new LightSensorMock(this);
}

ProximitySensor *HardwareFactoryDefault::dummyProximitySensor() {
    qCDebug(CLASS_LC) << "Using ProximitySensorMock";
    return new ProximitySensorMock(this);
}
