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

#include "device.h"

// Error translation strings are defined here to include them on every build, independent of the platform!
static QString ERR_DEV_PROXIMITY_INIT =
    QObject::tr("Cannot initialize the proximity sensor. Please restart the remote.");
static QString ERR_DEV_PROXIMITY_COMM = QObject::tr("Battery sensor communication error. Please restart the remote.");

class ProximitySensor : public Device {
    Q_OBJECT

 public:
    Q_PROPERTY(int proximity READ proximity NOTIFY proximityEvent)
    Q_PROPERTY(int proximitySetting READ proximitySetting WRITE setProximitySetting)

    Q_INVOKABLE virtual void proximityDetection(bool state) = 0;
    Q_INVOKABLE virtual void readInterrupt() = 0;

    virtual int proximitySetting() = 0;

    virtual void setProximitySetting(int proximity) = 0;

    virtual int proximity() = 0;

 signals:
    void proximityEvent();

 protected:
    explicit ProximitySensor(QString name, QObject *parent = nullptr) : Device(name, parent) {}
};
