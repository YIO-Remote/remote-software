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

#include <QObject>
#include <QVariant>

/// This interface is implemented by the Entities object and used by integration DLLs to access the entities
class YioAPIInterface : public QObject {
    Q_OBJECT

 public:
    virtual ~YioAPIInterface() {}

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void sendMessage(QString message) = 0;

    // CONFIG MANIPULATION METHODS
    virtual QVariantMap getConfig() = 0;
    virtual bool        addEntityToConfig(QVariantMap entity) = 0;

    // NETWORK SERVICES DISCOVERY
    virtual void discoverNetworkServices() = 0;
    virtual void discoverNetworkServices(QString mdns) = 0;

 signals:
    void serviceDiscovered(QMap<QString, QVariantMap> services);
};

QT_BEGIN_NAMESPACE
#define YioAPIInterface_iid "YIO.YioAPIInterface"
Q_DECLARE_INTERFACE(YioAPIInterface, YioAPIInterface_iid)
QT_END_NAMESPACE
