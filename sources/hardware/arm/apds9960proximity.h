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

#pragma once

#include <QObject>
#include <QtDebug>

#include <cassert>

#include "../proximitysensor.h"
#include "apds9960.h"

class Apds9960ProximitySensor : public ProximitySensor {
    Q_OBJECT

 public:
    int proximitySetting() override { return m_proximitySetting; }

    void setProximitySetting(int proximity) override { m_proximitySetting = proximity; }

    int proximity() override { return m_proximity; }

    Q_INVOKABLE void proximityDetection(bool state) override {
        if (p_apds->isInitialized()) {
            if (state != m_proximityDetection) {
                if (state) {
                    // turn on
                    p_apds->enableProximityInterrupt();

                } else {
                    // turn off
                    p_apds->disableProximityInterrupt();
                }
            }
        }

        m_proximityDetection = state;
    }

    Q_INVOKABLE void readInterrupt() override {
        if (p_apds->isInitialized()) {
            m_proximity = p_apds->readProximity();
            if (m_proximity > 0) {
                // prevent log flooding while docking
                qDebug() << "Proximity" << m_proximity;
            }

            if (m_proximityDetection) {
                if (m_proximity > m_proximitySetting) {
                    // turn of proximity detection
                    qDebug() << "Proximity detected, turning detection off";
                    proximityDetection(false);

                    // let qml know
                    emit proximityEvent();
                }
            }

            // clear the interrupt
            p_apds->clearInterrupt();
        }
    }

 public:
    explicit Apds9960ProximitySensor(APDS9960* apds, QObject* parent = nullptr)
        : ProximitySensor(parent), p_apds(apds) {
        assert(apds);
    }

    ~Apds9960ProximitySensor() override {}

 private:
    APDS9960* p_apds;

    uint8_t m_proximity;
    bool    m_proximityDetection = false;
    int     m_proximitySetting = 70;  // default value
};
