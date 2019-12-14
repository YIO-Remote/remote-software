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

#include <QLoggingCategory>
#include <QtDebug>

#include "hardwarefactory_rpi0.h"
#include "systemservice_mock.h"

#if defined (CONFIG_WPA_SUPPLICANT)
    #include "wifi_wpasupplicant.h"
#elif defined (Q_OS_LINUX)
    #include "wifi_shellscripts.h"
#else
    #include "wifi_mock.h"
#endif

static Q_LOGGING_CATEGORY(CLASS_LC, "HwRpi0");

HardwareFactoryRPi0::HardwareFactoryRPi0(QObject *parent) : HardwareFactory(parent)
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}


WifiControl *HardwareFactoryRPi0::getWifiControl()
{
#if defined (CONFIG_WPA_SUPPLICANT)
    static WifiWpaSupplicant singleton;
#elif defined (Q_OS_LINUX)
    static WifiShellScripts singleton;
#else
    static WifiMock singleton;
#endif

    return &singleton;
}


SystemService *HardwareFactoryRPi0::getSystemService()
{
    static SystemServiceMock singleton;

    return &singleton;
}
