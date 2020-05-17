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

QString Climate::Type = "climate";

ClimateInterface::~ClimateInterface() {}

void Climate::setTargetTemperature(int temp) { command(ClimateDef::C_TARGET_TEMPERATURE, temp); }

void Climate::heat() { command(ClimateDef::C_HEAT, ""); }

void Climate::cool() { command(ClimateDef::C_COOL, ""); }

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
                chg           = true;
                emit temperatureChanged();
            }
            break;
        case ClimateDef::TARGET_TEMPERATURE:
            if (m_targetTemperature != value.toDouble()) {
                m_targetTemperature = value.toDouble();
                chg                 = true;
                emit targetTemperatureChanged();
            }
            break;
        case ClimateDef::TEMPERATURE_UNIT:
            if (m_temperatureUnit != value.toString()) {
                m_temperatureUnit = value.toString();
                chg               = true;
                emit temperatureUnitChanged();
            }
            break;
        case ClimateDef::TEMPERATURE_MAX:
            if (m_temperatureMax != value.toDouble()) {
                m_temperatureMax = value.toDouble();
                chg              = true;
                emit temperatureMaxChanged();
            }
            break;
        case ClimateDef::TEMPERATURE_MIN:
            if (m_temperatureMin != value.toDouble()) {
                m_temperatureMin = value.toDouble();
                chg              = true;
                emit temperatureMinChanged();
            }
            break;
    }
    return chg;
}

void Climate::turnOn() { command(ClimateDef::C_ON, ""); }

void Climate::turnOff() { command(ClimateDef::C_OFF, ""); }

bool Climate::isOn() { return m_state == ClimateDef::ON || m_state == ClimateDef::HEAT || m_state == ClimateDef::COOL; }

bool Climate::supportsOn() {
    if (isSupported(ClimateDef::F_OFF)) {
        return true;
    } else {
        return false;
    }
}

Climate::Climate(const QVariantMap &config, IntegrationInterface *integrationObj, QObject *parent)
    : Entity(Type, config, integrationObj, parent), m_temperature(0), m_targetTemperature(0) {
    static QMetaEnum metaEnumAttr;
    static QMetaEnum metaEnumFeatures;
    static QMetaEnum metaEnumCommands;
    static QMetaEnum metaEnumState;

    if (!metaEnumAttr.isValid()) {
        int index        = ClimateDef::staticMetaObject.indexOfEnumerator("Attributes");
        metaEnumAttr     = ClimateDef::staticMetaObject.enumerator(index);
        index            = ClimateDef::staticMetaObject.indexOfEnumerator("States");
        metaEnumState    = ClimateDef::staticMetaObject.enumerator(index);
        index            = ClimateDef::staticMetaObject.indexOfEnumerator("Features");
        metaEnumFeatures = ClimateDef::staticMetaObject.enumerator(index);
        index            = ClimateDef::staticMetaObject.indexOfEnumerator("Commands");
        metaEnumCommands = ClimateDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<ClimateDef>("Entity.Climate", 1, 0, "Climate",
                                               "Not creatable as it is an enum type.");
    }
    m_enumAttr          = &metaEnumAttr;
    m_enumFeatures      = &metaEnumFeatures;
    m_enumCommands      = &metaEnumCommands;
    m_enumState         = &metaEnumState;
    m_specificInterface = qobject_cast<ClimateInterface *>(this);
    initializeSupportedFeatures(config);

    Config *   configObj = Config::getInstance();
    UnitSystem us        = configObj->getUnitSystem();
    if (us == Config::METRIC) {
        m_temperatureUnit = "ºC";
    } else {
        m_temperatureUnit = "ºF";
    }
    emit temperatureUnitChanged();

    QObject::connect(configObj, &Config::unitSystemChanged, this, [=]() {
        if (configObj->getUnitSystem() == Config::METRIC) {
            m_temperatureUnit = "ºC";
        } else {
            m_temperatureUnit = "ºF";
        }
        emit temperatureUnitChanged();
    });
}
