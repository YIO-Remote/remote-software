/******************************************************************************
 *
 * Copyright (C) 2019 Christian Riedl <ric@rts.co.at>
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

#include "weather.h"

#include <QQmlApplicationEngine>

WeatherInterface::~WeatherInterface() {}

QString Weather::Type = "weather";

bool Weather::updateAttrByIndex(int attrIndex, const QVariant& value) {
    bool chg = false;
    switch (attrIndex) {
        case WeatherDef::STATE:
            if (value.type() == QVariant::String)
                chg = setStateText(value.toString());
            else
                chg = setState(value.toInt());
            break;
        /*
        case WeatherDef::CURRENT:
            m_current = value.toMap();
            chg = true;
            emit currentChanged();
            break;
        case WeatherDef::FORECAST:
            m_forecast = value.toList();
            chg = true;
            emit forecastChanged();
            break;
        */
        default:
            break;
    }
    return chg;
}

Weather::Weather(const QVariantMap& config, IntegrationInterface* integrationObj, QObject* parent)
    : Entity(Type, config, integrationObj, parent), m_current(parent) {
    static QMetaEnum metaEnumAttr;
    static QMetaEnum metaEnumFeatures;
    static QMetaEnum metaEnumCommands;
    static QMetaEnum metaEnumState;
    if (!metaEnumAttr.isValid()) {
        int index        = WeatherDef::staticMetaObject.indexOfEnumerator("Attributes");
        metaEnumAttr     = WeatherDef::staticMetaObject.enumerator(index);
        index            = WeatherDef::staticMetaObject.indexOfEnumerator("States");
        metaEnumState    = WeatherDef::staticMetaObject.enumerator(index);
        index            = WeatherDef::staticMetaObject.indexOfEnumerator("Features");
        metaEnumFeatures = WeatherDef::staticMetaObject.enumerator(index);
        index            = WeatherDef::staticMetaObject.indexOfEnumerator("Commands");
        metaEnumCommands = WeatherDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<WeatherDef>("Entity.Weather", 1, 0, "Weather",
                                               "Not creatable as it is an enum type.");
    }
    m_enumAttr     = &metaEnumAttr;
    m_enumFeatures = &metaEnumFeatures;
    m_enumCommands = &metaEnumCommands;
    m_enumState    = &metaEnumState;
    m_forecast     = nullptr;
    initializeSupportedFeatures(config);
    m_specificInterface = qobject_cast<WeatherInterface*>(this);
}
void Weather::setCurrent(const WeatherItem& current) {
    m_current.setDate(current.date());
    m_current.setDescription(current.description());
    m_current.setImageurl(current.imageurl());
    m_current.setTemp(current.temp());
    m_current.setRain(current.rain());
    m_current.setSnow(current.snow());
    m_current.setWind(current.wind());
    m_current.setHumidity(current.humidity());

    emit currentChanged();
}
void Weather::setForecast(QObject* model) {
    m_forecast = model;
    emit forecastChanged();
}
