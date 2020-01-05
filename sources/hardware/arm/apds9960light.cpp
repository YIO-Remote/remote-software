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

#include "apds9960light.h"

#include <QLoggingCategory>
#include <QtDebug>

#include <cassert>

static Q_LOGGING_CATEGORY(CLASS_LC, "hw.dev.APDS9960.light");

Apds9960LightSensor::Apds9960LightSensor(APDS9960 *apds, QObject *parent)
    : LightSensor("APDS9960 light sensor", parent), p_apds(apds) {
    assert(apds);
}

int Apds9960LightSensor::readAmbientLight() {
    if (p_apds->isOpen()) {
        while (!p_apds->colorDataReady()) {
            delay(5);
        }

        m_ambientLight = p_apds->getAmbientLight();
        qCDebug(CLASS_LC) << "Lux:" << m_ambientLight;
    }

    return static_cast<int>(m_ambientLight);
}

const QLoggingCategory &Apds9960LightSensor::logCategory() const { return CLASS_LC(); }
