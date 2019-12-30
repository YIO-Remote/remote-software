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

#include "climate.h"

#include <QQmlApplicationEngine>
#include <QtDebug>

QString Climate::Type = "climate";

ClimateInterface::~ClimateInterface() {}

void Climate::setTargetTemperature(double temp) { command(ClimateDef::C_TARGET_TEMPERATURE, temp); }

void Climate::heat(double temp) { command(ClimateDef::C_HEAT, temp); }

void Climate::cool(double temp) { command(ClimateDef::C_COOL, temp); }

bool Climate::updateAttrByIndex(int attrIndex, const QVariant &value) {
    bool chg = false;
    switch (attrIndex) {
        case ClimateDef::STATE:
            if (value.type() == QVariant::String)
                chg = setStateText(value.toString());
            else
                chg = setState(value.toInt());
            break;
        case ClimateDef::TEMPERATURE:
            if (m_temperature != value.toDouble()) {
                m_temperature = value.toDouble();
                chg = true;
                emit temperatureChanged();
            }
            break;
        case ClimateDef::TARGET_TEMPERATURE:
            if (m_targetTemperature != value.toDouble()) {
                m_targetTemperature = value.toDouble();
                chg = true;
                emit targetTemperatureChanged();
            }
            break;
    }
    return chg;
}

void Climate::turnOn() { command(ClimateDef::C_ON, ""); }

void Climate::turnOff() { command(ClimateDef::C_OFF, ""); }

bool Climate::isOn() { return m_state == ClimateDef::ON; }

Climate::Climate(const QVariantMap &config, IntegrationInterface *integrationObj, QObject *parent)
    : Entity(Type, config, integrationObj, parent), m_temperature(0), m_targetTemperature(0) {
    static QMetaEnum metaEnumAttr;
    static QMetaEnum metaEnumFeatures;
    static QMetaEnum metaEnumCommands;
    static QMetaEnum metaEnumState;

    if (!metaEnumAttr.isValid()) {
        int index = ClimateDef::staticMetaObject.indexOfEnumerator("Attributes");
        metaEnumAttr = ClimateDef::staticMetaObject.enumerator(index);
        index = ClimateDef::staticMetaObject.indexOfEnumerator("States");
        metaEnumState = ClimateDef::staticMetaObject.enumerator(index);
        index = ClimateDef::staticMetaObject.indexOfEnumerator("Features");
        metaEnumFeatures = ClimateDef::staticMetaObject.enumerator(index);
        index = ClimateDef::staticMetaObject.indexOfEnumerator("Commands");
        metaEnumCommands = ClimateDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<ClimateDef>("Entity.Climate", 1, 0, "Climate",
                                               "Not creatable as it is an enum type.");
    }
    m_enumAttr = &metaEnumAttr;
    m_enumFeatures = &metaEnumFeatures;
    m_enumCommands = &metaEnumCommands;
    m_enumState = &metaEnumState;
    m_specificInterface = qobject_cast<ClimateInterface *>(this);
    initializeSupportedFeatures(config);
}
