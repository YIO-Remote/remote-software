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

#pragma once

#include <QMetaEnum>
#include <QObject>
#include <QQmlListProperty>
#include <QString>
#include <QVariant>

#include "entity.h"
#include "yio-interface/entities/weatherinterface.h"

class QWeatherItem : public QObject, public WeatherItem {
    Q_OBJECT

 public:
    explicit QWeatherItem(QObject* parent) : QObject(parent) {}
    Q_PROPERTY(QString date READ date CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString imageurl READ imageurl CONSTANT)
    Q_PROPERTY(QString temp READ temp CONSTANT)
    Q_PROPERTY(QString rain READ rain CONSTANT)
    Q_PROPERTY(QString snow READ snow CONSTANT)
    Q_PROPERTY(QString wind READ wind CONSTANT)
    Q_PROPERTY(QString humidity READ humidity CONSTANT)
};

class Weather : public Entity, WeatherInterface {
    Q_OBJECT
    Q_INTERFACES(WeatherInterface)

 public:
    Q_PROPERTY(QWeatherItem* current READ current NOTIFY currentChanged)
    Q_PROPERTY(QObject* forecast READ forecast NOTIFY forecastChanged)

    // update an entity's attributes
    bool updateAttrByIndex(int attrIndex, const QVariant& value) override;

    QWeatherItem* current() { return &m_current; }
    QObject*      forecast() { return m_forecast; }
    bool          isOn() override { return m_state == WeatherDef::ONLINE; }
    bool          supportsOn() override { return false; }

    Weather(const QVariantMap& config, IntegrationInterface* integrationObj, QObject* parent = nullptr);

    void setForecast(QObject* model) override;
    void setCurrent(const WeatherItem& current) override;

 signals:
    void currentChanged();
    void forecastChanged();

 public:
    static QString Type;

 private:
    QWeatherItem m_current;
    QObject*     m_forecast;
};
