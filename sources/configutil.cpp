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

#include "configutil.h"

QVariant ConfigUtil::getValue(QVariantMap const &map, QString const &path,
                          QVariant const &defaultValue /* = QVariant() */) {
    QStringList pathComponents = path.split("/");
    QVariantMap parent = map;

    while (pathComponents.size() > 0) {
        QString const &currentKey = pathComponents.takeFirst();

        if (parent.contains(currentKey)) {
            if (pathComponents.size() == 0) {
                return parent.value(currentKey);
            } else {
                parent = parent.value(currentKey).toMap();
            }
        } else {
            break;
        }
    }

    return defaultValue;
}

bool ConfigUtil::isEnabled(QVariantMap const& map, bool defaultValue /* = true */) {
    return map.value("enabled", defaultValue).toBool();
}

QVariant ConfigUtil::getValue(QJsonObject const& settings, QString const& path,
                              QVariant const &defaultValue /* = QVariant() */) {
    QStringList pathComponents = path.split("/");
    QJsonObject parent = settings;

    while (pathComponents.size() > 0) {
        QString const &currentKey = pathComponents.takeFirst();

        if (parent.contains(currentKey)) {
            if (pathComponents.size() == 0) {
                return parent.value(currentKey);
            } else {
                parent = parent.value(currentKey).toObject();
            }
        } else {
            break;
        }
    }

    return defaultValue;
}
