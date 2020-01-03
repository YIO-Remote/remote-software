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

#include <QLoggingCategory>
#include <QtDebug>

#include <mcp23017.h>
#include <wiringPi.h>

#include "../notifications.h"
#include "arm/apds9960gesture.h"
#include "arm/apds9960light.h"
#include "arm/apds9960proximity.h"
#include "arm/batterycharger_yio.h"
#include "arm/bq27441.h"
#include "arm/displaycontrol_yio.h"
#include "arm/drv2605.h"
#include "arm/mcp23017_interrupt.h"
#include "hardwarefactory_yio.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "HwYio");

HardwareFactoryYio::HardwareFactoryYio(const QVariantMap &config, QObject *parent)
    : HardwareFactoryRPi0(config, parent) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // TODO(mze) read i2c device from config
    p_apds9960 = new APDS9960("/dev/i2c-3", APDS9960_ADDRESS, this);
}

int HardwareFactoryYio::initialize() {
    HardwareFactoryRPi0::initialize();

    // WiringPi should only be initialized once (even though there's a failsafe now in newer versions)
    wiringPiSetup();
    mcp23017Setup(100, 0x21);

    // intialize the sensor
    if (!p_apds9960->open()) {
        qCCritical(CLASS_LC) << "Cannot initialise the APDS9960 sensor";
        //: Error message that shows up as notification when light value cannot be read
        Notifications::getInstance()->add(true,
                                          tr("Cannot initialize the proximity sensor. Please restart the remote."));
    } else {
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
        while (!p_apds9960->colorDataReady()) {
            delay(5);
        }
        getLightSensor()->readAmbientLight();
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

    return 0;
}

DisplayControl *HardwareFactoryYio::buildDisplayControl(const QVariantMap &config) {
    Q_UNUSED(config)
    qCDebug(CLASS_LC) << "DisplayControl device: DisplayControlYio";
    return new DisplayControlYio(this);
}

BatteryCharger *HardwareFactoryYio::buildBatteryCharger(const QVariantMap &config) {
    Q_UNUSED(config)
    qCDebug(CLASS_LC) << "BatteryCharger device: BatteryChargerYio";
    return new BatteryChargerYio(this);
}

BatteryFuelGauge *HardwareFactoryYio::buildBatteryFuelGauge(const QVariantMap &config) {
    Q_UNUSED(config)
    qCDebug(CLASS_LC) << "BatteryFuelGauge device: BQ27441";
    // TODO(mze) read i2c device from config
    return new BQ27441("/dev/i2c-3", BQ27441_I2C_ADDRESS, this);
}

InterruptHandler *HardwareFactoryYio::buildInterruptHandler(const QVariantMap &config) {
    Q_UNUSED(config)
    qCDebug(CLASS_LC) << "InterruptHandler device: Mcp23017InterruptHandler";
    // TODO(mze) read i2c device from config
    return new Mcp23017InterruptHandler( "/dev/i2c-3", MCP23017_ADDRESS, this);
}

HapticMotor *HardwareFactoryYio::buildHapticMotor(const QVariantMap &config) {
    Q_UNUSED(config)
    qCDebug(CLASS_LC) << "HapticMotor device: Drv2605";
    // TODO(mze) read i2c device from config
    return new Drv2605("/dev/i2c-3", DRV2605_ADDR, this);
}

GestureSensor *HardwareFactoryYio::buildGestureSensor(const QVariantMap &config) {
    Q_UNUSED(config)
    qCDebug(CLASS_LC) << "GestureSensor device: Apds9960GestureSensor";
    return new Apds9960GestureSensor(p_apds9960, this);
}

LightSensor *HardwareFactoryYio::buildLightSensorr(const QVariantMap &config) {
    Q_UNUSED(config)
    qCDebug(CLASS_LC) << "LightSensor device: Apds9960LightSensor";
    return new Apds9960LightSensor(p_apds9960, this);
}

ProximitySensor *HardwareFactoryYio::buildProximitySensor(const QVariantMap &config) {
    Q_UNUSED(config)
    qCDebug(CLASS_LC) << "ProximitySensor: Apds9960ProximitySensor";
    return new Apds9960ProximitySensor(p_apds9960, this);
}
