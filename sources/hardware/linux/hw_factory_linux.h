/******************************************************************************
 *
 * Copyright (C) 2019-2020 Markus Zehnder <business@markuszehnder.ch>
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

#include "../hardwarefactory_default.h"

/**
 * @brief Generic Linux hardware factory implementation.
 * May be sub-classed by more specific hardware factories. E.g. RPi or YIO remote.
 */
class HardwareFactoryLinux : public HardwareFactoryDefault {
    Q_OBJECT

 public:
    explicit HardwareFactoryLinux(const QVariantMap &config, QObject *parent = nullptr);

    // HardwareFactory interface
 protected:
    bool buildDevices(const QVariantMap &config) override;

    // Helper methods
 protected:
    virtual WifiControl *     buildWifiControl(const QVariantMap &config);
    virtual SystemService *   buildSystemService(const QVariantMap &config);
    virtual WebServerControl *buildWebServerControl(const QVariantMap &config);
};
