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

#include "../../logger.h"
#include "../hardwarefactory.h"
#include "../proximitysensor.h"
#include "apds9960.h"

class Apds9960ProximitySensor : public ProximitySensor {
    Q_OBJECT

 public:
    int proximitySetting() override { return m_proximitySetting; }

    void setProximitySetting(int proximity) override { m_proximitySetting = proximity; }

    int proximity() override { return m_proximity; }

    Q_INVOKABLE void proximityDetection(bool state) override {
        qCDebug(m_log) << "Proximity detection set to" << state;

        if (p_apds->isOpen()) {
            if (state != m_proximityDetection) {
                if (state) {
                    // turn on
                    p_apds->enableProximityInterrupt();
                    qCDebug(m_log) << "Proximity detection enabled";

                } else {
                    // turn off
                    p_apds->disableProximityInterrupt();
                    qCDebug(m_log) << "Proximity detection disabled";
                }
            }
        }

        m_proximityDetection = state;
    }

    Q_INVOKABLE void readInterrupt() override {
        if (p_apds->isOpen()) {
            m_proximity = p_apds->readProximity();
            if (m_proximity > 0) {
                // prevent log flooding while docking
                qCDebug(m_log) << "Proximity" << m_proximity;
            }

            if (m_proximityDetection) {
                if (m_proximity > m_proximitySetting) {
                    // turn of proximity detection
                    qCDebug(m_log) << "Proximity detected, turning detection off";
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
        : ProximitySensor(parent), p_apds(apds), m_log("APDS9960 Proximity") {
        Q_ASSERT(apds);

        Logger::getInstance()->defineLogCategory(m_log.categoryName(), QtMsgType::QtDebugMsg, &m_log);
        InterruptHandler* interruptHandler = HardwareFactory::instance()->getInterruptHandler();

        connect(interruptHandler, &InterruptHandler::interruptEvent, this, [&](int event) {
            if (event == InterruptHandler::APDS9960) {
                readInterrupt();
            }
        });
    }

    ~Apds9960ProximitySensor() override {}

 private:
    APDS9960* p_apds;

    uint8_t m_proximity;
    bool    m_proximityDetection = false;
    int     m_proximitySetting   = 40;  // default value

    QLoggingCategory m_log;
};
