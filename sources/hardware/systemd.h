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

#ifndef SYSTEMD_H
#define SYSTEMD_H

#include <QObject>
#include <QMap>

#include "systemservice.h"

class Systemd : public SystemService
{
    Q_OBJECT
public:
    Systemd(QMap<SystemServiceName, QString> &serviceNameMap, QObject *parent = nullptr);

    // SystemService interface
public:
    virtual bool startService(SystemServiceName serviceName) override;
    virtual bool stopService(SystemServiceName serviceName) override;
    virtual bool restartService(SystemServiceName serviceName) override;
    virtual bool reloadService(SystemServiceName serviceName) override;

private:
    bool launch(const QString &command);

    QMap<SystemServiceName, QString> m_serviceNameMap;
};

#endif // SYSTEMD_H
