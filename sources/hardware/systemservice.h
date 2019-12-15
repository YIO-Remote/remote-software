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

#ifndef SYSTEMSERVICE_H
#define SYSTEMSERVICE_H

#include <QObject>

#include "systemservice_name.h"

class SystemService : public QObject
{
    Q_OBJECT
public:
    explicit SystemService(QObject *parent = nullptr);

    /**
     * @brief startService Starts the given service
     * @param serviceName the name of the service
     * @return true if the service was started successfully
     */
    virtual bool startService(SystemServiceName serviceName) = 0;

    /**
     * @brief stopService Stopps the given service
     * @param serviceName the name of the service
     * @return true if the service was stopped successfully
     */
    virtual bool stopService(SystemServiceName serviceName) = 0;

    /**
     * @brief restartService Restarts the given service
     * @param serviceName the name of the service
     * @return true if the service was restarted successfully
     */
    virtual bool restartService(SystemServiceName serviceName);

    /**
     * @brief reloadService Reloads the configuration of the given service
     * @param serviceName the name of the service
     * @return true if the service was reloaded successfully
     */
    virtual bool reloadService(SystemServiceName serviceName);

signals:

};

#endif // SYSTEMSERVICE_H
