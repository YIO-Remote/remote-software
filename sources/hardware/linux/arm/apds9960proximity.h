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

#pragma once

#include "../../proximitysensor.h"
#include "../../interrupthandler.h"
#include "apds9960.h"

class Apds9960ProximitySensor : public ProximitySensor {
    Q_OBJECT

 public:
    explicit Apds9960ProximitySensor(APDS9960* apds, InterruptHandler* interruptHandler, QObject* parent = nullptr);

    int proximitySetting() override { return m_proximitySetting; }

    void setProximitySetting(int proximity) override { m_proximitySetting = proximity; }

    int proximity() override { return m_proximity; }

    Q_INVOKABLE void proximityDetection(bool state) override;

    Q_INVOKABLE void readInterrupt() override;

    // Device interface
 protected:
    const QLoggingCategory& logCategory() const override;

 private:
    APDS9960* p_apds;

    uint8_t m_proximity;
    bool    m_proximityDetection = false;
    int     m_proximitySetting   = 40;  // default value
};
