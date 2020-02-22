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

#include "apds9960proximity.h"

#include <QLoggingCategory>
#include <QtDebug>


static Q_LOGGING_CATEGORY(CLASS_LC, "hw.dev.APDS9960.proximity");

Apds9960ProximitySensor::Apds9960ProximitySensor(APDS9960 *apds, InterruptHandler* interruptHandler, QObject *parent)
    : ProximitySensor("APDS990 proximity sensor", parent), p_apds(apds) {
    Q_ASSERT(apds);
    Q_ASSERT(interruptHandler);
    qCDebug(CLASS_LC) << name();

    connect(interruptHandler, &InterruptHandler::interruptEvent, this, [&](int event) {
        if (event == InterruptHandler::APDS9960) {
            readInterrupt();
        }
    });
}

void Apds9960ProximitySensor::proximityDetection(bool state) {
    qCDebug(CLASS_LC) << "Proximity detection set to" << state;

    if (p_apds->isOpen()) {
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

void Apds9960ProximitySensor::readInterrupt() {
    if (p_apds->isOpen()) {
        m_proximity = p_apds->readProximity();
        if (m_proximity > 0) {
            // prevent log flooding while docking
            qCDebug(CLASS_LC) << "Proximity" << m_proximity;
        }

        if (m_proximityDetection) {
            if (m_proximity > m_proximitySetting) {
                // turn of proximity detection
                qCDebug(CLASS_LC) << "Proximity detected, turning detection off";
                proximityDetection(false);

                // let qml know
                emit proximityEvent();
            }
        }

        // clear the interrupt
        p_apds->clearInterrupt();
    }
}

const QLoggingCategory &Apds9960ProximitySensor::logCategory() const { return CLASS_LC(); }
