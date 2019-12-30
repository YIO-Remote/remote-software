/******************************************************************************
 *
 * Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
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

#include <assert.h>

#include <QLoggingCategory>
#include <QtDebug>

#include "../jsonfile.h"
#include "hardwarefactory.h"

#if defined(__arm__)
#include "hardwarefactory_yio.h"
#elif defined(Q_OS_LINUX)
#include "hardwarefactory_rpi0.h"
#else
#include "hardwarefactory_mock.h"
#endif

static Q_LOGGING_CATEGORY(CLASS_LC, "HwFactory");

HardwareFactory *HardwareFactory::s_instance = nullptr;

HardwareFactory::HardwareFactory(QObject *parent) : QObject(parent) {}

HardwareFactory::~HardwareFactory() { s_instance = nullptr; }

HardwareFactory *HardwareFactory::build(const QString &configFileName, const QString &schemaFileName) {
    JsonFile hwCfg(configFileName, schemaFileName);

    QVariantMap config = hwCfg.read().toMap();
    if (!hwCfg.isValid()) {
        // FIXME decide how to proceed:
        // a) use an empty configuration for build to use default values
        // b) return null and let caller handle this as a fatal error.
        qCCritical(CLASS_LC).noquote() << "Invalid hardware configuration!"
                                       << "Ignoring configuration file and using default values. Errors:" << endl
                                       << hwCfg.error();
        config.clear();
    }
    return build(config);
}

HardwareFactory *HardwareFactory::build(const QVariantMap &config) {
    if (s_instance != nullptr) {
        qCCritical(CLASS_LC) << "BUG ALERT: Invalid program flow!"
                             << "HardwareFactory already initialized, ignoring build() call.";
        return s_instance;
    }

    // KISS: sufficient for now, custom logic possible with config interface when needed.
#if defined(__arm__)
    s_instance = new HardwareFactoryYio(config);
#elif defined(Q_OS_LINUX)
    s_instance = new HardwareFactoryRPi0(config);
#else  // anyone wants to write Android, macOS or Windows factories?
    s_instance = new HardwareFactoryMock(config);
#endif

    return s_instance;
}

HardwareFactory *HardwareFactory::instance() {
    assert(s_instance);

    return s_instance;
}

QObject *HardwareFactory::batteryFuelGaugeProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    assert(s_instance);

    QObject *device = s_instance->getBatteryFuelGauge();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::displayControlProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    assert(s_instance);

    QObject *device = s_instance->getDisplayControl();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::interruptHandlerProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    assert(s_instance);

    QObject *device = s_instance->getInterruptHandler();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::hapticMotorProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    assert(s_instance);

    QObject *device = s_instance->getHapticMotor();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::gestureSensorProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    assert(s_instance);

    QObject *device = s_instance->getGestureSensor();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::lightSensorProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    assert(s_instance);

    QObject *device = s_instance->getLightSensor();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::proximitySensorProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    assert(s_instance);

    QObject *device = s_instance->getProximitySensor();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}
