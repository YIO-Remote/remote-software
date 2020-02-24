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

#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QVariant>

/**
 * @brief The ConfigUtil class contains common configuration handling utility functions.
 */
class ConfigUtil {
 public:
    /**
     * @brief Retrieve the value of a nested key specified in path
     * @param map Configuration map
     * @param path Path to the key, where individual keys are separated by "/"
     * @param defaultValue Value to return when the key / it's parents are not found in the settings object
     * @return Value if found, else defaultValue
     */
    static QVariant getValue(QVariantMap const& map, QString const& path, QVariant const& defaultValue = QVariant());

    /**
     * @brief Utility method to check if the given map has an 'enabled' key with value 'true'.
     * Default is true unless the 'defaultValue' parameter is specified.
     * @param map Configuration map
     * @param defaultValue Value to return when the key is not found
     * @return true if the map contains an entry "enabled"=true
     */
    static bool isEnabled(QVariantMap const& map, bool defaultValue = true);

    /**
     * @brief Retrieve the value of a nested key specified in path
     * @param settings Source json object
     * @param path Path to the key, where individual keys are separated by "/"
     * @param defaultValue Value to return when the key / it's parents are not found in the settings object
     * @return Value if found, else defaultValue
     */
    static QVariant getValue(QJsonObject const& settings, QString const& path,
                             QVariant const& defaultValue = QVariant());
};
