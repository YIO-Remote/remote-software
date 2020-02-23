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

#include "hw_factory_yio.h"

#include <mcp23017.h>
#include <wiringPi.h>

#include <QDateTime>
#include <QLoggingCategory>
#include <QtDebug>

#include "../../../configutil.h"
#include "../../hw_config.h"
#include "apds9960gesture.h"
#include "apds9960light.h"
#include "apds9960proximity.h"
#include "batterycharger_yio.h"
#include "bq27441.h"
#include "displaycontrol_yio.h"
#include "drv2605.h"
#include "mcp23017_interrupt.h"

const QString HardwareFactoryYio::DEF_I2C_DEVICE = "/dev/i2c-3";

static Q_LOGGING_CATEGORY(CLASS_LC, "hw.factory.yio");

HardwareFactoryYio::HardwareFactoryYio(const QVariantMap &config, const QString &profile, QObject *parent)
    : HardwareFactoryLinux(config, profile, parent) {
    // gesture, light and proximity are handled by the same hardware sensor!
    // The device needs to be shared among three device abstractions.
    QVariantMap deviceCfg = ConfigUtil::getValue(config, HW_CFG_PROXIMITY).toMap();
    if (ConfigUtil::isEnabled(deviceCfg)) {
        auto dev = ConfigUtil::getValue(deviceCfg, HW_CFG_PATH_I2C_DEV, DEF_I2C_DEVICE).toString();
        auto id = ConfigUtil::getValue(deviceCfg, HW_CFG_PATH_I2C_ID, APDS9960_ADDRESS).toInt();

        p_apds9960 = new APDS9960(dev, id, this);
    } else {
        p_apds9960 = nullptr;
    }
}

bool HardwareFactoryYio::buildDevices(const QVariantMap &config) {
    QVariantMap deviceCfg = ConfigUtil::getValue(config, "wiringPi").toMap();
    if (ConfigUtil::isEnabled(deviceCfg)) {
        initializeWiringPi(deviceCfg);
    }

    // InterruptHandler is used by other devices, so make sure it's built first and available!
    deviceCfg = ConfigUtil::getValue(config, HW_CFG_BTN_INTR_HANDLER).toMap();
    p_interruptHandler = ConfigUtil::isEnabled(deviceCfg) ? buildInterruptHandler(deviceCfg) : dummyInterruptHandler();

    deviceCfg = ConfigUtil::getValue(config, HW_CFG_SYSTEMSERVICE).toMap();
    p_systemService = ConfigUtil::isEnabled(deviceCfg) ? buildSystemService(deviceCfg) : dummySystemService();

    deviceCfg = ConfigUtil::getValue(config, HW_CFG_WEBSERVER).toMap();
    p_webServerControl = ConfigUtil::isEnabled(deviceCfg) ? buildWebServerControl(deviceCfg) : dummyWebServerControl();

    deviceCfg = ConfigUtil::getValue(config, HW_CFG_WIFI).toMap();
    p_wifiControl = ConfigUtil::isEnabled(deviceCfg) ? buildWifiControl(deviceCfg) : dummyWifiControl();

    deviceCfg = ConfigUtil::getValue(config, HW_CFG_DISPLAY_CONTROL).toMap();
    p_displayControl = ConfigUtil::isEnabled(deviceCfg) ? buildDisplayControl(deviceCfg) : dummyDisplayControl();

    deviceCfg = ConfigUtil::getValue(config, HW_CFG_BATTERY_CHARGER).toMap();
    p_batteryCharger = ConfigUtil::isEnabled(deviceCfg) ? buildBatteryCharger(deviceCfg) : dummyBatteryCharger();

    deviceCfg = ConfigUtil::getValue(config, HW_CFG_BATTERY_FUEL_GAUGE).toMap();
    p_batteryFuelGauge = ConfigUtil::isEnabled(deviceCfg) ? buildBatteryFuelGauge(deviceCfg) : dummyBatteryFuelGauge();

    deviceCfg = ConfigUtil::getValue(config, HW_CFG_HAPTIC_MOTOR).toMap();
    p_hapticMotor = ConfigUtil::isEnabled(deviceCfg) ? buildHapticMotor(deviceCfg) : dummyHapticMotor();

    deviceCfg = ConfigUtil::getValue(config, HW_CFG_GESTURE).toMap();
    p_gestureSensor = ConfigUtil::isEnabled(deviceCfg) ? buildGestureSensor(deviceCfg) : dummyGestureSensor();

    deviceCfg = ConfigUtil::getValue(config, HW_CFG_LIGHT).toMap();
    p_lightSensor = ConfigUtil::isEnabled(deviceCfg) ? buildLightSensor(deviceCfg) : dummyLightSensor();

    deviceCfg = ConfigUtil::getValue(config, HW_CFG_PROXIMITY).toMap();
    p_proximitySensor = ConfigUtil::isEnabled(deviceCfg) ? buildProximitySensor(deviceCfg) : dummyProximitySensor();

    return true;
}

bool HardwareFactoryYio::initializeWiringPi(const QVariantMap &config) {
    // WiringPi should only be initialized once (even though there's a failsafe now in newer versions)
    // Attention:
    // - wiringPiSetup() terminates the app in case of failure!
    // - wiringPiSetup() needs to be called with root privileges!
    // TODO(zehnm) Verify if wiringPiSetupSys() could be used with remote-os
    wiringPiSetup();

    // The 1st IO expander for the buttons is directly handled with an interrupt and i2c access.
    // The 2nd IO expander for display SPI and battery charger is accessed through WiringPi
    auto pinBase = ConfigUtil::getValue(config, "ioExpander/pinBase", 100).toInt();
    auto id = ConfigUtil::getValue(config, "ioExpander/i2cId", MCP23017_ADDRESS2).toInt();
    qCDebug(CLASS_LC) << "Initializing 2nd MCP23017ML IO expander using WiringPi on i2c address" << id
                      << "with pin base" << pinBase;

    if (mcp23017Setup(pinBase, id) == 0) {
        qCCritical(CLASS_LC) << "Error initializing 2nd MCP23017ML IO expander! i2c id:" << id
                             << "pin base:" << pinBase;
        return false;
    }

    return true;
}

int HardwareFactoryYio::initialize() {
    // intialize the proximity sensor
    if (p_apds9960) {
        connect(p_apds9960, &Device::error, this, &HardwareFactoryYio::onError);
        if (p_apds9960->open()) {
            delay(100);

            // turn on the light sensor
            p_apds9960->enableColor(true);

            // turn on proximity sensor
            p_apds9960->enableProximity(true);

            // set the proximity threshold
            p_apds9960->setProximityInterruptThreshold(0, uint8_t(getProximitySensor()->proximitySetting()), 1);

            // set the proximity gain
            p_apds9960->setProxGain(APDS9960_PGAIN_2X);

            // m_apds.setLED(APDS9960_LEDDRIVE_100MA, APDS9960_LEDBOOST_200PCNT);
            p_apds9960->setLED(APDS9960_LEDDRIVE_25MA, APDS9960_LEDBOOST_100PCNT);

            // read ambient light
            qint64 timeout = QDateTime::currentMSecsSinceEpoch() + 3000;
            while (!p_apds9960->colorDataReady()) {
                delay(5);
                if (QDateTime::currentMSecsSinceEpoch() > timeout) {
                    qCWarning(CLASS_LC) << "Error retrieving color data from APDS9960 sensor";
                    break;
                }
            }
            getLightSensor()->readAmbientLight();
        }
    }

    return HardwareFactoryLinux::initialize();
}

DisplayControl *HardwareFactoryYio::buildDisplayControl(const QVariantMap &config) {
    auto pin = ConfigUtil::getValue(config, HW_CFG_PATH_GPIO_PIN, 26).toInt();

    DisplayControl *device = new DisplayControlYio(pin, this);
    connect(device, &Device::error, this, &HardwareFactoryYio::onError);

    return device;
}

BatteryCharger *HardwareFactoryYio::buildBatteryCharger(const QVariantMap &config) {
    auto pin = ConfigUtil::getValue(config, HW_CFG_PATH_GPIO_PIN, 108).toInt();

    BatteryCharger *device = new BatteryChargerYio(pin, this);
    connect(device, &Device::error, this, &HardwareFactoryYio::onError);

    return device;
}

BatteryFuelGauge *HardwareFactoryYio::buildBatteryFuelGauge(const QVariantMap &config) {
    auto dev = ConfigUtil::getValue(config, HW_CFG_PATH_I2C_DEV, DEF_I2C_DEVICE).toString();
    auto id = ConfigUtil::getValue(config, HW_CFG_PATH_I2C_ID, BQ27441_I2C_ADDRESS).toInt();

    BatteryFuelGauge *device = new BQ27441(getInterruptHandler(), dev, id, this);
    connect(device, &Device::error, this, &HardwareFactoryYio::onError);

    return device;
}

InterruptHandler *HardwareFactoryYio::buildInterruptHandler(const QVariantMap &config) {
    auto dev = ConfigUtil::getValue(config, HW_CFG_PATH_I2C_DEV, DEF_I2C_DEVICE).toString();
    auto id = ConfigUtil::getValue(config, HW_CFG_PATH_I2C_ID, MCP23017_ADDRESS).toInt();
    auto gpio = ConfigUtil::getValue(config, HW_CFG_PATH_INTR_GPIO_PIN, 18).toInt();

    InterruptHandler *device = new Mcp23017InterruptHandler(dev, id, gpio, this);
    connect(device, &Device::error, this, &HardwareFactoryYio::onError);

    return device;
}

HapticMotor *HardwareFactoryYio::buildHapticMotor(const QVariantMap &config) {
    auto dev = ConfigUtil::getValue(config, HW_CFG_PATH_I2C_DEV, DEF_I2C_DEVICE).toString();
    auto id = ConfigUtil::getValue(config, HW_CFG_PATH_I2C_ID, DRV2605_ADDR).toInt();

    HapticMotor *device = new Drv2605(dev, id, this);
    connect(device, &Device::error, this, &HardwareFactoryYio::onError);

    return device;
}

GestureSensor *HardwareFactoryYio::buildGestureSensor(const QVariantMap &config) {
    Q_UNUSED(config)
    GestureSensor *device = p_apds9960 ? new Apds9960GestureSensor(p_apds9960, this) : dummyGestureSensor();
    connect(device, &Device::error, this, &HardwareFactoryYio::onError);

    return device;
}

LightSensor *HardwareFactoryYio::buildLightSensor(const QVariantMap &config) {
    Q_UNUSED(config)
    LightSensor *device = p_apds9960 ? new Apds9960LightSensor(p_apds9960, this) : dummyLightSensor();
    connect(device, &Device::error, this, &HardwareFactoryYio::onError);

    return device;
}

ProximitySensor *HardwareFactoryYio::buildProximitySensor(const QVariantMap &config) {
    Q_UNUSED(config)
    ProximitySensor *device =
        p_apds9960 ? new Apds9960ProximitySensor(p_apds9960, getInterruptHandler(), this) : dummyProximitySensor();
    connect(device, &Device::error, this, &HardwareFactoryYio::onError);

    return device;
}
