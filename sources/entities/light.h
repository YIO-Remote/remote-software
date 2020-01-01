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

#include <QColor>
#include <QMetaEnum>
#include <QObject>
#include <QString>
#include <QVariant>

#include "entity.h"
#include "lightinterface.h"

class Light : public Entity, LightInterface {
    Q_OBJECT
    Q_INTERFACES(LightInterface)

 public:
    Q_PROPERTY(int brightness READ brightness NOTIFY brightnessChanged)
    Q_PROPERTY(QColor color READ color NOTIFY colorChanged)
    Q_PROPERTY(int colorTemp READ colorTemp NOTIFY colorTempChanged)

    // light commands
    Q_INVOKABLE void toggle();
    Q_INVOKABLE void setBrightness(int value);
    Q_INVOKABLE void setColor(QColor value);
    Q_INVOKABLE void setColorTemp(int value);

    bool updateAttrByIndex(int attrIndex, const QVariant& value) override;

    void   turnOn() override;
    void   turnOff() override;
    int    brightness() override { return m_brightness; }
    QColor color() override { return m_color; }
    int    colorTemp() override { return m_colorTemp; }
    bool   isOn() override { return m_state == LightDef::ON; }

    explicit Light(const QVariantMap& config, IntegrationInterface* integrationObj, QObject* parent = nullptr);

 signals:
    void brightnessChanged();
    void colorChanged();
    void colorTempChanged();

 public:
    static QString Type;

 private:
    QColor m_color;
    int    m_brightness;
    int    m_colorTemp;
};
