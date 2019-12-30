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

#include "../notifications.h"
#include "arm/apds9960gesture.h"
#include "arm/apds9960light.h"
#include "arm/apds9960proximity.h"
#include "arm/bq27441.h"
#include "arm/displaycontrol_yio.h"
#include "arm/drv2605.h"
#include "arm/mcp23017_interrupt.h"
#include "hardwarefactory_yio.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "HwYio");

HardwareFactoryYio::HardwareFactoryYio(const QVariantMap &config, QObject *parent)
    : HardwareFactoryRPi0(config, parent) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // FIXME(mze) create initialization method / hook
    // intialize the sensor
    if (!m_apds.begin()) {
        qCritical() << "Cannot initialise the APDS9960 sensor";
        //: Error message that shows up as notification when light value cannot be read
        Notifications::getInstance()->add(true,
                                          tr("Cannot initialize the proximity sensor. Please restart the remote."));
        return;
    }

    delay(100);

    // turn on the light sensor
    m_apds.enableColor(true);

    // turn on proximity sensor
    m_apds.enableProximity(true);

    // set the proximity threshold
    m_apds.setProximityInterruptThreshold(0, uint8_t(getProximitySensor()->proximitySetting()), 1);

    // set the proximity gain
    m_apds.setProxGain(APDS9960_PGAIN_2X);

    // m_apds.setLED(APDS9960_LEDDRIVE_100MA, APDS9960_LEDBOOST_200PCNT);
    m_apds.setLED(APDS9960_LEDDRIVE_25MA, APDS9960_LEDBOOST_100PCNT);

    // read ambient light
    while (!m_apds.colorDataReady()) {
        delay(5);
    }

    getLightSensor()->readAmbientLight();
}

DisplayControl *HardwareFactoryYio::buildDisplayControl(const QVariantMap &config) {
    Q_UNUSED(config)
    return new DisplayControlYio(this);
}

BatteryFuelGauge *HardwareFactoryYio::buildBatteryFuelGauge(const QVariantMap &config) {
    Q_UNUSED(config)
    return new BQ27441(this);
}

InterruptHandler *HardwareFactoryYio::buildInterruptHandler(const QVariantMap &config) {
    Q_UNUSED(config)
    return new Mcp23017InterruptHandler(this);
}

HapticMotor *HardwareFactoryYio::buildHapticMotor(const QVariantMap &config) {
    Q_UNUSED(config)
    return new Drv2605(this);
}

GestureSensor *HardwareFactoryYio::buildGestureSensor(const QVariantMap &config) {
    Q_UNUSED(config)
    return new Apds9960GestureSensor(&m_apds, this);
}

LightSensor *HardwareFactoryYio::buildLightSensorr(const QVariantMap &config) {
    Q_UNUSED(config)
    return new Apds9960LightSensor(&m_apds, this);
}

ProximitySensor *HardwareFactoryYio::buildProximitySensor(const QVariantMap &config) {
    Q_UNUSED(config)
    return new Apds9960ProximitySensor(&m_apds, this);
}
