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

#ifndef HARDWAREFACTORYRPI0_H
#define HARDWAREFACTORYRPI0_H

#include "hardwarefactory.h"

class HardwareFactoryRPi0 : public HardwareFactory
{
    Q_OBJECT
public:
    HardwareFactoryRPi0(ConfigInterface *config, QObject* parent = nullptr);

    // HardwareFactory interface
public:
    virtual WifiControl *getWifiControl() override;

    virtual SystemService *getSystemService() override;

private:
    WifiControl   *p_wifiControl;
    SystemService *p_systemService;
};

#endif // HARDWAREFACTORYRPI0_H
