/******************************************************************************
 *
 * Copyright (C) 2020 Markus Zehnder <business@markuszehnder.ch>
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

/**
 * @brief Provides information about system resources.
 */
class SystemInfo : public QObject {
    Q_OBJECT

 public:
    explicit SystemInfo(QObject *parent = nullptr) : QObject(parent) {}

    Q_PROPERTY(QString uptime READ uptime)
    Q_PROPERTY(float usedMemory READ usedMemory)
    Q_PROPERTY(bool cpuTemperatureSupported READ cpuTemperatureSupported NOTIFY cpuTemperatureSupportChanged)
    Q_PROPERTY(float cpuTemperature READ cpuTemperature)
    Q_PROPERTY(float cpuLoadAverage READ cpuLoadAverage)

    virtual void init() = 0;

    /**
     * @brief Returns the system uptime in days, hours, minutes
     * @details Format: D d H h M m
     * Example: 0 d 8 h 45 m
     */
    virtual QString uptime() = 0;

    /**
     * @brief Returns the used system memory as percentage.
     * @details When possible only real memory is considered without swap.
     */
    virtual float usedMemory() = 0;

    /**
     * @brief Returns if CPU temperature reading is supported.
     * @details Not all platforms support temperature reading. This notifyable property allows to hide the temperature
     * label in the system menu.
     */
    virtual bool cpuTemperatureSupported() { return true; }

    /**
     * @brief Returns the current CPU temperature in Celcius
     */
    virtual float cpuTemperature() = 0;

    /**
     * @brief Returns the average CPU load as percentage.
     * @details Sampling is usually based on the duration between calls or the default sampling duration of the
     * underlaying system.
     */
    virtual float cpuLoadAverage() = 0;

 signals:
    void cpuTemperatureSupportChanged();
};
