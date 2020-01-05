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

#include <QLoggingCategory>
#include <QtDebug>

#include "batterycharger_mock.h"
#include "batteryfuelgauge_mock.h"
#include "displaycontrol_mock.h"
#include "gesturesensor_mock.h"
#include "hapticmotor_mock.h"
#include "hardwarefactory_mock.h"
#include "interrupthandler_mock.h"
#include "lightsensor_mock.h"
#include "proximitysensor_mock.h"
#include "systemservice_mock.h"
#include "webserver_mock.h"
#include "wifi_mock.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "hw.factory.mock");

HardwareFactoryMock::HardwareFactoryMock(const QVariantMap &config, QObject *parent) : HardwareFactory(parent) {
    Q_UNUSED(config)

    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

WifiControl *HardwareFactoryMock::getWifiControl() {
    static WifiMock singleton;
    return &singleton;
}

SystemService *HardwareFactoryMock::getSystemService() {
    static SystemServiceMock singleton;
    return &singleton;
}

WebServerControl *HardwareFactoryMock::getWebServerControl() {
    static WebServerMock singleton;
    return &singleton;
}

DisplayControl *HardwareFactoryMock::getDisplayControl() {
    static DisplayControlMock singleton;
    return &singleton;
}

BatteryCharger *HardwareFactoryMock::getBatteryCharger() {
    static BatteryChargerMock singleton;
    return &singleton;
}

BatteryFuelGauge *HardwareFactoryMock::getBatteryFuelGauge() {
    static BatteryFuelGaugeMock singleton;
    return &singleton;
}

InterruptHandler *HardwareFactoryMock::getInterruptHandler() {
    static InterruptHandlerMock singleton;
    return &singleton;
}

HapticMotor *HardwareFactoryMock::getHapticMotor() {
    static HapticMotorMock singleton;
    return &singleton;
}

GestureSensor *HardwareFactoryMock::getGestureSensor() {
    static GestureSensorMock singleton;
    return &singleton;
}

LightSensor *HardwareFactoryMock::getLightSensor() {
    static LightSensorMock singleton;
    return &singleton;
}

ProximitySensor *HardwareFactoryMock::getProximitySensor() {
    static ProximitySensorMock singleton;
    return &singleton;
}
