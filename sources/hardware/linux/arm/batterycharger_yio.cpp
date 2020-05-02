/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
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

#include "batterycharger_yio.h"

#include <wiringPi.h>

#include <QLoggingCategory>
#include <QtDebug>

static Q_LOGGING_CATEGORY(CLASS_LC, "hw.dev.battery");

BatteryChargerYio::BatteryChargerYio(int pin, QObject *parent)
    : BatteryCharger("YIO battery charger", parent), m_pin(pin) {
    Q_ASSERT(pin);
    qCDebug(CLASS_LC()) << name() << "on GPIO" << pin;
}

void BatteryChargerYio::batteryChargingOn() {
    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, LOW);
    qCDebug(CLASS_LC) << "Turning battery charging on";
}

void BatteryChargerYio::batteryChargingOff() {
    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, HIGH);
    qCDebug(CLASS_LC) << "Turning battery charging off";
}

const QLoggingCategory &BatteryChargerYio::logCategory() const { return CLASS_LC(); }
