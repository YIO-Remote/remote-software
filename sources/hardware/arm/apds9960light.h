/******************************************************************************
 *
 * Copyright (C) 2019 Foo Bar <foo@bar.com>
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

#pragma once

#include <QObject>
#include <QtDebug>

#include <cassert>

#include "../lightsensor.h"
#include "apds9960.h"

class Apds9960LightSensor : public LightSensor {
    Q_OBJECT

 public:
    int ambientLight() override { return static_cast<int>(m_ambientLight); }

    Q_INVOKABLE int readAmbientLight() override {
        if (p_apds->isOpen()) {
            while (!p_apds->colorDataReady()) {
                delay(5);
            }

            m_ambientLight = p_apds->getAmbientLight();
            qDebug() << "Lux:" << m_ambientLight;
        }

        return static_cast<int>(m_ambientLight);
    }

    explicit Apds9960LightSensor(APDS9960* apds, QObject* parent = nullptr) : LightSensor(parent), p_apds(apds) {
        assert(apds);
    }

    ~Apds9960LightSensor() override {}

 private:
    APDS9960* p_apds;

    uint16_t m_ambientLight = 100;
};
