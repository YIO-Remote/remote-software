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
 * @brief Enum class for WiFi signal strength.
 * @details Dedicated class for improved QML usability based on https://qml.guide/enums-in-qt-qml/
 */
class SignalStrengthEnum {
    Q_GADGET

 public:
    enum Value {
        NONE,
        WEAK,
        OK,
        GOOD,
        EXCELLENT
    };
    Q_ENUM(Value)

    static Value fromRssi(int rssi) {
        // Calibration is based on "various Internet sources", taken from wpa_cute project
        if (rssi >= -60)
            return EXCELLENT;
        else if (rssi >= -68)
            return GOOD;
        else if (rssi >= -76)
            return OK;
        else if (rssi >= -84)
            return WEAK;
        else
            return NONE;
    }

 private:
    SignalStrengthEnum() {}
};

typedef SignalStrengthEnum::Value SignalStrength;
