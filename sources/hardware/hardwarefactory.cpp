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

#include "hardwarefactory.h"

#include <QLoggingCategory>
#include <QtDebug>

#include "../config.h"
#include "../jsonfile.h"
#include "../notifications.h"

#if defined(Q_OS_ANDROID)
#include "android/hw_factory_android.h"
#elif defined(Q_OS_LINUX)
#if defined(Q_PROCESSOR_ARM)
#include "linux/arm/hw_factory_yio.h"
#else
#include "linux/hw_factory_linux.h"
#endif
#elif defined(Q_OS_WIN)
#include "windows/hw_factory_win.h"
#elif defined(Q_OS_MACOS)
#include "macos/hw_factory_mac.h"
#else
#include "hardwarefactory_default.h"
#endif

static Q_LOGGING_CATEGORY(CLASS_LC, "hw.factory");

HardwareFactory *HardwareFactory::s_instance = nullptr;

HardwareFactory::HardwareFactory(QObject *parent) : QObject(parent) {}

HardwareFactory::~HardwareFactory() { s_instance = nullptr; }

void HardwareFactory::onError(Device::DeviceError deviceError, const QString &message) {
    if (deviceError == Device::InitializationError || deviceError == Device::CommunicationError) {
        Notifications::getInstance()->add(true, message);
    }
}

HardwareFactory *HardwareFactory::build(const QString &configFileName, const QString &schemaFileName,
                                        const QString &profile) {
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
    return build(config, profile);
}

HardwareFactory *HardwareFactory::build(const QVariantMap &config, const QString &profile) {
    if (s_instance != nullptr) {
        qCCritical(CLASS_LC)
            << "BUG ALERT: Invalid program flow! HardwareFactory already initialized, ignoring build() call.";
        return s_instance;
    }

    // KISS: sufficient for now, custom logic possible with config interface when needed.
#if defined(Q_OS_ANDROID)
    Q_UNUSED(profile)
    s_instance = new HardwareFactoryAndroid(config);
#elif defined(Q_OS_LINUX)
#if defined(Q_PROCESSOR_ARM)
    s_instance = new HardwareFactoryYio(config, profile);
#else
    s_instance = new HardwareFactoryLinux(config, profile);
#endif
#elif defined(Q_OS_WIN)
    Q_UNUSED(profile)
    s_instance = new HardwareFactoryWindows(config);
#elif defined(Q_OS_MACOS)
    Q_UNUSED(profile)
    s_instance = new HardwareFactoryMacOS(config);
#else
    Q_UNUSED(profile)
    s_instance = new HardwareFactoryDefault();
#endif

    s_instance->buildDevices(config);
    return s_instance;
}

HardwareFactory *HardwareFactory::instance() {
    Q_ASSERT(s_instance);

    return s_instance;
}

QObject *HardwareFactory::batteryChargerProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    Q_ASSERT(s_instance);

    QObject *device = s_instance->getBatteryCharger();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::batteryFuelGaugeProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    Q_ASSERT(s_instance);

    QObject *device = s_instance->getBatteryFuelGauge();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::displayControlProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    Q_ASSERT(s_instance);

    QObject *device = s_instance->getDisplayControl();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::interruptHandlerProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    Q_ASSERT(s_instance);

    QObject *device = s_instance->getInterruptHandler();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::hapticMotorProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    Q_ASSERT(s_instance);

    QObject *device = s_instance->getHapticMotor();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::gestureSensorProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    Q_ASSERT(s_instance);

    QObject *device = s_instance->getGestureSensor();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::lightSensorProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    Q_ASSERT(s_instance);

    QObject *device = s_instance->getLightSensor();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}

QObject *HardwareFactory::proximitySensorProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)
    Q_ASSERT(s_instance);

    QObject *device = s_instance->getProximitySensor();
    engine->setObjectOwnership(device, QQmlEngine::CppOwnership);
    return device;
}
