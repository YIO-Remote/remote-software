/******************************************************************************
 *
 * Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
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
 * @brief Enum class for system services
 * @details Dedicated class for clearly separated enums and improved QML usability based on
 * https://www.embeddeduse.com/2017/09/15/passing-enum-properties-between-c-and-qml/
 *
 * Note: the "typedef style" described in https://qml.guide/enums-in-qt-qml/ unfortunately doesn't work with
 * "=== Strict Equality Comparison" in QML!
 */
class SystemServiceName {
    Q_GADGET

 public:
    enum Enum {
        WIFI,
        WEBSERVER,
        DHCP,
        NAME_RESOLUTION,
        NETWORKING,
        NTP,
        ZEROCONF
    };
    Q_ENUM(Enum)

 private:
    SystemServiceName() {}
};
