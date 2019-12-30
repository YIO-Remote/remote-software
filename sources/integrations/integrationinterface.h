/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
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

#include <QVariant>

class IntegrationInterface {
 public:
    enum States { CONNECTED = 0, CONNECTING = 1, DISCONNECTED = 2 };

    virtual ~IntegrationInterface();

    virtual void connect() = 0;       // Must be implemented by integration
    virtual void disconnect() = 0;    // Must be implemented by integration
    virtual void enterStandby() = 0;  // Can be overriden by integration
    virtual void leaveStandby() = 0;  // Can be overriden by integration
    virtual void sendCommand(const QString& type, const QString& entity_id, int command, const QVariant& param) = 0;

    virtual int     state() = 0;
    virtual QString integrationId() = 0;
    virtual QString friendlyName() = 0;
};

QT_BEGIN_NAMESPACE
#define IntegrationInterface_iid "YIO.IntegrationInterface"
Q_DECLARE_INTERFACE(IntegrationInterface, IntegrationInterface_iid)
QT_END_NAMESPACE
