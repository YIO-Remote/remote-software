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

#include "yio-interface/entities/climateinterface.h"
#include "entity.h"

class Climate : public Entity, ClimateInterface {
    Q_OBJECT
    Q_INTERFACES(ClimateInterface)

 public:
    static QString Type;

    Q_PROPERTY(double temperature READ temperature NOTIFY temperatureChanged)
    Q_PROPERTY(double targetTemperature READ targetTemperature NOTIFY targetTemperatureChanged)
    Q_PROPERTY(QString temperatureUnit READ temperatureUnit NOTIFY temperatureUnitChanged)
    Q_PROPERTY(double temperatureMax READ temperatureMax NOTIFY temperatureMaxChanged)
    Q_PROPERTY(double temperatureMin READ temperatureMin NOTIFY temperatureMinChanged)

    // climte commands
    Q_INVOKABLE void setTargetTemperature(int temp);
    Q_INVOKABLE void heat();
    Q_INVOKABLE void cool();

    // overrides from Entity class
    bool updateAttrByIndex(int attrIndex, const QVariant& value) override;

    void turnOn() override;
    void turnOff() override;
    bool isOn() override;
    bool supportsOn() override;

    // overrides from ClimateInterface
    double  temperature() override { return m_temperature; }
    double  targetTemperature() override { return m_targetTemperature; }
    QString temperatureUnit() override { return m_temperatureUnit; }
    double  temperatureMax() override { return m_temperatureMax; }
    double  temperatureMin() override { return m_temperatureMin; }

    explicit Climate(const QVariantMap& config, IntegrationInterface* integrationObj, QObject* parent = nullptr);

 signals:
    void temperatureChanged();
    void targetTemperatureChanged();
    void temperatureUnitChanged();
    void temperatureMaxChanged();
    void temperatureMinChanged();

 private:
    double  m_temperature;
    double  m_targetTemperature;
    QString m_temperatureUnit = "ºC";
    double  m_temperatureMax  = 35.0;
    double  m_temperatureMin  = 4.0;
};
