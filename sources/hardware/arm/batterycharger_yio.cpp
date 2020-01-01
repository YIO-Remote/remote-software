/******************************************************************************
 *
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

#include <QLoggingCategory>
#include <QtDebug>

#include <wiringPi.h>

#include "batterycharger_yio.h"

#define CLK 107
#define MOSI 106
#define CS 105
#define RST 104

static Q_LOGGING_CATEGORY(CLASS_LC, "BatCharger");

BatteryChargerYio::BatteryChargerYio(QObject *parent) : BatteryCharger(parent) { setup(); }

void BatteryChargerYio::setup() {
    wiringPiSetup();
}

void BatteryChargerYio::batteryChargingOn() {
    pinMode(108, OUTPUT);
    digitalWrite(108, LOW);
    qCDebug(CLASS_LC) << "Turning battery charging on";
}

void BatteryChargerYio::batteryChargingOff() {
    pinMode(108, OUTPUT);
    digitalWrite(108, HIGH);
    qCDebug(CLASS_LC) << "Turning battery charging off";
}
