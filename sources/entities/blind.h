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

#include <QMetaEnum>
#include <QObject>
#include <QString>
#include <QVariant>

#include "entity.h"
#include "yio-interface/entities/blindinterface.h"

class Blind : public Entity, BlindInterface {
    Q_OBJECT
    Q_INTERFACES(BlindInterface)

 public:
    Q_PROPERTY(int position READ position NOTIFY positionChanged)

    // update an entity's attributes

    // blind commands
    Q_INVOKABLE void close();
    Q_INVOKABLE void open();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void setPosition(int value);

    bool updateAttrByIndex(int attrIndex, const QVariant& value) override;

    void turnOn() override { open(); }
    void turnOff() override { close(); }
    int  position() override { return m_position; }
    bool isOn() override { return m_state == BlindDef::CLOSED; }
    bool supportsOn() override { return isSupported(BlindDef::F_POSITION); }

    Blind(const QVariantMap& config, IntegrationInterface* integrationObj, QObject* parent = nullptr);

 signals:
    void positionChanged();

 public:
    static QString Type;

 private:
    int m_position;
};
