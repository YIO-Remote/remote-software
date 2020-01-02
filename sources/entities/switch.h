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
#include <QVariant>

#include "entity.h"
#include "switchinterface.h"

class Switch : public Entity, SwitchInterface {
    Q_OBJECT
    Q_INTERFACES(SwitchInterface)

 public:
    Q_PROPERTY(int power READ power NOTIFY powerChanged)

    // switch commands
    Q_INVOKABLE void toggle();

    bool updateAttrByIndex(int attrIndex, const QVariant& value) override;

    void turnOn() override;
    void turnOff() override;
    int  power() override { return m_power; }
    bool isOn() override { return m_state == SwitchDef::ON; }

    explicit Switch(const QVariantMap& config, IntegrationInterface* integrationObj, QObject* parent = nullptr);

    static QString Type;

 signals:
    void powerChanged();

 private:
    int m_power;
};
