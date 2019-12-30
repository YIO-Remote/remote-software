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

class ClimateDef : public QObject {
    Q_OBJECT
 public:
    enum Attributes { STATE, TEMPERATURE, TARGET_TEMPERATURE, TEMPERATURE_UNIT, TEMPERATURE_MAX, TEMPERATURE_MIN };
    Q_ENUM(Attributes)

    enum States { OFF = 0, ON = 1, HEAT = 2, COOL = 3 };
    Q_ENUM(States)

    enum Features { F_TEMPERATURE, F_TARGET_TEMPERATURE, F_TEMPERATURE_MAX, F_TEMPERATURE_MIN };
    Q_ENUM(Features)

    enum Commands { C_OFF, C_ON, C_HEAT, C_COOL, C_TARGET_TEMPERATURE };
    Q_ENUM(Commands)

    explicit ClimateDef(QObject* parent = nullptr) : QObject(parent) {}
};

/// This interface allows integrations to access the specific attributes
/// You get this interface from the generic entity using getSpecificInterface
class ClimateInterface {
 public:
    virtual ~ClimateInterface();

    virtual double  temperature() = 0;
    virtual double  targetTemperature() = 0;
    virtual QString temperatureUnit() = 0;
    virtual double  temperatureMax() = 0;
    virtual double  temperatureMin() = 0;
};

QT_BEGIN_NAMESPACE
#define ClimateInterface_iid "YIO.ClimateInterface"
Q_DECLARE_INTERFACE(ClimateInterface, ClimateInterface_iid)
QT_END_NAMESPACE
