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

#ifndef HARDWARE_DISPLAYCONTROL_H_
#define HARDWARE_DISPLAYCONTROL_H_

#include <QObject>

class DisplayControl : public QObject {
    Q_OBJECT

 public:
    Q_INVOKABLE virtual bool setmode(const QString& mode) = 0;

    Q_INVOKABLE virtual void setBrightness(int from, int to) = 0;

    Q_INVOKABLE virtual void batteryChargingOn() = 0;
    Q_INVOKABLE virtual void batteryChargingOff() = 0;

    virtual void setup() = 0;

 protected:
    explicit DisplayControl(QObject* parent = nullptr) : QObject(parent) {}
};

#endif  // HARDWARE_DISPLAYCONTROL_H_
