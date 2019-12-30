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

#include "arm/bq27441.h"
#include "arm/displaycontrol_yio.h"
#include "arm/mcp23017_interrupt.h"
#include "hardwarefactory_yio.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "HwRpi0");

HardwareFactoryYio::HardwareFactoryYio(const QVariantMap &config, QObject *parent)
    : HardwareFactoryRPi0(config, parent) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
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
