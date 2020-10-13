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

#include <QtDebug>

Apds9960LightSensor::Apds9960LightSensor(APDS9960 *apds, QObject *parent)
    : LightSensor("APDS9960 light sensor", parent), p_apds(apds) {
    Q_ASSERT(apds);
    qCDebug(lcApds9960Light) << name();
}

int Apds9960LightSensor::readAmbientLight() {
    if (p_apds->isOpen()) {
        while (!p_apds->colorDataReady()) {
            delay(5);
        }

        m_ambientLight = p_apds->getAmbientLight();
        p_apds->getColorData(&m_r, &m_g, &m_b, &m_c);

        qCDebug(lcApds9960Light) << "Ambientlight:" << m_ambientLight;
        qCDebug(lcApds9960Light) << "Lux:" << calculateIlluminance(m_r, m_g, m_b);  // not really precise
    }

    return static_cast<int>(m_ambientLight);
}

const QLoggingCategory &Apds9960LightSensor::logCategory() const {
    return lcApds9960Light();
}

uint16_t Apds9960LightSensor::calculateIlluminance(uint16_t r, uint16_t g, uint16_t b) {
    float lux;
    lux = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
    return static_cast<uint16_t>(lux);
}
