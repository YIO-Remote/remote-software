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

#include "webserver_mock.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "WebMock");

WebServerMock::WebServerMock(QObject *parent) : WebServerControl(parent)

{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}


bool WebServerMock::startService()
{
    qCDebug(CLASS_LC) << "startService";
    return true;
}

bool WebServerMock::stopService()
{
    qCDebug(CLASS_LC) << "stopService";
    return true;
}

bool WebServerMock::startWifiSetupPortal()
{
    qCDebug(CLASS_LC) << "startWifiSetupPortal";
    return true;
}

bool WebServerMock::startWebConfigurator()
{
    qCDebug(CLASS_LC) << "startWebConfiugorator";
    return true;
}
